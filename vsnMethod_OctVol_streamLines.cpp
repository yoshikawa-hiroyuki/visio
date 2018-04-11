//
// vsnMethod_OctVol_streamLines
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

#include "vsnMethod_OctVol_streamLines.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_OctVol_streamLines
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_streamLines::vsnMethod_OctVol_streamLines(const string& name)
  : vsnMethod_streamLines(name), m_pArry(NULL)
{
}

vsnMethod_OctVol_streamLines::~vsnMethod_OctVol_streamLines() {
  if ( m_pArry ) delete [] m_pArry;
}


/* from vsnTimeSeriesMethodIF */

bool vsnMethod_OctVol_streamLines::updateStep(const int stp, const bool force,
                                              const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  // check needs
  //  don't calculate when time-step changed if !force ...
  if ( m_always ) m_needUpd = true;
  if ( ! m_needUpd && 
       (m_updatedStp == m_requestedStp || ! force) ) return true;
  m_updatedStp = -1;

  // check vecData
  if ( ! isValidVecData() ) return false;

  // lines
  if ( ! m_lines ) {
    m_lines = new vsnPtSet();
    if ( ! m_lines ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_lines->alcMaterial();
    addChild(m_lines);
  }
  m_lines->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  register int i, l;

  // check sampler
  if ( ! p_splr ) return true;
  Point2 sampleSize = p_splr->getSampleNumber();
  size_t sampleSz = sampleSize.x * sampleSize.y;
  if ( sampleSz < 1 ) return true;
  const vector3* const samplePts = p_splr->getSamplePoints();
  if ( ! samplePts ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get sample points data"));
    return false;
  }

  // get bbox from OctVol data
  const Vec3<float>* pbb = pData->getBbox();

  // alloc pArry
  if ( m_pArry ) delete [] m_pArry;
  m_pArry = new deque< Vec3<float> >[sampleSz];
  if ( ! m_pArry ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }

  // calculate streamlines
  register size_t num_pts = 0;
  
  for ( l = 0; l < sampleSz; l++ ) {
    // pre-check using bbox
    if ( samplePts[l][0] < pbb[0].m_v[0] || samplePts[l][0] > pbb[1].m_v[0] ||
         samplePts[l][1] < pbb[0].m_v[1] || samplePts[l][1] > pbb[1].m_v[1] ||
         samplePts[l][2] < pbb[0].m_v[2] || samplePts[l][2] > pbb[1].m_v[2] )
      continue;

    m_pArry[l].clear();
    calc_streamLine(samplePts[l], m_pArry[l]);
    num_pts += m_pArry[l].size();
  } // end of for(l)

  // alloc lines data
  if ( ! m_lines->alcVerts(num_pts) ||
       ! m_lines->alcIndices(sampleSz) ) {
     ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }
  if ( m_colored ) {
    if ( ! m_lines->alcColors(sampleSz) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_lines->setColorMode(AT_PER_FACE);
  } else {
    m_lines->setColorMode(AT_WHOLE);
    m_lines->setColor4(0, m_colour);
  }

  // set lines
  vector3* vl = m_lines->getVerts();
  int* il = m_lines->getIndices();

  register int idx = 0;
  for ( l = 0; l < sampleSz; l++ ) {
    // set vertex
    for ( i = 0; i < m_pArry[l].size(); i++ ) {
      vl[idx][0] = m_pArry[l][i].m_v[0];
      vl[idx][1] = m_pArry[l][i].m_v[1];
      vl[idx][2] = m_pArry[l][i].m_v[2];
      idx++;
    } // end of for(i)

    // indices
    il[l] = m_pArry[l].size();
  } // end of for(l)

  // colors
  if ( m_colored ) {
    vfrDatamap lrgb((float)(sampleSize.x -1), 0.f);
    if ( sampleSize.x == 1 ) lrgb.setHue(0, 0.f);
    vector4* cl = m_lines->getColors();
    vector4 wkc;
    for ( l = 0; l < sampleSz; l++ ) {
      lrgb.datamap((float)(l % sampleSize.x), wkc);
      cl[l][0] = wkc[0];
      cl[l][1] = wkc[1];
      cl[l][2] = wkc[2];
      cl[l][3] = 1.f;
    } // end of for(l)
  } // end of if(m_colored)

  m_lines->getPrivateMaterial()->setLineWidth(m_lineWidth);
  m_lines->setTransparency(m_antiAlias);
  m_lines->generateBbox();

  // ok
  m_lines->getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;
  m_needUpd = false;

  if ( m_autoExport && ! m_exportPath.empty() ) {
    if ( ! exportLines(m_exportPath) )
      setAutoExport(false);
  }

  return true;
}


/* internal methods to make stream lines */

bool vsnMethod_OctVol_streamLines::
calc_streamLine(const Vec3<float>& x0, deque< Vec3<float> >& parry) {
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  if ( ! isValidVecData() ) return false;

  // get the leaf-node includes x0
  vsnOctTree::Node* pnode2 = pData->getNode(x0);
  if ( ! pnode2 ) return false; // out of region
  vsnOctTree::Node* pnode(NULL);

  // start point
  parry.push_back(x0);

  // main loop
  const float dtFac = 1.f / ((m_divTime == 0) ? 1 : m_divTime);
  register int l, lvl, it, end = (m_maxPts - 1) * m_numSkip;
  register float dt, scaleFac, vLen;
  Vec3<float> x(x0), xn, v;
  Vec3<float> size, pitch = pData->getPitch();

  for ( it = 1; it < end; it++ ) {
    if ( pnode != pnode2 ) {
      pnode = pnode2;

      v.m_v[0] = pnode->m_pData[m_vecDataIdx[0]];
      v.m_v[1] = pnode->m_pData[m_vecDataIdx[1]];
      v.m_v[2] = pnode->m_pData[m_vecDataIdx[2]];

      // check stagnation point
      vLen = v.Length();
      if ( vLen < 1e-8f ) break;

      // get size of the leaf-node
      lvl = pnode->GetLevel();
      scaleFac = 1.f;
      for ( l = 0; l < lvl; l++ ) scaleFac *= 0.5f;
      size = pitch * scaleFac;

      // set dt
      dt = size.m_v[0];
      if ( dt < size.m_v[1] ) dt = size.m_v[1];
      if ( dt < size.m_v[2] ) dt = size.m_v[2];
      if ( dt < 1e-8 ) break;
      dt *= (0.5f / vLen);
      dt *= dtFac;
    } // end of if(pnode!=pnode2)

    // get next pos
    if ( ! calc_nextPosRKG(x, pnode, dt, xn) )
      break;

    // add vertex
    if ( ! ((it+1) % m_numSkip) )
      parry.push_back(xn);

    // for next loop
    x = xn;
    pnode2 = pData->getNode(x);
    if ( ! pnode2 ) break;
  } // end of for(it)

  return true;
}


#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880 /* sqrt(2) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440 /* 1/sqrt(2) */
#endif

bool vsnMethod_OctVol_streamLines::
calc_nextPosRKG(const Vec3<float>& x0, vsnOctTree::Node* pnode,
		const float dt, Vec3<float>& xn) {
  if ( ! pnode ) return false;
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;

  Vec3<float> P1, P2, P3, Q1, Q2, Q3;
  Vec3<float> U0, U1, U2, U3;
  Vec3<float> wkv;

  // stage 1
  if ( ! pData->interpolateData(x0, pnode, m_vecDataIdx, U0) )
    return false;
  Q1 = U0 * dt;
  P1 = x0 + (Q1 * 0.5f);

  // stage 2
  if ( ! pData->interpolateData(P1, pnode, m_vecDataIdx, U1) )
    return false;
  wkv = (U1 * dt) - Q1;
  P2 = P1 + (wkv * (1.f - M_SQRT1_2));
  Q2 = (Q1 * (3.f * M_SQRT1_2 - 2.f)) + (U1 * ((2.f - M_SQRT2)*dt));

  // stage 3
  if ( ! pData->interpolateData(P2, pnode, m_vecDataIdx, U2) )
    return false;
  wkv = (U2 * dt) - Q2;
  P3 = P2 + (wkv * (1.f + M_SQRT1_2));
  Q3 = (Q2 * (-2.f - 3.f * M_SQRT1_2)) + (U2 * ((2.f + M_SQRT2)*dt));

  // the last stage
  if ( ! pData->interpolateData(P3, pnode, m_vecDataIdx, U3) )
    return false;
  wkv = P3 + (U3 * (dt / 6.f));
  xn = wkv - (Q3 * (1.f / 3.f));
  return true;
}
