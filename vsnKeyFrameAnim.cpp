//
// vsnKeyFrameAnim
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

#include "vsnKeyFrameAnim.h"
#include <math.h>

#if 0
#if defined(WINDOWS)
/* replacement of Unix rint() for Windows */
static int rint (double x) {
  char *buf;
  int i, dec, sig;
  buf = _fcvt(x, 0, &dec, &sig);
  i = atoi(buf);
  if (sig == 1) {i = i * -1;}
  return(i);
}
#endif
#endif

//----------------------------------------------------------------
// class vsnAnimFrame
//----------------------------------------------------------------
bool vsnAnimFrame::setRotMat(const CES::Mat4<float>& m) {
  double EPS = 1.0e-3;
  double q0, q1, q2, q3;

  q0 = ( m[0] + m[5] + m[10] + 1.0) / 4.0;
  q1 = ( m[0] - m[5] - m[10] + 1.0) / 4.0;
  q2 = (-m[0] + m[5] - m[10] + 1.0) / 4.0;
  q3 = (-m[0] - m[5] + m[10] + 1.0) / 4.0;
  if ( q0 < 0.0 ) q0 = 0.0;
  if ( q1 < 0.0 ) q1 = 0.0;
  if ( q2 < 0.0 ) q2 = 0.0;
  if ( q3 < 0.0 ) q3 = 0.0;
  q0 = sqrt(q0);
  q1 = sqrt(q1);
  q2 = sqrt(q2);
  q3 = sqrt(q3);
  if ( q0 < EPS && q1 < EPS ) {
    q0 = 0.0;
    q1 = 0.0;
    q2 *= +1.0;
    if ( m[6] + m[9] < 0.0 )
      q3 *= -1.0;
  } else if ( q0 < EPS ) {
    q0 = 0.0;
    q1 *= +1.0;
    if ( m[1] + m[4] < 0.0 )
      q2 *= -1.0;
    if ( m[8] + m[2] < 0.0 )
      q3 *= -1.0;
  } else {
    q0 *= +1.0;
    if ( m[6] - m[9] < 0.0 )
      q1 *= -1.0;
    if ( m[8] - m[2] < 0.0 )
      q2 *= -1.0;
    if ( m[1] - m[4] < 0.0 )
      q3 *= -1.0;
  }
  double r = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  rotQuat.m_w = q0 / r;
  rotQuat.m_v[0] = q1 / r;
  rotQuat.m_v[1] = q2 / r;
  rotQuat.m_v[2] = q3 / r;

  return true;
}

bool vsnAnimFrame::parseXML(xmlNodePtr node, double& tm) {
  static xmlChar* xs;

  if ( ! node ) return false;
  if ( strcmp((const char*)node->name, "frame") ) return false;

  // initialize
  rotQuat = CES::Quat4<float>();
  transVec = CES::Vec3<float>();
  scaleVec = CES::Vec3<float>(1.f, 1.f, 1.f);
  centerVec = CES::Vec3<float>();
  step = 0;
  tm = 0.0;

  // get time prop
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(node, (const xmlChar*)"time");
  if ( xs && strlen((const char*)xs) > 0 ) tm = atof((const char*)xs);

  // get step prop
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(node, (const xmlChar*)"step");
  if ( xs && strlen((const char*)xs) > 0 ) step = atoi((const char*)xs);

  // search 'transform' node
  xmlNodePtr cur = node->xmlChildrenNode;
  while ( cur ) {
    if ( !strcmp((const char*)cur->name, "transform") ) break;
    cur = cur->next;
  }
  if ( ! cur ) return true;

  // parse the 'transform' node
  cur = cur->xmlChildrenNode;
  while ( cur ) {
    CES::Mat4<float> M; CES::Vec3<float> V;
    if ( !strcmp((const char*)cur->name, "rotate") ) {
      if ( ParseRotMat(cur, M) ) setRotMat(M);
    }
    else if ( !strcmp((const char*)cur->name, "posture") ) {
      if ( ParsePosture(cur, M) ) setRotMat(M);
    }
    else if ( !strcmp((const char*)cur->name, "translate") ) {
      if ( ParseVector(cur, V) ) transVec = V;
    }
    else if ( !strcmp((const char*)cur->name, "scale") ) {
      if ( ParseVector(cur, V) ) scaleVec = V;
    }
    else if ( !strcmp((const char*)cur->name, "center") ) {
      if ( ParseVector(cur, V) ) centerVec = V;
    }
    cur = cur->next;
  }

  return true;
}

bool vsnAnimFrame::outputXML(std::ostream& os, const double tm, const size_t ts)
{
  using namespace std;
  using namespace CES;

  string idts;
  register int i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');

  os << idts << "<frame time=\"" << tm << "\" step=\"" << step << "\">"
     << "<transform>" << endl;

  if ( transVec.Length() > 1e-6 ) {
    os << idts_2 << "<translate>" << transVec[0] << " " << transVec[1] << " "
       << transVec[2] << "</translate>" << endl;
  }

  if ( (scaleVec - Vec3<float>(1.f,1.f,1.f)).Length() > 1e-6 ) {
    os << idts_2 << "<scale>" << scaleVec[0] << " " << scaleVec[1] << " "
       << scaleVec[2] << "</scale>" << endl;
  }

  if ( centerVec.Length() > 1e-6 ) {
    os << idts_2 << "<center>" << centerVec[0] << " " << centerVec[1] << " "
       << centerVec[2] << "</center>" << endl;
  }

  Mat4<float> rm = rotQuat.GetRotMat();
  float rmLen =
    (Vec3<float>(rm[0],rm[4],rm[8]) - Vec3<float>(1.f,0.f,0.f)).Length() +
    (Vec3<float>(rm[1],rm[5],rm[9]) - Vec3<float>(0.f,1.f,0.f)).Length() +
    (Vec3<float>(rm[2],rm[6],rm[10])- Vec3<float>(0.f,0.f,1.f)).Length();
  if ( rmLen > 1e-6 ) {
    os << idts_2 << "<rotate>" << endl;
    os << idts_2 << rm[0] << " " << rm[4] << " " << rm[8] << endl;
    os << idts_2 << rm[1] << " " << rm[5] << " " << rm[9] << endl;
    os << idts_2 << rm[2] << " " << rm[6] << " " << rm[10] << endl;
    os << idts_2 << "</rotate>" << endl;
  }

  os << idts << "</transform>" << "</frame>" << endl;
  return true;
}


bool vsnAnimFrame::ParseRotMat(xmlNodePtr node, CES::Mat4<float>& M)
{
  if ( ! node ) return false;
  if ( strcmp((const char*)node->name, "rotate") ) return false;
  CES::Mat4<float> m;
  xmlNodePtr cur = node->xmlChildrenNode;
  if ( cur->type != XML_TEXT_NODE ) return false;
  if ( sscanf((const char*)cur->content, "%f %f %f %f %f %f %f %f %f",
              &m.m_v[0], &m.m_v[4], &m.m_v[8],
              &m.m_v[1], &m.m_v[5], &m.m_v[9],
              &m.m_v[2], &m.m_v[6], &m.m_v[10]) != 9 ) return false;
  M = m; return true;
}

bool vsnAnimFrame::ParsePosture(xmlNodePtr node, CES::Mat4<float>& M)
{
  if ( ! node ) return false;
  if ( strcmp((const char*)node->name, "posture") ) return false;
  CES::Vec3<float> axis(0,0,1); float ang = 0.f;
  xmlNodePtr cur = node->xmlChildrenNode;
  while ( cur ) {
    if ( !strcmp((const char*)cur->name, "axis") ) {
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) axis[0] = (float)atof((const char*)xs);
      if ( ys ) axis[1] = (float)atof((const char*)ys);
      if ( zs ) axis[2] = (float)atof((const char*)zs);
    }
    else if ( !strcmp((const char*)cur->name, "angle") ) {
      xmlChar* vs = xmlGetProp(cur, (const xmlChar*)"value");
      xmlChar* ts = xmlGetProp(cur, (const xmlChar*)"type");
      if ( vs ) {
        ang = (float)atof((const char*)vs);
        if ( ts && !strcmp((const char*)ts, "DEG") )
          ang = CES::Deg2Rad(ang);
      }
    }
    cur = cur->next;
  }
  M.Identity(); M.Rotation(ang, axis);
  return true;
}

bool vsnAnimFrame::ParseVector(xmlNodePtr node, CES::Vec3<float>& V)
{
  if ( ! node ) return false;
  CES::Vec3<float> v;
  xmlNodePtr cur = node->xmlChildrenNode;
  if ( cur->type != XML_TEXT_NODE ) return false;
  if ( sscanf((const char*)cur->content, "%f %f %f",
              &v.m_v[0], &v.m_v[1], &v.m_v[2]) != 3 ) return false;
  V = v; return true;
}


//----------------------------------------------------------------
// class vsnKeyFrameAnim
//----------------------------------------------------------------

size_t vsnKeyFrameAnim::getTotalFrames() const
{
  if ( getNumKeyframes() < 1 ) return 0;
  return (size_t)(m_totalTime * m_fps +1);
}

bool vsnKeyFrameAnim::getKeyframe(const size_t idx,
                                   double& tm, vsnAnimFrame& frame) const
{
  if ( idx >= getNumKeyframes() ) return false;
  register size_t i = 0;
  std::map<double, vsnAnimFrame>::const_iterator it;
  for ( it = m_keyFrames.begin();
        i != idx && it != m_keyFrames.end(); it++, i++ );
  tm = it->first; frame = it->second;
  return true;
}

bool vsnKeyFrameAnim::addKeyframe(const double tm, const vsnAnimFrame& frame)
{
  std::map<double, vsnAnimFrame>::iterator it = m_keyFrames.find(tm);
  if ( it != m_keyFrames.end() ) return false;

  m_keyFrames.insert(vsnKeyframe(tm, frame));

  // adjust time params;
  if ( m_keyFrames.size() > 1 ) {
    double tm1, tm2;
    it = m_keyFrames.begin();
    tm1 = it->first;
    it = m_keyFrames.end(); it--;
    tm2 = it->first;
    m_totalTime = tm2 - tm1;
  }
  else {
    m_totalTime = 0.;
  }
  m_initialTime = m_keyFrames.begin()->first;

  return true;
}

bool vsnKeyFrameAnim::delKeyframe(const size_t idx)
{
  if ( idx >= getNumKeyframes() ) return false;
  register size_t i = 0;
  std::map<double, vsnAnimFrame>::iterator it;
  for ( it = m_keyFrames.begin();
        it != m_keyFrames.end() && i < idx; it++, i++ );
  if ( it == m_keyFrames.end() ) return false;

  m_keyFrames.erase(it);

  // adjust time params;
  if ( m_keyFrames.size() > 1 ) {
    double tm1, tm2;
    it = m_keyFrames.begin();
    tm1 = it->first;
    it = m_keyFrames.end(); it--;
    tm2 = it->first;
    m_totalTime = tm2 - tm1;
  }
  else {
    m_totalTime = 0.;
  }
  if ( m_keyFrames.size() > 0 )
    m_initialTime = m_keyFrames.begin()->first;
  else
    m_initialTime = 0.;

  return true;
}

bool vsnKeyFrameAnim::delKeyframe(const double tm)
{
  std::map<double, vsnAnimFrame>::iterator it = m_keyFrames.find(tm);
  if ( it == m_keyFrames.end() ) return false;

  m_keyFrames.erase(it);

  // adjust time params;
  if ( m_keyFrames.size() > 1 ) {
    double tm1, tm2;
    it = m_keyFrames.begin();
    tm1 = it->first;
    it = m_keyFrames.end(); it--;
    tm2 = it->first;
    m_totalTime = tm2 - tm1;
  }
  else {
    m_totalTime = 0.;
  }
  if ( m_keyFrames.size() > 0 )
    m_initialTime = m_keyFrames.begin()->first;
  else
    m_initialTime = 0.;

  return true;
}

bool
vsnKeyFrameAnim::replaceKeyframe(const size_t idx, const vsnAnimFrame& frame)
{
  if ( idx >= getNumKeyframes() ) return false;
  register size_t i = 0;
  std::map<double, vsnAnimFrame>::iterator it;
  for ( it = m_keyFrames.begin();
        it != m_keyFrames.end() && i < idx; it++, i++ );
  if ( it == m_keyFrames.end() ) return false;

  double tm = it->first;
  m_keyFrames.erase(it);
  m_keyFrames.insert(vsnKeyframe(tm, frame));

  // adjust time params;
  if ( m_keyFrames.size() > 1 ) {
    double tm1, tm2;
    it = m_keyFrames.begin();
    tm1 = it->first;
    it = m_keyFrames.end(); it--;
    tm2 = it->first;
    m_totalTime = tm2 - tm1;
  }
  else {
    m_totalTime = 0.;
  }
  if ( m_keyFrames.size() > 0 )
    m_initialTime = m_keyFrames.begin()->first;
  else
    m_initialTime = 0.;
  return true;
}

bool vsnKeyFrameAnim::getFrame(const double tm, vsnAnimFrame& frame) const
{
  if ( m_keyFrames.size() < 1 ) return false;

  double tm1, tm2;
  std::map<double, vsnAnimFrame>::const_iterator it = m_keyFrames.begin();
  std::map<double, vsnAnimFrame>::const_iterator it0;
  if ( m_keyFrames.size() == 1 ) {
    frame = it->second;
    return true;
  }
  tm1 = it->first;
  tm2 = tm1 + m_totalTime;

  if ( tm < tm1 ) {
    frame = it->second;
    return true;
  } else if ( tm >= tm2 ) {
    it0 = m_keyFrames.end(); it0--;
    frame = it0->second;
    return true;
  }

  for ( ; it != m_keyFrames.end(); it++ ) {
    if ( it->first > tm ) break;
    it0 = it;
  }
  if ( it == m_keyFrames.end() ) return false;

  double tt = (it->first - it0->first);
  if ( fabs(tt) < 1e-8 ) {
    frame = it0->second;
    return true;
  }
  float dt = (float)((tm - it0->first) / tt);
  const vsnAnimFrame& F0 = it0->second;
  const vsnAnimFrame& F1 = it->second;
  CES::Vec3<float> wv;
  frame.rotQuat = CES::QuatSlerp(F0.rotQuat, F1.rotQuat, dt);
  wv = F1.transVec - F0.transVec;
  frame.transVec = F0.transVec + (wv * dt);
  wv = F1.scaleVec - F0.scaleVec;
  frame.scaleVec = F0.scaleVec + (wv * dt);
  wv = F1.centerVec - F0.centerVec;
  frame.centerVec = F0.centerVec + (wv * dt);
  if ( m_stepMode ) {
    int ds = F1.step - F0.step;
    frame.step = F0.step + (int)rint(ds * dt);
  } else
    frame.step = 0;

  return true;
}

bool vsnKeyFrameAnim::parseXML(xmlNodePtr node)
{
  if ( ! node ) return false;
  if ( strcmp((const char*)node->name, "keyframes") ) return false;

  // initialize
  Reset();

  // get fps prop
  xmlChar* fpsStr = xmlGetProp(node, (const xmlChar*)"fps");
  if ( fpsStr ) {
    int xfps = atoi((const char*)fpsStr);
    if ( xfps > 0 ) m_fps = xfps;
  }
  // get use_step prop
  xmlChar* stpStr = xmlGetProp(node, (const xmlChar*)"use_step");
  if ( stpStr ) {
    if ( ! strncmp((const char*)stpStr, "YES", 3) ||
         ! strncmp((const char*)stpStr, "yes", 3) )
      setStepMode(true);
  }
  // get loop prop
  xmlChar* loopStr = xmlGetProp(node, (const xmlChar*)"loop");
  if ( loopStr ) {
    if ( ! strncmp((const char*)loopStr, "YES", 3) ||
         ! strncmp((const char*)loopStr, "yes", 3) )
      setLoopMode(true);
  }

  // children nodes
  xmlNodePtr cur = node->xmlChildrenNode;
  while ( cur ) {
    if ( !strcmp((const char*)cur->name, "frame") ) {
      vsnAnimFrame F; double tm = 0.;
      if ( F.parseXML(cur, tm) ) {
        addKeyframe(tm, F);
      }
    }
    cur = cur->next;
  }

  return true;
}

bool
vsnKeyFrameAnim::outputXML(std::ostream& os, const size_t ts)
{
  using namespace std;

  string idts;
  register int i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');

  os << idts << "<keyframes fps=\"" << m_fps << "\" use_step=\""
     << (m_stepMode ? "yes" : "no") << "\" loop=\""
     << (m_loopMode ? "yes" : "no") << "\">" << endl;

  std::map<double, vsnAnimFrame>::iterator it;
  for ( it = m_keyFrames.begin(); it != m_keyFrames.end(); it++ ) {
    vsnAnimFrame& pF = it->second;
    if ( ! pF.outputXML(os, it->first, ts+2) ) {
      break;
    }
  } // end of for(it)

  os << idts << "</keyframes>" << endl;
  return true;
}
