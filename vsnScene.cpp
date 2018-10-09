//
// vsnScene
//
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/config.h"

#include <sstream>
#include <assert.h>

#include "vsnScene.h"
#include "vsnError.h"

#include "vsnData_Sph.h"
#include "vsnData_SphPEX.h"
#include "vsnData_P3dF.h"
#include "vsnData_FdvStr.h"
#include "vsnData_Dfi.h"
#include "vsnData_DfiSv.h"
#include "vsnData_OctVol.h"
#include "vsnData_Scatter.h"
#include "vsnData_TriaCells.h"
#include "vsnData_Shape.h"
#include "vsnData_ShapeLOD.h"
#include "vsnVolumeRender.h"

#include "vsnMethod_Shape_scalarMap.h"

#ifdef PGSQL
#include "vsnData_SvSQL.h"
#endif // PGSQL

using namespace std;
using namespace CES;
using namespace VSN;

//----------------------------------------------------------------
// class vsnScene
//----------------------------------------------------------------

/* static members */

bool vsnScene::s_xformSimpleRender = true;


/* constructors / destructor */

vsnScene::vsnScene(const std::string& name)
  : vfrGroup(name, FALSE), vsnIoObject(), m_lightAttr(this),
    m_frontObjGrp(NULL), p_lastLoadData(NULL), m_xforming(false)
{
  m_frontObjGrp = new vfrGroup("FRONT_GRP");
  assert(m_frontObjGrp);
}

vsnScene::~vsnScene() {
  register int n = getNumChildren();
  while ( n > 0 ) {
    vfrNode* p = getChild(n - 1);
    if ( p ) delete p;
    n = getNumChildren();
  }

  if ( m_frontObjGrp )
    delete m_frontObjGrp;
}


/* methods */

void vsnScene::reset() {
  register int n = getNumChildren();
  while ( n > 0 ) {
    vfrNode* p = getChild(n - 1);
    if ( p ) delete p;
    n = getNumChildren();
  }

  m_lightAttr.resetLight();
  m_frontObjGrp->remAllChildren();
  p_lastLoadData = NULL;
}


/* DataObj interface */

size_t vsnScene::getNumDataObj() const {
  register size_t i, ndo = 0;
  for ( i = 0; i < nChild; i++ )
    if ( dynamic_cast<vsnDataObj*>(_children[i]) ) ndo++;
  return ndo;
}

vsnDataObj* vsnScene::getDataObj(const size_t n) {
  register size_t i, ndo = 0;
  for ( i = 0; i < nChild; i++ ) {
    vsnDataObj* pdo = dynamic_cast<vsnDataObj*>(_children[i]);
    if ( pdo ) {
      if ( n == ndo )
	return pdo;
      else
	ndo++;
    }
  }
  return NULL;
}


/* ObjGroup interface */

bool vsnScene::addNewObjGroup(const std::string& name) {
  string newName(name);
  if ( name.empty() || name == string(VFR_NONAME) ) {
    size_t nog = getNumObjGroup();
    stringstream ss;
    ss << string("ObjGrp_") << nog;
    newName = ss.str();
  }

  vsnObjGroup* og = new vsnObjGroup(newName);
  if ( ! og ) return false;
  return addObjGroup(og);
}

size_t vsnScene::getNumObjGroup() const {
  register size_t i, nog = 0;
  for ( i = 0; i < nChild; i++ )
    if ( dynamic_cast<vsnObjGroup*>(_children[i]) ) nog++;
  return nog;
}

vsnObjGroup* vsnScene::getObjGroup(const size_t n) {
  register size_t i, nog = 0;
  for ( i = 0; i < nChild; i++ ) {
    vsnObjGroup* og = dynamic_cast<vsnObjGroup*>(_children[i]);
    if ( og ) {
      if ( n == nog ) return og;
    } else {
      nog++;
    }
  }
  return NULL;
}


/* FrontObj interface */

size_t vsnScene::getNumFrontObj() const {
  return (size_t)m_frontObjGrp->getNumChildren();
}

vsnFrontObj* vsnScene::getFrontObj(const size_t n) {
  return dynamic_cast<vsnFrontObj*>(m_frontObjGrp->getChild(n));
}


/* Volume renderer interface */

std::multimap<float, vsnVolumeRender*>
vsnScene::getSortedVolRenLst(const CES::Mat4<float>& MVM) {
  multimap<float, vsnVolumeRender*> svrLst;

  map<vsnVolumeRender*, vsnMethodObj*>::iterator it;
  for ( it = m_pVolRenLst.begin(); it != m_pVolRenLst.end(); it++ ) {
    if ( ! it->first ) continue;
    if ( it->second ) {
      if ( it->second->getRenderMode() == RT_NONE ) continue;
      vsnDataObj* pdo = it->second->getRefData();
      if ( ! pdo || pdo->getRenderMode() == RT_NONE ) continue;
    }

    CES::Mat4<float> M;
    if ( ! accumMatrix(it->first->getID(), M) ) continue;
    M = MVM * M;
    const CES::Vec3<float>* pbb = it->first->getBbox();
    CES::Vec3<float> c = (pbb[0] + pbb[1]) * 0.5f;
    c = M * c;
    CES::Vec3<float> x = M * pbb[1];
    float r = (x - c).Length();

    svrLst.insert(std::make_pair(c.Length() - r, it->first));
  } // end of for(i)

  return svrLst;
}

void vsnScene::addVolRender(vsnVolumeRender* pvr, vsnMethodObj* ppar) {
  if ( pvr && ppar ) m_pVolRenLst.insert(make_pair(pvr, ppar));
  
}

void vsnScene::delVolRender(vsnVolumeRender* pvr, vsnMethodObj* ppar) {
  map<vsnVolumeRender*, vsnMethodObj*>::iterator it
    = m_pVolRenLst.find(pvr);
  if ( it == m_pVolRenLst.end() ) return;
  m_pVolRenLst.erase(it);
}


/* override vfrGroup methods */

void vsnScene::render(const Bool transpMode, vfrMaterialStack* mstk) {
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  if ( mstk && _material )
    mstk->push(_material);

  RenderType renderMode = getRenderMode();
  if ( renderMode == RT_NONE ) {
    if ( mstk && _material )
      mstk->pop();
    return;
  }

  if ( _pickable & PT_OBJECT )
    glPushName((GLuint)_currentSeq);

  // Apply local coodinate
  applyMatrix();

  // Apply material
  const vfrMaterial* mp = getMaterial();
  if ( mp )
    mp->apply();

  // Rendering Children
  register int c;
  for ( c = 0; c < nChild; c++ ) {
    _children[c]->render(transpMode, mstk);
  }

  // Un-Apply material
  if ( mp )
    mp->unApply();

  // Draw Bounding Box
  if ( _pickable & PT_BBOX || !_pickable )
    ;
  else
    glLoadName((GLuint)0);
  drawBbox();

  if ( _pickable & PT_OBJECT )
    glPopName();

  unApplyMatrix();

  // Draw Volume datas
  if ( transpMode ) {
    CES::Mat4<float> MVM; glGetFloatv(GL_MODELVIEW_MATRIX, MVM.m_v);
    multimap<float, vsnVolumeRender*> svrLst = getSortedVolRenLst(MVM);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    multimap<float, vsnVolumeRender*>::reverse_iterator rit;
    for ( rit = svrLst.rbegin(); rit != svrLst.rend(); rit++ ) {
      vsnVolumeRender* pvr = rit->second;
      CES::Mat4<float> M;
      if ( accumMatrix(pvr->getID(), M) )
	pvr->DrawVolume(M);
    } // end of for(rit)
  }

  if ( mstk && _material )
    mstk->pop();
}


/* from vsnIoObject */

bool vsnScene::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  if ( ! xnp ) return false;

  // is 'scene' node?
  if ( strcmp((const char*)xnp->name, "scene") ) return false;

  // set name
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( xs && strlen((const char*)xs) > 0 &&
       strcmp(VFR_NONAME, (const char*)xs) ) {
    string scName = (const char*)xs;
    if ( scName != getName() ) {
      vsnScene* psc = vsnApp::GetApp()->getScene(scName);
      if ( ! psc )
	setName(scName);
    }
  }

  // get children node
  xmlNodePtr cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;

    if ( !strcmp((const char*)cur->name, "data") ) {
      if ( ! importDataXML(cur) )
	goto _NEXT_XML_NODE;
    } // end of if(data node)

    else if ( !strcmp((const char*)cur->name, "light") ) {
      if ( ! m_lightAttr.parseXML(cur) )
	    goto _NEXT_XML_NODE;
    } // end of "light"

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnScene::outputXML(std::ostream& os, const size_t ts) {
  std::string idts;
  register size_t i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  string outName = getName();
  if ( outName == VFR_NONAME ) outName = "";

  // output
  os << idts << "<scene";
  if ( ! outName.empty() )
    os << " name=\"" << outName << "\"";
  os << ">" << std::endl;

  bool ret = true;

  // output light
  if ( ! m_lightAttr.isStandardLight() ) {
    if ( m_lightAttr.outputXML(os, ts+2) )
      ret = false;
  }

  // output data
  size_t ndo = getNumDataObj();
  for ( i = 0; i < ndo; i++ ) {
    vsnDataObj* pdo = getDataObj(i);
    if ( ! pdo ) continue;
    if ( ! pdo->outputXML(os, ts+2) )
      ret = false;
  } // end of for(i)
  
  // output obj-group
  size_t nog = getNumObjGroup();
  for ( i = 0; i < nog; i++ ) {
    vsnObjGroup* pog = getObjGroup(i);
    if ( ! pog ) continue;
    if ( ! pog->outputXML(os, ts+2) )
      ret = false;
  } // end of for(i)

  os << idts << "</scene>" << std::endl;

  return ret;
}

bool vsnScene::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = string("Scene[") + getName() + string("]: commandXML: ");

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is my command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( string((const char*)xs) != getName() ) return false;

  // get command name
  string nameStr, valueStr;
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'name' property");
    return false;
  }
  nameStr = string((const char*)xs);

  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 0 )
    valueStr = string((const char*)xs);

  // do the command
  if ( nameStr == "set_name" ) {
    if ( valueStr == getName() ) return true;
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command set_name: no value"));
      return false;
    }
    vsnScene* posc = vsnApp::GetApp()->getScene(valueStr);
    if ( posc ) {
      ErrMsg(MsgERR, msgHdr + "command set_name: the scene named " +
	     valueStr + " has already exists");
      return false;
    }
    setName(valueStr);
  } // end of 'set_name'
  else if ( nameStr == "import_data" ) {
    xmlNodePtr cur;
    for ( cur = xnp->xmlChildrenNode; cur; cur = cur->next ) {
      if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
      if ( strcmp((const char*)cur->name, "data") ) continue;
      if ( ! importDataXML(cur) ) continue;
    } // end of for(cur)
  } // end of 'import_data'
  else if ( nameStr == "delete_data" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr+string("command delete_data: no value"));
      return false;
    }
    vsnDataObj* pdo = dynamic_cast<vsnDataObj*>(getNode(valueStr));
    if ( ! pdo ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command delete_data: can't find data: ") + valueStr);
      return false;
    }
    delete pdo;
  } // // end of 'delete_data'
  else if ( nameStr == "set_light_type" ) {
    vsnLightAttribute::LightType lt;
    if ( valueStr == "bi-directional" || valueStr == "bidirectional" )
      lt = vsnLightAttribute::Light_BiDirection;
    else if ( valueStr == "directional" )
      lt = vsnLightAttribute::Light_Direction;
    else if ( valueStr == "point" )
      lt = vsnLightAttribute::Light_Point;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_light_type: invalid light type: ") +valueStr);
      return false;
    }
    m_lightAttr.setLightType(lt);
  } // // end of 'set_light_type'
  else if ( nameStr == "set_light_xyz" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_light_xyz: no value"));
      return false;
    }
    vector3 xyz;
    int ns = sscanf(valueStr.c_str(), "%f %f %f", &xyz[0], &xyz[1], &xyz[2]);
    if ( ns < 3 ) {
      ErrMsg(MsgERR, msgHdr +string("command set_light_xyz: lack of values"));
      return false;
    }
    if ( m_lightAttr.getLightType() == vsnLightAttribute::Light_Point )
      m_lightAttr.setPosition(CES::Vec3<float>(xyz));
    else
      m_lightAttr.setDirection(CES::Vec3<float>(xyz));
  } // // end of 'set_light_xyz'
  else if ( nameStr == "set_light_color" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_light_xyz: no value"));
      return false;
    }
    vector4 lc = {1.f, 1.f, 1.f, 1.f};
    int ns = sscanf(valueStr.c_str(), "%f %f %f", &lc[0], &lc[1], &lc[2]);
    if ( ns < 3 ) {
      ErrMsg(MsgERR, msgHdr+string("command set_light_color: lack of values"));
      return false;
    }
    m_lightAttr.setColor(lc);
  } // // end of 'set_light_color'
  else {
    // not 'scene' command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}

// util method for XML (private)
bool vsnScene::importDataXML(xmlNodePtr cur) {
  static xmlChar* xs;
  if ( ! cur ) return false;
  if ( strcmp((const char*)cur->name, "data") ) return false;
  string msgHdr = "Scene: importDataXML: ";
  p_lastLoadData = NULL;

  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;

  // check data-type
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(cur, (const xmlChar*)"type");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid data: no type specified"));
    return false;
  }

  string dataType((const char*)xs);
  vsnDataObj* pdata = NULL;
  if ( !strcmp((const char*)xs, "Sph") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_Sph());
  } // end of if(Sph data)
  else if ( !strcmp((const char*)xs, "P3dF") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_P3dF());
  } // end of if(P3dF data)
  else if ( !strcmp((const char*)xs, "FdvStr") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_FdvStr());
  } // end of if(FdvStr data)
  else if ( !strcmp((const char*)xs, "Dfi") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_Dfi());
  } // end of if(Dfi data)
  else if ( !strcmp((const char*)xs, "DfiSv") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_DfiSv());
  } // end of if(DfiSv data)
  else if ( !strcmp((const char*)xs, "OctVol") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_OctVol());
  } // end of if(OctVol data)
  else if ( !strcmp((const char*)xs, "Scatter") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_Scatter());
  } // end of if(Scatter data)
  else if ( !strcmp((const char*)xs, "TriaCells") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_TriaCells());
  } // end of if(TriaCells data)
  else if ( !strcmp((const char*)xs, "Shape") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_Shape());
  } // end of if(Shape data)
  else if ( !strcmp((const char*)xs, "ShapeLOD") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_ShapeLOD());
  } // end of if(ShapeLOD data)
#ifdef PGSQL
  else if ( !strcmp((const char*)xs, "SvSQL") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_SvSQL());
  } // end of if(SvSQL data)
#endif // PGSQL
  else if ( !strcmp((const char*)xs, "SphPEX") ) {
    pdata = dynamic_cast<vsnDataObj*>(new vsnData_SphPEX());
  } // end of if(SphPEX data)
  else {
    ErrMsg(MsgERR, msgHdr + string((const char*)xs)
	   + string(" type data is not implemented"));
    return false;
  }

  if ( ! pdata ) {
    ErrMsg(MsgERR, msgHdr
	   + string("create ") + dataType + string(" data failed"));
    return false;
  }

  if ( ! addChild(pdata) ) {
    ErrMsg(MsgERR, msgHdr
	   + string("can't add ") + dataType + string(" data to Scene"));
    delete pdata;
    return false;
  }

  if ( ! pdata->parseXML(cur) ) {
    delete pdata;
    return false;
  }

  if ( pdata->getNumMethod() < 1 )
    pdata->setBboxShowMode(TRUE);

  if ( getNumDataObj() == 1 ) {
    size_t i, nvf = pApp->getNumViewFrame();
    for ( i = 0; i < nvf; i++ ) {
      vsnViewFrame* pvf = pApp->getViewFrame(i);
      if ( ! pvf || pvf->getScene() != this ) continue;
      pvf->getGfxView()->normalize();
    } // end of for(i)
  }

  if ( pdata->hasMultiFiles() ) {
    deque<string> fLst = pdata->getFileList();
    if ( fLst.size() > 0 )
      pApp->setImportDir(DirName(fLst[0], vsnPath_getDelimChar()));
  } else {
    string path = pdata->getFilePath();
    pApp->setImportDir(DirName(path, vsnPath_getDelimChar()));
  }
  p_lastLoadData = pdata;

  // MHIR append begin
  {
    for (int i = 0; i < getNumChildren(); ++i) {
      vsnData_Shape* shape = dynamic_cast<vsnData_Shape*>(getChild(i));
      if (shape) {
	for (int j = 0; j < shape -> getNumMethod(); ++j) {
	  vsnMethod_Shape_scalarMap* method
	    = dynamic_cast<vsnMethod_Shape_scalarMap*>(shape -> getMethod(j));
	  if (method) {
	    method -> viewByParam();
	  }
	}
      }
    }
  }
  // MHIR append end

  return true;
}

/* static methods */

void vsnScene::SetXformSimpleRender(const bool mode) {
  s_xformSimpleRender = mode;
}

bool vsnScene::GetXformSimpleRender() {
  return s_xformSimpleRender;
}

void vsnScene::ReadConfig() {
  wxConfigBase* pConfig = wxConfigBase::Get();
  if ( ! pConfig ) return;
  s_xformSimpleRender =
      pConfig->Read(wxT("/Scene/Xform/SimpleRender"), s_xformSimpleRender);
}

void vsnScene::WriteConfig() {
  wxConfigBase* pConfig = wxConfigBase::Get();
  if ( ! pConfig ) return;
  pConfig->Write(wxT("/Scene/Xform/SimpleRender"), s_xformSimpleRender);
}

