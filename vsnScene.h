//
// vsnScene
//
#ifndef _VSN_SCENE_H_
#define _VSN_SCENE_H_

#include "vfrGroup.h"
#include "vsnIoObject.h"
#include "vsnObjGroup.h"
#include "vsnFrontObj.h"
#include "vsnLightAttribute.h"
#include <map>


//----------------------------------------------------------------
// class vsnScene
//----------------------------------------------------------------
class vsnScene : public vfrGroup, public vsnIoObject {
public:
  vsnScene(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnScene();

  void reset();

  // ObjGroup interface
  bool addNewObjGroup(const std::string& name =std::string(VFR_NONAME));
  bool addObjGroup(vsnObjGroup* pGrp) {return addChild(pGrp) ? true : false;}
  bool delObjGroup(vsnObjGroup* pGrp) {return remChild(pGrp) ? true : false;}
  size_t getNumObjGroup() const;
  vsnObjGroup* getObjGroup(const size_t n);

  // FrontObj interface
  bool addFrontObj(vsnFrontObj* pfo) {
    return m_frontObjGrp->addChild(pfo) ? true : false;
  }
  bool delFrontObj(vsnFrontObj* pfo) {
    return m_frontObjGrp->remChild(pfo) ? true : false;
  }
  size_t getNumFrontObj() const;
  vsnFrontObj* getFrontObj(const size_t n);
  vfrGroup* getFrontObjGrp() {return m_frontObjGrp;}

  // DataObj interface
  class vsnDataObj* getLastLoadedData() const {return p_lastLoadData;}

  // volume render list interface
  void addVolRender(class vsnVolumeRender* pvr, class vsnMethodObj* ppar);
  void delVolRender(class vsnVolumeRender* pvr, class vsnMethodObj* ppar);

  // light attribute interface
  vsnLightAttribute& getLightAttr() {return m_lightAttr;}
  vsnLightAttribute getLightAttr() const {return m_lightAttr;}

  // xforming interface
  bool getXforming() const {return m_xforming;}
  void setXforming(const bool mode) {m_xforming = mode; notice();}

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // override vfrGroup methods
  void render(const Bool tm, vfrMaterialStack* mstk =NULL);

  // static methods
  static void SetXformSimpleRender(const bool mode);
  static bool GetXformSimpleRender();
  static void ReadConfig();
  static void WriteConfig();

private:
  vfrGroup* m_frontObjGrp;
  std::map<class vsnVolumeRender*, class vsnMethodObj*> m_pVolRenLst;
  vsnLightAttribute m_lightAttr;

  bool importDataXML(xmlNodePtr cur);
  mutable class vsnDataObj* p_lastLoadData;

  mutable volatile bool m_xforming;

  // utils
  std::multimap<float, vsnVolumeRender*>
    getSortedVolRenLst(const CES::Mat4<float>& MVM);

  // static members
  static bool s_xformSimpleRender;
};

#endif // _VSN_SCENE_H_
