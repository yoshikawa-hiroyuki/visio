//
// vsnXForm
//
#ifndef _VSN_XFORM_H_
#define _VSN_XFORM_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/statline.h"

#include <string>
#include <iostream>
#include <libxml/tree.h>

#include "vfrBase.h"

namespace VSN {
  // control ids
  enum {XFormDlg_Reset =1300,
	XFormDlg_Tx, XFormDlg_Ty, XFormDlg_Tz,
	XFormDlg_Rx, XFormDlg_Ry, XFormDlg_Rz,
	XFormDlg_Sx, XFormDlg_Sy, XFormDlg_Sz,
	XFormDlg_Cx, XFormDlg_Cy, XFormDlg_Cz,
	XFormDlg_Close
  };
};


//----------------------------------------------------------------
// class vsnXForm
//----------------------------------------------------------------
class vsnXForm {
public:
  vsnXForm() : m_S(1.f,1.f,1.f) {}
  virtual ~vsnXForm() {}

  // transformation interface
  virtual void updateXForm() =0;
  virtual void resetXForm() {
    m_T = CES::Vec3<float>(0.f,0.f,0.f);
    m_C = m_T; m_HPR = m_T;
    m_S = CES::Vec3<float>(1.f,1.f,1.f);
    updateXForm();
  }

  void setT(const CES::Vec3<float>& tv, const bool upd =true) {
    m_T = tv;
    if ( upd ) updateXForm();
  }
  CES::Vec3<float> getT() const {return m_T;}
  void setS(const CES::Vec3<float>& sv, const bool upd =true) {
    m_S = sv;
    if ( upd ) updateXForm();
  }
  CES::Vec3<float> getS() const {return m_S;}
  void setHPR(const CES::Vec3<float>& hpr, const bool upd =true) {
    m_HPR = hpr;
    if ( upd ) updateXForm();
  }
  CES::Vec3<float> getHPR() const {return m_HPR;}
  void setC(const CES::Vec3<float>& cv, const bool upd =true) {
    m_C = cv;
    if ( upd ) updateXForm();
  }
  CES::Vec3<float> getC() const {return m_C;}

  // XML util
  bool importXMLNode(xmlNodePtr xnp);
  bool exportXMLNode(std::ostream& os, const size_t ts =0) const;

protected:
  vfrMatrix getXFormMatrix() const {
    vfrMatrix M; M.Translate(m_T); M.Translate(m_C);
    M.RotY(CES::Deg2Rad(m_HPR[0]));
    M.RotX(CES::Deg2Rad(m_HPR[1]));
    M.RotZ(CES::Deg2Rad(m_HPR[2]));
    M.Scale(m_S); M.Translate(m_C * (-1.f));
    return M;
  }

  CES::Vec3<float> m_T;
  CES::Vec3<float> m_C;
  CES::Vec3<float> m_HPR;
  CES::Vec3<float> m_S;
};


//----------------------------------------------------------------
// class vsnXFormDlg
//----------------------------------------------------------------
class vsnXFormDlg : public wxDialog {
public:
  vsnXFormDlg(wxWindow *parent, vsnXForm* refXForm,
	      const bool useC =true, const bool useSz =true);
  ~vsnXFormDlg();

  /* interface */
  bool update(); // copy values from p_xform to this
  vsnXForm* getRefXForm() {return p_xform;}
  void setRefXForm(vsnXForm* refXForm);

  /* event handler */
  void OnChangeValues(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxTextCtrl *m_pTx, *m_pTy, *m_pTz;
  wxTextCtrl *m_pRx, *m_pRy, *m_pRz;
  wxTextCtrl *m_pSx, *m_pSy, *m_pSz;
  wxTextCtrl *m_pCx, *m_pCy, *m_pCz;
  wxButton *m_pReset, *m_pClose;

  vsnXForm *p_xform;
  void updateRefXForm(); // copy values from this to p_xform

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_XFORM_H_
