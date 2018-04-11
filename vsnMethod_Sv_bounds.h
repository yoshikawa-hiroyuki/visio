//
// vsnMethod_Sv_bounds
//
#ifndef _VSN_METHOD_SV_BOUNDS_H_
#define _VSN_METHOD_SV_BOUNDS_H_

#include "wx/defs.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"
#include "vfrIndexLines.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_bounds_LineWidthTxt = 5100,
	MPP_Sv_bounds_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_bounds
//   method parameter-panel for vsnMethod_Sv_bounds
//----------------------------------------------------------------
class vsnMPP_Sv_bounds : public vsnMethodPP {
public:
  vsnMPP_Sv_bounds(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_bounds();

  // interface
  virtual bool update();

  // event handler
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxTextCtrl* m_pLineWidthTxt;
  wxCheckBox* m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_bounds
//----------------------------------------------------------------
class vsnMethod_Sv_bounds
  : public vsnMethodObj, public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Sv_bounds(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_bounds();

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
			  const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {return std::string("bounds");}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  float m_lineWidth;

  vfrIndexLines* m_bounds;
};

#endif // _VSN_METHOD_SV_BOUNDS_H_

