//
// vsnMethod_probe
//
#ifndef _VSN_METHOD_PROBE_H_
#define _VSN_METHOD_PROBE_H_

#include "wx/defs.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnXForm.h"
#include "vsnGfxActions.h"

namespace VSN {
  // control ids
  enum {MPP_probe_RepTxt =1800,
	MPP_probe_XFormBtn,
	MPP_probe_UseMouseChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_probe
//   method parameter-panel for vsnMethod_probe
//----------------------------------------------------------------
class vsnMPP_probe : public vsnMethodPP {
public: 
  vsnMPP_probe(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_probe();

  // interface
  virtual bool update();
  virtual void settlement();
  void showXFormDlg(const bool show =true);
  void setUseMouseMode(const bool umm);

  // event handler
  void OnXFormBtn(wxCommandEvent& event);
  void OnUseMouseChk(wxCommandEvent& event);

private:
  vsnXFormDlg* m_pXFormDlg;

  wxButton*    m_pXFormBtn;
  wxCheckBox*  m_pUseMouseChk;
  wxTextCtrl*  m_pRepTxt;

  //-------- ProbeDragTransAct --------
  class ProbeDragTransAct : public vfrAction, public vsnGfxBaseAct {
  public:
    ProbeDragTransAct(vsnGfxView* pgv =NULL,
		      class vsnMethod_probe* pm =NULL);
    void setProbeMethod(class vsnMethod_probe* pm);
    void execute(vfrEvent& e);
  private:
    class vsnMethod_probe* p_method;
    friend class vsnMPP_probe;
  } gfxAct_DTG;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_probe
//----------------------------------------------------------------
class vsnMethod_probe
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnXForm {
public:
  vsnMethod_probe(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_probe();

  // report string
  std::string getRepStr() const {return m_repStr;}

  // from vsnTimeSeriesMethodIF
  //   delivered class must implements:
  //   virtual bool updateStep();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {return std::string("probe");}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  vfrGroup*        m_glyph;
  std::string      m_repStr;

  vfrMatrix        m_matXForm;
  CES::Vec3<float> m_initT;
  CES::Vec3<float> m_initS;
  CES::Vec3<float> m_initHPR;
  bool             m_xformInited;

  // from vsnXForm
  virtual void updateXForm();
  virtual void resetXForm();
};

#endif // _VSN_METHOD_PROBE_H_
