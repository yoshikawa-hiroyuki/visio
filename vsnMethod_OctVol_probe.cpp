//
// vsnMethod_OctVol_probe
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

#include "vsnMethod_OctVol_probe.h"
#include "vsnData_OctVol.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_OctVol_probe
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_probe::vsnMethod_OctVol_probe(const string& name)
  : vsnMethod_probe(name)
{
}

vsnMethod_OctVol_probe::~vsnMethod_OctVol_probe() {
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_OctVol_probe::updateStep(const int stp,
					const bool force,
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

  // glyph
  if ( ! m_glyph ) return false;

  // initialize glyph / repStr
  m_glyph->setMatrix(m_matXForm);
  m_repStr = "out of data\n";
  if ( ! m_show )
    m_glyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  else
    m_glyph->getPrivateMaterial()->setRenderMode(RT_SMOOTH);

  // updated step
  m_updatedStp = m_requestedStp;


  //--------------- data probing ---------------
  char txt[64];
  Vec3<float> pos = m_matXForm * Vec3<float>(0.f, 0.f, 0.f);
  m_repStr = "Coord:\n";
  m_repStr += "  x = "; sprintf(txt, "%g\n", pos[0]); m_repStr += txt;
  m_repStr += "  y = "; sprintf(txt, "%g\n", pos[1]); m_repStr += txt;
  m_repStr += "  z = "; sprintf(txt, "%g\n", pos[2]); m_repStr += txt;

  // root cell index
  m_repStr += "Root index:\n";
  vsnOctTree::Node* pnode = pData->getNode(pos);
  if ( ! pnode ) {
    m_repStr += "  IJK = [out of range]";
    updateUI();
    return true;
  }
  register size_t i = 0;
  vsnOctTree::Node* pn = pnode;
  while ( pn->p_parent ) {
    pn = pn->p_parent;
    i ++;
  } // end of while(pn->p_parent);
  vsnOctTree::RootNode* prn = dynamic_cast<vsnOctTree::RootNode*>(pn);
  if ( prn ) {
    sprintf(txt, "  IJK = [%lu, %lu, %lu]\n",
	    prn->m_idx[0], prn->m_idx[1], prn->m_idx[2]);
    m_repStr += txt;
  } else {
    m_repStr += "  IJK = [out of data, no root]\n";
  }

  m_repStr += "OctTree:\n";
  sprintf(txt, "  level = %lu\n", i); m_repStr += txt;
  if ( i > 0 )
    sprintf(txt, "  pedigree = [Root]%llu[Leaf]\n", pnode->m_pedigree);
  else
    sprintf(txt, "  pedigree = [Root]\n");
  m_repStr += txt;
  
  // datas
  register size_t dlen = pData->getDataLen();
  if ( dlen > 0 && pnode->m_pData ) {
    m_repStr += "Datas:\n";
    for ( i = 0; i < dlen; i++ ) {
      sprintf(txt, "  data%lu = %g\n", i, pnode->m_pData[i]);
      m_repStr += txt;
    } // end of for(i)
  }

  updateUI();
  return true;
}
