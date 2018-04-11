//
// vsnMethod_OctVol_leafSlice
//
#ifndef _VSN_METHOD_OCTVOL_LEAFSLICE_H_
#define _VSN_METHOD_OCTVOL_LEAFSLICE_H_

#include "wx/defs.h"
#include "wx/checkbox.h"
#include "wx/radiobox.h"
#include "wx/textctrl.h"
#include "wx/slider.h"

#include "vsnMethodObj.h"
#include "vfrPrimSet.h"
#include "vfrCube.h"

namespace VSN {
  // control ids
  enum {MPP_OctVol_leafSlice_AxisRdo = 4400,
        MPP_OctVol_leafSlice_PlaneSld,
        MPP_OctVol_leafSlice_PlaneTxt,
	MPP_OctVol_leafSlice_LineWidthTxt,
	MPP_OctVol_leafSlice_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_OctVol_leafSlice
//   method parameter-panel for vsnMethod_OctVol_leafSlice
//----------------------------------------------------------------
class vsnMPP_OctVol_leafSlice : public vsnMethodPP {
public: 
  vsnMPP_OctVol_leafSlice(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_OctVol_leafSlice();

  // interface
  virtual bool update();

  // event handler
  void OnAxisRdo(wxCommandEvent& event);
  void OnPlaneSld(wxScrollEvent& event);
  void OnPlaneTxt(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxRadioBox* m_pAxisRdo;
  wxSlider*   m_pPlaneSld;
  wxTextCtrl* m_pPlaneTxt;
  wxTextCtrl* m_pLineWidthTxt;
  wxCheckBox* m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_OctVol_leafSlice
//----------------------------------------------------------------
class vsnMethod_OctVol_leafSlice
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_OctVol_leafSlice(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_leafSlice();

  int getSliceAxis() const {return m_sliceAxis;}
  int getSlicePlane() const{return m_slicePlane;}
  Point2 getSliceSize() const {return m_sliceSize;}
  bool setSliceParam(const int sa, const int sp);

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("leafSlice");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // override vfrGroup method
  virtual void generateBbox();

private:
  int         m_sliceAxis;
  int         m_slicePlane;
  Point2      m_sliceSize;
  float       m_lineWidth;

  vfrPrimSet* m_leafs;
  vfrCube*    m_pcube;
};

#endif // _VSN_METHOD_OCTVOL_LEAFSLICE_H_
