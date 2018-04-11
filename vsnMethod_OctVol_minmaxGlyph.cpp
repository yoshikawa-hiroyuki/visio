//
// vsnMethod_OctVol_minmaxGlyph
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

#include "vsnMethod_OctVol_minmaxGlyph.h"
#include "vsnError.h"

#define LIMIT_GLYPH_NUM 1000

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_OctVol_minmaxGlyph
//----------------------------------------------------------------

/* methods */

bool vsnMethod_OctVol_minmaxGlyph::getMinMaxCurStp(float minmax[2]) {
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  
  if ( m_selectedData == DATA_None ) return false;
  else if ( m_selectedData == DATA_Veclen ) {
    Vec3<int> vidx = m_vecDataIdx;
    const vsnOctTree* otv = pData->getOctTree();
    if ( ! otv || otv->m_pLeafLst.empty() ) return false;
    if ( vidx[0] < 0 || vidx[0] >= otv->m_dataLen ) return false;
    if ( vidx[1] < 0 || vidx[1] >= otv->m_dataLen ) return false;
    if ( vidx[2] < 0 || vidx[2] >= otv->m_dataLen ) return false;

    register float val;
    set<vsnOctTree::Node*>::const_iterator it = otv->m_pLeafLst.begin();
    if ( ! *it || ! (*it)->m_pData ) return false;
    Vec3<float> vv((*it)->m_pData[vidx[0]],
		   (*it)->m_pData[vidx[1]], (*it)->m_pData[vidx[2]]);
    minmax[0] = minmax[1] = vv.Length();
    for ( ; it != otv->m_pLeafLst.end(); it++ ) {
      if ( ! *it || ! (*it)->m_pData ) continue;
      vv = Vec3<float>((*it)->m_pData[vidx[0]],
		       (*it)->m_pData[vidx[1]], (*it)->m_pData[vidx[2]]);
      val = vv.Length();
      if ( minmax[0] > val ) minmax[0] = val;
      if ( minmax[1] < val ) minmax[1] = val;
    } // end of for(it)
  }
  else if ( m_selectedData > 0 && m_selectedData <= pData->getDataLen() ) {
    return pData->getMinMax(m_selectedData -1, minmax);
  }
  else return false;

  return true;
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_OctVol_minmaxGlyph::updateStep(const int stp, const bool force,
					      const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // check glyphs
  if ( ! m_minGlyph || ! m_maxGlyph ) return false;
  m_minGlyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  m_maxGlyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) return true;

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {0.f, 1.f};
  if ( ! getMinMaxCurStp(dr) ) return true;
  float delta = (float)(fabs(dr[1] - dr[0]) * m_tolerance / 100.0);
  dr[0] += delta;
  dr[1] -= delta;

  // get Node list, params
  const set<vsnOctTree::Node*>& leafLst = pData->getLeafNodeList();
  Vec3<float> rootPitch = pData->getPitch();
  const Vec3<float>* dbb = pData->getBbox();

  // alloc verts / normals
  register float val;
  deque<vsnOctTree::Node*> minLeafLst, maxLeafLst;
  set<vsnOctTree::Node*>::const_iterator its;
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> vv;
    for ( its = leafLst.begin(); its != leafLst.end(); its++ ) {
      vsnOctTree::Node* pn = (*its);
      if ( ! pn || ! pn->m_pData ) continue;
      vv.m_v[0] = pn->m_pData[m_vecDataIdx.m_v[0]];
      vv.m_v[1] = pn->m_pData[m_vecDataIdx.m_v[1]];
      vv.m_v[2] = pn->m_pData[m_vecDataIdx.m_v[2]];
      val = vv.Length();
      if ( val < dr[0] ) minLeafLst.push_back(pn);
      if ( val > dr[1] ) maxLeafLst.push_back(pn);
    } // end of for(its)
  }
  else {
    for ( its = leafLst.begin(); its != leafLst.end(); its++ ) {
      vsnOctTree::Node* pn = (*its);
      if ( ! pn || ! pn->m_pData ) continue;
      val = pn->m_pData[m_selectedData -1];
      if ( val < dr[0] ) minLeafLst.push_back(pn);
      if ( val > dr[1] ) maxLeafLst.push_back(pn);
    } // end of for(its)
  }
  size_t minSz = minLeafLst.size();
  size_t maxSz = maxLeafLst.size();
  if ( minSz < 1 && maxSz < 1 ) return true;
  if ( ! m_minGlyph->alcVerts(minSz) ||
       ! m_maxGlyph->alcVerts(maxSz) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: out of memory for vertices"));
    return false;
  }

  // set min verts
  register size_t l, i = 0;
  deque<vsnOctTree::Node*>::iterator it;
  for ( it = minLeafLst.begin(); it != minLeafLst.end(); it++ ) {
    if ( ! (*it) ) continue;
    register size_t lvl = (*it)->GetLevel();
    register float scaleFac = 1.f;
    for ( l = 0; l < lvl; l++ ) scaleFac *= 0.5f;
    vsnOctTree::Node* pnode = *it;

    Vec3<float> orig, size = rootPitch;
    PedigType pedig = pnode->m_pedigree;
    while ( pedig && pnode ) {
      register int relp = pedig % 10 -1;
      if ( relp & (0x1)    ) orig.m_v[0] += rootPitch.m_v[0] * scaleFac;
      if ( relp & (0x1<<1) ) orig.m_v[1] += rootPitch.m_v[1] * scaleFac;
      if ( relp & (0x1<<2) ) orig.m_v[2] += rootPitch.m_v[2] * scaleFac;
      size = size * 0.5f;
      scaleFac *= 2.f;

      pedig = pedig / 10;
      pnode = pnode->p_parent;
    } // end of while(pedig && pnode)

    vsnOctTree::RootNode* proot = dynamic_cast<vsnOctTree::RootNode*>(pnode);
    if ( ! proot ) continue;

    orig.m_v[0] += rootPitch.m_v[0] * (float)proot->m_idx[0];
    orig.m_v[1] += rootPitch.m_v[1] * (float)proot->m_idx[1];
    orig.m_v[2] += rootPitch.m_v[2] * (float)proot->m_idx[2];
    orig = orig + dbb[0];
    orig = orig + (size * 0.5f); // move to center

    m_minGlyph->setVert(i, orig.m_v, FALSE);
    i ++;
  } // end of for(it)
  if ( m_showMin && i > LIMIT_GLYPH_NUM ) {
    char msg[128];
    sprintf(msg, "too many min-glyphs (%d+) may be shown, "
	    "set to off 'show min glyph.", LIMIT_GLYPH_NUM);
    ErrMsg(MsgINFO, msg);
    m_showMin = false;
    updateUI();
  }

  // set max verts
  i = 0;
  for ( it = maxLeafLst.begin(); it != maxLeafLst.end(); it++ ) {
    if ( ! (*it) ) continue;
    register size_t lvl = (*it)->GetLevel();
    register float scaleFac = 1.f;
    for ( l = 0; l < lvl; l++ ) scaleFac *= 0.5f;
    vsnOctTree::Node* pnode = *it;

    Vec3<float> orig, size = rootPitch;
    PedigType pedig = pnode->m_pedigree;
    while ( pedig && pnode ) {
      register int relp = pedig % 10 -1;
      if ( relp & (0x1)    ) orig.m_v[0] += rootPitch.m_v[0] * scaleFac;
      if ( relp & (0x1<<1) ) orig.m_v[1] += rootPitch.m_v[1] * scaleFac;
      if ( relp & (0x1<<2) ) orig.m_v[2] += rootPitch.m_v[2] * scaleFac;
      size = size * 0.5f;
      scaleFac *= 2.f;

      pedig = pedig / 10;
      pnode = pnode->p_parent;
    } // end of while(pedig && pnode)

    vsnOctTree::RootNode* proot = dynamic_cast<vsnOctTree::RootNode*>(pnode);
    if ( ! proot ) continue;

    orig.m_v[0] += rootPitch.m_v[0] * (float)proot->m_idx[0];
    orig.m_v[1] += rootPitch.m_v[1] * (float)proot->m_idx[1];
    orig.m_v[2] += rootPitch.m_v[2] * (float)proot->m_idx[2];
    orig = orig + dbb[0];
    orig = orig + (size * 0.5f); // move to center

    m_maxGlyph->setVert(i, orig.m_v, FALSE);
    i ++;
  } // end of for(it)
  if ( m_showMax && i > LIMIT_GLYPH_NUM ) {
    char msg[128];
    sprintf(msg, "too many max-glyphs (%d+) may be shown, "
	    "set to off 'show max glyph.", LIMIT_GLYPH_NUM);
    ErrMsg(MsgINFO, msg);
    m_showMax = false;
    updateUI();
  }

  // set range
  updateLutRange();

  // set show
  updateShowGlyph();

  // ok
  m_updatedStp = m_requestedStp;
  return true;
}
