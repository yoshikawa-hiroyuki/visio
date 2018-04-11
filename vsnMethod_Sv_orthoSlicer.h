//
// vsnMethod_Sv_orthoSlicer
//
#ifndef _VSN_METHOD_SV_ORTHOSLICER_H_
#define _VSN_METHOD_SV_ORTHOSLICER_H_

#include "wx/defs.h"
#include "wx/radiobox.h"
#include "wx/slider.h"
#include "wx/textctrl.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"
#include "vsnGfxActions.h"
#include "vfrLineStrip.h"
#include "vfrMesh2D.h"
#include <set>
#include <deque>

class vsnMethod_Sv_orthoSlicer;

namespace VSN {
  // control ids
  enum {MPP_Sv_orthoSlicer_Axis = 5200,
	MPP_Sv_orthoSlicer_PlaneSld,
	MPP_Sv_orthoSlicer_PlaneTxt,
	MPP_Sv_orthoSlicer_LineWidthTxt,
	MPP_Sv_orthoSlicer_ShowGridChk,
	MPP_Sv_orthoSlicer_AntiAliasChk,
	MPP_Sv_orthoSlicer_LeftBtn,
	MPP_Sv_orthoSlicer_RightBtn
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoSlicer
//   method parameter-panel for vsnMethod_Sv_orthoSlicer
//----------------------------------------------------------------
class vsnMPP_Sv_orthoSlicer : public vsnMethodPP {
public:
  vsnMPP_Sv_orthoSlicer(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_orthoSlicer();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnSelAxisRadio(wxCommandEvent& event);
  void OnPlaneSlider(wxScrollEvent& event);
  void OnPlaneTxt(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnShowGridChk(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);
  void OnLeftBtn(wxCommandEvent& event);
  void OnRightBtn(wxCommandEvent& event);
  void OnChar(wxKeyEvent& event);

private:
  wxRadioBox* m_pAxisRadio;
  wxSlider*   m_pPlaneSlider;
  wxTextCtrl* m_pPlaneTxt;
  wxTextCtrl* m_pLineWidthTxt;
  wxCheckBox* m_pShowGridChk;
  wxCheckBox* m_pAntiAliasChk;
  wxButton*   m_pLeftBtn;
  wxButton*   m_pRightBtn;

  //-------- OrthoSlicerKeyAct : key-in action class --------
  class OrthoSlicerKeyAct : public vsnGfxAct_KeyIn {
  public:
    OrthoSlicerKeyAct(vsnGfxView* pgv =NULL,
		      vsnMethod_Sv_orthoSlicer* pm =NULL);
    void setOrthoSlicerMethod(vsnMethod_Sv_orthoSlicer* pm);
    void execute(vfrEvent& e);
  private:
    vsnMethod_Sv_orthoSlicer* p_method;
    friend class vsnMPP_Sv_orthoSlicer;
  } gfxAct_KI;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnRef_Sv_orthoSlicer
//   refers to vsnMethod_Sv_orthoSlicer
//----------------------------------------------------------------
class vsnRef_Sv_orthoSlicer {
public:
  vsnRef_Sv_orthoSlicer() : p_osr(NULL) {}
  virtual ~vsnRef_Sv_orthoSlicer();

  std::deque<class vsnMethod_Sv_orthoSlicer*>
    getOrthoSlicerList(class vsnData_Sv* pdo);

  class vsnMethod_Sv_orthoSlicer* getOrthoSlicer() {return p_osr;}

  void setOrthoSlicer(class vsnMethod_Sv_orthoSlicer* osr);
  void setOrthoSlicer(class vsnData_Sv* pdo, const int n);
  bool setOrthoSlicer(class vsnData_Sv* pdo, const string& slicerName);

  void noticeDie();
  virtual void noticeUpdate() =0;

  // XML util
  bool exportXMLNode(std::ostream& os, const size_t ts =0) const;

protected:
  class vsnMethod_Sv_orthoSlicer* p_osr;
};


//----------------------------------------------------------------
// class vsnRefOrthoSlicerKeyAct
//   key-in action class for vsnMPP_Sv_orthoXXX
//----------------------------------------------------------------
class vsnRefOrthoSlicerKeyAct : public vsnGfxAct_KeyIn {
public:
  vsnRefOrthoSlicerKeyAct(vsnGfxView* pgv =NULL,
			  vsnRef_Sv_orthoSlicer* prosr =NULL);
  virtual ~vsnRefOrthoSlicerKeyAct() {}
  void setRefOrthoSlicer(vsnRef_Sv_orthoSlicer* prosr);
  void execute(vfrEvent& e);
private:
  vsnRef_Sv_orthoSlicer* p_rosr;
};


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoSlicer
//----------------------------------------------------------------
class vsnMethod_Sv_orthoSlicer
  : public vsnMethodObj, public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Sv_orthoSlicer(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_orthoSlicer();

  // slice axis / plane
  int getSliceAxis() const {return m_sliceAxis;}
  int getSlicePlane() const{return m_slicePlane;}
  bool setSliceParam(const int sa, const int sp);

  // line width
  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  // show grid
  bool getShowGrid() const {return m_showGrid;}
  bool setShowGrid(const bool sgm);

  // output interface
  Point2 getSliceSize() const {return m_sliceSize;}
  const int* const getSliceIndices() {return getIndices();}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
			  const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("orthoSlicer");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // refMethodLst control
  void appendRefMethod(vsnRef_Sv_orthoSlicer* refm);
  void removeRefMethod(vsnRef_Sv_orthoSlicer* refm);

private:
  int     m_sliceAxis;
  int     m_slicePlane;
  Point2  m_sliceSize;

  float   m_lineWidth;
  bool    m_showGrid;

  vfrLineStrip* m_outline;
  vfrMesh2D*    m_grid;

  std::set<vsnRef_Sv_orthoSlicer*> m_refMethodLst;
  void updateRefMethod();
};

#endif // _VSN_METHOD_SV_ORTHOSLICER_H_
