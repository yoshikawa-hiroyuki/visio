//
// vsnMethod_label
//
#ifndef _VSN_METHOD_LABEL_H_
#define _VSN_METHOD_LABEL_H_

#include "wx/defs.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnXForm.h"
#include "vfrHelveticaText.h"


namespace VSN {
  // control ids
  enum {MPP_label_LabelTxt =1850,
        MPP_label_LabelBtn,
        MPP_label_XFormBtn
  };
};


//----------------------------------------------------------------
// class vsnMPP_label
//   method parameter-panel for vsnMethod_label
//----------------------------------------------------------------
class vsnMPP_label : public vsnMethodPP {
public: 
  vsnMPP_label(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_label();

  // interface
  virtual bool update();
  virtual void settlement();
  void showXFormDlg(const bool show =true);

  // event handler
  void OnLabelBtn(wxCommandEvent& event);
  void OnXFormBtn(wxCommandEvent& event);

private:
  vsnXFormDlg* m_pXFormDlg;
  wxButton*    m_pXFormBtn;
  wxTextCtrl*  m_pLabelTxt;
  wxButton*    m_pLabelBtn;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_label
//----------------------------------------------------------------
class vsnMethod_label
  : public vsnMethodObj, public vsnXForm
{
public:
  vsnMethod_label(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_label();

  // label string
  void setLabelStr(const std::string& str);
  std::string getLabelStr() const {return m_labelStr;}

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {return std::string("label");}
  virtual bool hasBbox() const {return false;}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  vfrHelveticaText *m_pLabel;
  std::string       m_labelStr;
  std::string getLabelStrOut() const;

  vfrMatrix         m_matXForm;
  CES::Vec3<float>  m_initT;
  CES::Vec3<float>  m_initS;
  CES::Vec3<float>  m_initHPR;
  bool              m_xformInited;

  // from vsnXForm
  virtual void updateXForm();
  virtual void resetXForm();
};

#endif // _VSN_METHOD_LABEL_H_
