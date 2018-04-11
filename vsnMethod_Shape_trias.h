//
// vsnMethod_Shape_trias
//
#ifndef _VSN_METHOD_SHAPE_TRIAS_H_
#define _VSN_METHOD_SHAPE_TRIAS_H_

#include "wx/defs.h"

#include "vsnMethodObj.h"
#include "vfrPrimSet.h"

namespace VSN {
  // control ids
  enum {MPP_Shape_trias_RenType = 3100,
	MPP_Shape_trias_LineWidth,
	MPP_Shape_trias_PointSize,
	MPP_Shape_trias_AntiAlias
  };
};


//----------------------------------------------------------------
// class vsnMPP_Shape_trias
//   method parameter-panel for vsnMethod_Shape_trias
//----------------------------------------------------------------
class vsnMPP_Shape_trias
  : public vsnMethodPP {
public: 
  vsnMPP_Shape_trias(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Shape_trias();

  // interface
  virtual bool update();

  // event handler
  void OnRenTypeRadio(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnPointSizeTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxRadioBox* m_pRenTypeRadio;
  wxTextCtrl* m_pLineWidthTxt;
  wxTextCtrl* m_pPointSizeTxt;
  wxCheckBox* m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Shape_trias
//----------------------------------------------------------------
class vsnMethod_Shape_trias
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Shape_trias(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Shape_trias();

  VFR::RenderType getRenderType() const {return m_renType;}
  virtual bool setRenderType(const VFR::RenderType rt);

  float getLineWidth() const {return m_lineWidth;}
  virtual bool setLineWidth(const float lw);

  float getPointSize() const {return m_pointSize;}
  virtual bool setPointSize(const float psz);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Shape")); return r;
  }
  virtual std::string getMethodType() const {return std::string("trias");}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setLighting(const bool mode);
  virtual bool getLighting() const {return m_lightMode;}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  VFR::RenderType m_renType;
  float           m_lineWidth;
  float           m_pointSize;
  bool            m_lightMode;

  vfrPrimSet*     m_shape;
};


//----------------------------------------------------------------
// class vsnMethod_Shape_trias2
//----------------------------------------------------------------
class vsnMethod_Shape_trias2 : public vsnMethod_Shape_trias {
public:
  vsnMethod_Shape_trias2(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Shape_trias2();

  // from vsnMethod_Shape_trias
  virtual bool setRenderType(const VFR::RenderType rt);
  virtual bool setLineWidth(const float lw);
  virtual bool setPointSize(const float psz);
  virtual bool setAntiAliasMode(const bool aam);

  // from vsnMethodObj
  virtual std::string getMethodType() const {return std::string("trias2");}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  vfrGroup*  m_refShape;
};

#endif // _VSN_METHOD_SHAPE_TRIAS_H_
