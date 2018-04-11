//
// vsnMethod_orthoProbe
//
#ifndef _VSN_METHOD_SV_ORTHO_PROBE_H_
#define _VSN_METHOD_SV_ORTHO_PROBE_H_

#include "wx/defs.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"
#include "vsnGfxActions.h"
#include "vfrMesh2D.h"
#include "vfrBall.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_orthoProbe_RepTxt =5500,
        MPP_Sv_orthoProbe_AxisRadio,
        MPP_Sv_orthoProbe_ITxt,
        MPP_Sv_orthoProbe_JTxt,
        MPP_Sv_orthoProbe_KTxt,
	MPP_Sv_orthoProbe_PointSizeTxt,
	MPP_Sv_orthoProbe_GlyphBiasTxt,
	MPP_Sv_orthoProbe_LeftBtn,
	MPP_Sv_orthoProbe_RightBtn,
	MPP_Sv_orthoProbe_DownBtn,
	MPP_Sv_orthoProbe_UpBtn,
	MPP_Sv_orthoProbe_MinusBtn,
	MPP_Sv_orthoProbe_PlusBtn
  };
};

class vsnMethod_Sv_orthoProbe;


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoProbe
//   method parameter-panel for vsnMethod_Sv_orthoProbe
//----------------------------------------------------------------
class vsnMPP_Sv_orthoProbe : public vsnMethodPP {
public: 
  vsnMPP_Sv_orthoProbe(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_orthoProbe();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnAxisRadio(wxCommandEvent& event);
  void OnIdxTxt(wxCommandEvent& event);
  void OnPointSizeTxt(wxCommandEvent& event);
  void OnGlyphBiasTxt(wxCommandEvent& event);
  void OnLeftBtn(wxCommandEvent& event);
  void OnRightBtn(wxCommandEvent& event);
  void OnDownBtn(wxCommandEvent& event);
  void OnUpBtn(wxCommandEvent& event);
  void OnMinusBtn(wxCommandEvent& event);
  void OnPlusBtn(wxCommandEvent& event);
  void OnChar(wxKeyEvent& event);

private:
  wxRadioBox* m_pAxisRadio;
  wxTextCtrl* m_pITxt;
  wxTextCtrl* m_pJTxt;
  wxTextCtrl* m_pKTxt;
  wxTextCtrl* m_pRepTxt;
  wxTextCtrl* m_pPointSizeTxt;
  wxTextCtrl* m_pGlyphBiasTxt;
  wxButton*   m_pLeftBtn;
  wxButton*   m_pRightBtn;
  wxButton*   m_pDownBtn;
  wxButton*   m_pUpBtn;
  wxButton*   m_pMinusBtn;
  wxButton*   m_pPlusBtn;

  //-------- OrthoProbeKeyAct : key-in action class --------
  class OrthoProbeKeyAct : public vsnGfxAct_KeyIn {
  public:
    OrthoProbeKeyAct(vsnGfxView* pgv =NULL,
		     vsnMethod_Sv_orthoProbe* pm =NULL);
    void setOrthoProbeMethod(vsnMethod_Sv_orthoProbe* pm);
    void execute(vfrEvent& e);
  private:
    vsnMethod_Sv_orthoProbe* p_method;
    friend class vsnMPP_Sv_orthoProbe;
  } gfxAct_KI;

  //-------- OrthoProbeClickAct : left-click action class --------
  class OrthoProbeClickAct : public vfrAction, public vsnGfxBaseAct {
  public:
    OrthoProbeClickAct(vsnGfxView* pgv =NULL,
		       vsnMethod_Sv_orthoProbe* pm =NULL);
    void setOrthoProbeMethod(vsnMethod_Sv_orthoProbe* pm);
    void execute(vfrEvent& e);
  private:
    vsnMethod_Sv_orthoProbe* p_method;
    friend class vsnMPP_Sv_orthoProbe;
  } gfxAct_CLK;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoProbe
//----------------------------------------------------------------
class vsnMethod_Sv_orthoProbe
  : public vsnMethodObj, public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Sv_orthoProbe(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_orthoProbe();

  int getSliceAxis() const {return m_sliceAxis;}
  bool setSliceAxis(const int sa);
  CES::Vec3<size_t> getProbeIdx() const {return m_probeIdx;}
  bool setProbeIdx(const CES::Vec3<size_t>& pidx);

  float getGlyphBias() const {return m_glyphBias;}
  bool setGlyphBias(const float gb);
  float getPointSize() const {return m_pointSize;}
  bool setPointSize(const float ps);

  std::string getRepStr() const {return m_repStr;}
  bool clickSelect(const Point2& rpos, vfrScreen* pscr);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual void setLighting(const bool mode);
  virtual std::string getMethodType() const {
    return std::string("orthoProbe");
  }
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  int        m_sliceAxis;
  CES::Vec3<size_t>
             m_probeIdx;
  float      m_glyphBias;
  float      m_pointSize;

  vfrGroup*  m_root;
  vfrMesh2D* m_slice;
  vfrBall*   m_glyph;
  float      m_glyphRad;

  mutable bool m_idxInited;
  std::string  m_repStr;
  bool updateRepStr(const int stp);
};

#endif // _VSN_METHOD_SV_ORTHO_PROBE_H_
