//
// vsnExtrude
//
#ifndef _VSN_EXTRUDE_H_
#define _VSN_EXTRUDE_H_

#include "vfrLines.h"
#include "vfrLineStrip.h"
#include "vsnGridUtilSv.h" // for DVec3


//----------------------------------------------------------------
// class vsnTubeLines
//----------------------------------------------------------------
class vsnTubeLines : public vfrLines {
public:
  vsnTubeLines(const std::string& =std::string(VFR_NONAME), const Bool =FALSE);
  virtual ~vsnTubeLines();

  double getRadius() const {return m_radius;}
  bool setRadius(const double r);

  // from vfrNode
  RenderType getRenderMode() const;
  void renderSolid();

protected:
  double m_radius;
};

//----------------------------------------------------------------
// class vsnTubeLineStrip
//----------------------------------------------------------------
class vsnTubeLineStrip : public vfrLineStrip {
public:
  vsnTubeLineStrip(const std::string& =std::string(VFR_NONAME),
		   const Bool =FALSE);
  virtual ~vsnTubeLineStrip();

  double getRadius() const {return m_radius;}
  bool setRadius(const double r);

  // from vfrNode
  RenderType getRenderMode() const;
  void renderSolid();

protected:
  double m_radius;
  VSN::DVec3 *m_ptbuff;
};

#endif // _VSN_EXTRUDE_H_
