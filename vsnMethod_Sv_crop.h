//
// vsnMethod_Sv_crop
//
#ifndef _VSN_METHOD_SV_CROP_H_
#define _VSN_METHOD_SV_CROP_H_

#include "wx/button.h"
#include "wx/slider.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_crop_ApplyBtn =5550,
	MPP_Sv_crop_NoneBtn,
	MPP_Sv_crop_X1Sld,
	MPP_Sv_crop_X2Sld,
	MPP_Sv_crop_X1Txt,
	MPP_Sv_crop_X2Txt,
	MPP_Sv_crop_Y1Sld,
	MPP_Sv_crop_Y2Sld,
	MPP_Sv_crop_Y1Txt,
	MPP_Sv_crop_Y2Txt,
	MPP_Sv_crop_Z1Sld,
	MPP_Sv_crop_Z2Sld,
	MPP_Sv_crop_Z1Txt,
	MPP_Sv_crop_Z2Txt
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_crop
//   method parameter-panel for vsnMethod_Sv_crop
//----------------------------------------------------------------
class vsnMPP_Sv_crop : public vsnMethodPP {
public:
  vsnMPP_Sv_crop(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_crop();

  // interface
  virtual bool update();

  // event handler
  void OnApplyBtn(wxCommandEvent& event);
  void OnNoneBtn(wxCommandEvent& event);
  void OnX1Sld(wxScrollEvent& event);
  void OnX2Sld(wxScrollEvent& event);
  void OnX1Txt(wxCommandEvent& event);
  void OnX2Txt(wxCommandEvent& event);
  void OnY1Sld(wxScrollEvent& event);
  void OnY2Sld(wxScrollEvent& event);
  void OnY1Txt(wxCommandEvent& event);
  void OnY2Txt(wxCommandEvent& event);
  void OnZ1Sld(wxScrollEvent& event);
  void OnZ2Sld(wxScrollEvent& event);
  void OnZ1Txt(wxCommandEvent& event);
  void OnZ2Txt(wxCommandEvent& event);

private:
  wxButton*   m_pApplyBtn;
  wxButton*   m_pNoneBtn;
  wxSlider*   m_pX1Sld;
  wxSlider*   m_pX2Sld;
  wxSlider*   m_pY1Sld;
  wxSlider*   m_pY2Sld;
  wxSlider*   m_pZ1Sld;
  wxSlider*   m_pZ2Sld;
  wxTextCtrl* m_pX1Txt;
  wxTextCtrl* m_pX2Txt;
  wxTextCtrl* m_pY1Txt;
  wxTextCtrl* m_pY2Txt;
  wxTextCtrl* m_pZ1Txt;
  wxTextCtrl* m_pZ2Txt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_crop
//----------------------------------------------------------------
class vsnMethod_Sv_crop :
  public vsnMethodObj
{
public:
  vsnMethod_Sv_crop(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_crop();

  enum ParamType {NoneNone =0, X1, X2, Y1, Y2, Z1, Z2};

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("crop");
  }
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // value
  void getXValue(int xv[2]) const {
    xv[0] = m_x1; xv[1] = m_x2;
  }
  void getYValue(int yv[2]) const {
    yv[0] = m_y1; yv[1] = m_y2;
  }
  void getZValue(int zv[2]) const {
    zv[0] = m_z1; zv[1] = m_z2;
  }

  bool setParam(const ParamType type =NoneNone, const int val =0);
  bool setMask(const bool apply);
  bool setMask(const bool apply, const int x1, const int x2,
	       const int y1, const int y2, const int z1, const int z2);

private:
  int     m_x1, m_x2;
  int     m_y1, m_y2;
  int     m_z1, m_z2;
};

#endif // _VSN_METHOD_SV_CROP_H_
