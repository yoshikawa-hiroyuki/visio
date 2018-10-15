//
// vsnExtrude
//
#ifndef _VSN_EXTRUDE_H_
#define _VSN_EXTRUDE_H_

#include "vfrLines.h"
#include "vfrLineStrip.h"


//----------------------------------------------------------------
// class vsnTubeLines
//----------------------------------------------------------------
class vsnTubeLines : public vfrLines {
public:
  vsnTubeLines(const std::string& =std::string(VFR_NONAME), const Bool =FALSE);
  virtual ~vsnTubeLines();

  int getNumRadius() const {return m_numRadius;}
  double* getRadiusList() {return m_radiusList;}
  bool setRadiusList(const int n, const double* pr);

  // from vfrNode
  RenderType getRenderMode() const;
  void renderSolid();

protected:
  int     m_numRadius;
  double* m_radiusList;
};

//----------------------------------------------------------------
// class vsnTubeLineStrip
//----------------------------------------------------------------
class vsnTubeLineStrip : public vfrLineStrip {
public:
  vsnTubeLineStrip(const std::string& =std::string(VFR_NONAME),
		   const Bool =FALSE);
  virtual ~vsnTubeLineStrip();

  int getNumRadius() const {return m_numRadius;}
  std::deque<double>& getRadiusList() {return m_radiusList;}
  bool alcRadiusList(const int n);

  // from vfrNode
  RenderType getRenderMode() const;
  void renderSolid();

protected:
  int     m_numRadius;
  std::deque<double> m_radiusList;
};

#endif // _VSN_EXTRUDE_H_
