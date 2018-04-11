//
// vsnMethod_sampler
//
#ifndef _VSN_METHOD_SAMPLER_H_
#define _VSN_METHOD_SAMPLER_H_

#include "wx/defs.h"

#include "vsnMethodObj.h"
#include "vsnXForm.h"
#include "vsnGfxActions.h"
#include "vfrLineStrip.h"
#include <set>
#include <deque>

namespace VSN {
  // control ids
  enum {MPP_sampler_DivM = 1700,
	MPP_sampler_DivN,
	MPP_sampler_XFormBtn,
	MPP_sampler_UseMouseChk,
	MPP_sampler_LineWidth,
	MPP_sampler_PointSize,
	MPP_sampler_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_sampler
//   method parameter-panel for vsnMethod_sampler
//----------------------------------------------------------------
class vsnMPP_sampler : public vsnMethodPP {
public:
  vsnMPP_sampler(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_sampler();

  // interface
  virtual bool update();
  virtual void settlement();
  void showXFormDlg(const bool show =true);
  void setUseMouseMode(const bool umm);

  // event handler
  void OnDivChanged(wxCommandEvent& event);
  void OnLineWidthChanged(wxCommandEvent& event);
  void OnPointSizeChanged(wxCommandEvent& event);
  void OnXFormBtn(wxCommandEvent& event);
  void OnUseMouseChk(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  vsnXFormDlg* m_pXFormDlg;

  wxTextCtrl*  m_pDivMTxt;
  wxTextCtrl*  m_pDivNTxt;
  wxTextCtrl*  m_pLineWidthTxt;
  wxTextCtrl*  m_pPointSizeTxt;
  wxButton*    m_pXformBtn;
  wxCheckBox*  m_pUseMouseChk;
  wxCheckBox*  m_pAntiAliasChk;

  //-------- SamplerDragTransAct --------
  class SamplerDragTransAct : public vfrAction, public vsnGfxBaseAct {
  public:
    SamplerDragTransAct(vsnGfxView* pgv =NULL,
			class vsnMethod_sampler* pm =NULL);
    void setSamplerMethod(class vsnMethod_sampler* pm);
    void execute(vfrEvent& e);
  private:
    class vsnMethod_sampler* p_method;
    friend class vsnMPP_sampler;
  } gfxAct_DTG;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnRef_sampler
//   refers to vsnMethod_sampler
//----------------------------------------------------------------
class vsnRef_sampler {
public:
  vsnRef_sampler() : p_splr(NULL) {}
  virtual ~vsnRef_sampler();

  std::deque<class vsnMethod_sampler*> getSamplerList(class vsnDataObj* pdo);
  class vsnMethod_sampler* getSampler() {return p_splr;}

  void setSampler(class vsnMethod_sampler* splr);
  void setSampler(class vsnDataObj* pdo, const int n);
  bool setSampler(class vsnDataObj* pdo, const string& samplerName);

  void noticeDie();
  virtual void noticeUpdate() =0;

  // XML util
  bool exportXMLNode(std::ostream& os, const size_t ts =0) const;
  
protected:
  class vsnMethod_sampler* p_splr;
};


//----------------------------------------------------------------
// class vsnMethod_sampler
//----------------------------------------------------------------
class vsnMethod_sampler
  : public vsnMethodObj, public vsnXForm {
public:
  vsnMethod_sampler(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_sampler();

  // sampling number
  Point2 getSampleNumber() const {return m_sampleSize;}
  bool setSampleNumber(const Point2& sn);

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  float getPointSize() const {return m_pointSize;}
  bool setPointSize(const float psz);

  virtual Point2 getIniSampleNumber() const {
    Point2 s; s.x = 8; s.y = 1; return s;
  }
  virtual float getIniLineWidth() const {return 1.f;}
  virtual float getIniPointSize() const {return 2.f;}

  // output interface
  const vector3* const getSamplePoints() {return getVerts();}
  CES::Vec3<float> getNormalVec() const;

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {return std::string("sampler");}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // refMethodLst control
  void appendRefMethod(vsnRef_sampler* refm);
  void removeRefMethod(vsnRef_sampler* refm);

protected:
  Point2  m_sampleSize;
  float   m_lineWidth;
  float   m_pointSize;

  vfrLineStrip* m_outline;
  vfrLineStrip* m_points;

  vfrMatrix        m_matXForm;
  CES::Vec3<float> m_initT;
  CES::Vec3<float> m_initS;
  CES::Vec3<float> m_initHPR;
  bool             m_xformInited;

  // from vsnXForm
  virtual void updateXForm();
  virtual void resetXForm();

  // initialize XForm
  virtual void initXForm();

  std::set<vsnRef_sampler*> m_refMethodLst;
  void updateRefMethod();
};


//----------------------------------------------------------------
// class vsnMethod_sliceSampler
//----------------------------------------------------------------
class vsnMethod_sliceSampler : public vsnMethod_sampler {
public:
  vsnMethod_sliceSampler(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_sliceSampler() {}

  // override vsnMethod_sampler
  virtual Point2 getIniSampleNumber() const {
    Point2 s; s.x = s.y = 32; return s;
  }
  virtual float getIniPointSize() const {return 0.f;}

protected:
  // override vsnMethod_sampler
  virtual void initXForm();
};

#endif // _VSN_METHOD_SAMPLER_H_
