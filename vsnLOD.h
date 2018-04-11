//
// vsnLOD
//
#ifndef _VSN_LOD_H_
#define _VSN_LOD_H_

#include "vfrGroup.h"
#include <set>
#include <functional>


class vsnLOD : public vfrGroup {
public:
  typedef std::set<float, std::greater<float> > AreaListType;

  vsnLOD(const std::string& =std::string(VFR_NONAME), const Bool =FALSE);
  virtual ~vsnLOD();

  size_t getNumLevels() const {return m_areaLst.size();}
  const AreaListType& getAreaList() const {return m_areaLst;}
  bool setAreaList(const AreaListType& al);
  bool addArea(const float a);

  // override vfrGroup
  virtual void render(const Bool transpMode, vfrMaterialStack *mstk);
  virtual void renderSolid();
  virtual void renderWire();
  virtual void renderPoint();
  
protected:
  int getCurrentLevel() const;
  void callPrimRender(const RenderType rt, vfrNode* pn);

  AreaListType m_areaLst;
  static float s_areaEps;
};

#endif // _VSN_LOD_H_
