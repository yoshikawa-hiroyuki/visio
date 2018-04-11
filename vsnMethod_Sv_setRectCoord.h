//
// vsnMethod_Sv_setRectCoord
//
#ifndef _VSN_METHOD_SV_SETRECTCOORD_H_
#define _VSN_METHOD_SV_SETRECTCOORD_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/radiobox.h"

#include "vsnMethodObj.h"
#include "vsnData_Sph.h"
#include "vsnGridUtilSv.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_setRectCoord_FileTxt =5650,
	MPP_Sv_setRectCoord_BrowsBtn,
	MPP_Sv_setRectCoord_SetRectCoordBtn,
	MPP_Sv_setRectCoord_OffsetXTxt,
	MPP_Sv_setRectCoord_OffsetYTxt,
	MPP_Sv_setRectCoord_OffsetZTxt
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_setRectCoord
//   method parameter-panel for vsnMethod_Sv_setRectCoord
//----------------------------------------------------------------
class vsnMPP_Sv_setRectCoord : public vsnMethodPP {
public:
  vsnMPP_Sv_setRectCoord(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_setRectCoord();

  // interface
  virtual bool update();

  // event handler
  void OnBrowsBtn(wxCommandEvent& event);
  void OnSetRectCoordBtn(wxCommandEvent& event);
  void OnOffsetTxts(wxCommandEvent& event);

private:
  wxTextCtrl* m_pFileTxt;
  wxButton*   m_pBrowsBtn;
  wxButton*   m_pSetRectCoordBtn;
  wxTextCtrl* m_pOffsetXTxt;
  wxTextCtrl* m_pOffsetYTxt;
  wxTextCtrl* m_pOffsetZTxt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_setRectCoord
//----------------------------------------------------------------
class vsnMethod_Sv_setRectCoord
  : public vsnMethodObj {
public:
  vsnMethod_Sv_setRectCoord(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_setRectCoord();

  bool setRectCoord(const std::string& mskPath, const size_t* ofst =NULL);

  std::string getPath() const {return m_rectCrdFile;}
  void getOffset(size_t* ofst) const;

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("setRectCoord");
  }
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  std::string m_rectCrdFile;
  size_t m_offset[3];
};

#endif // _VSN_METHOD_SV_SETMASK_H_
