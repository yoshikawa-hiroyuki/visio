//
// vsnMethod_ShapeLOD_trias_lod
//
#ifndef _VSN_METHOD_SHAPE_LOD_TRIAS_LOD_H_
#define _VSN_METHOD_SHAPE_LOD_TRIAS_LOD_H_

#include "wx/defs.h"

#include "vsnMethodObj.h"
#include "vsnLOD.h"

namespace VSN {
  // control ids
  enum {MPP_ShapeLOD_trias_lod_Areas = 3300,
	MPP_ShapeLOD_trias_lod_RenType,
	MPP_ShapeLOD_trias_lod_LineWidth,
	MPP_ShapeLOD_trias_lod_PointSize,
	MPP_ShapeLOD_trias_lod_AntiAlias
  };
};


//----------------------------------------------------------------
// class vsnMPP_ShapeLOD_trias_lod
//   method parameter-panel for vsnMethod_ShapeLOD_trias_lod
//----------------------------------------------------------------
class vsnMPP_ShapeLOD_trias_lod
  : public vsnMethodPP {
public: 
  vsnMPP_ShapeLOD_trias_lod(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_ShapeLOD_trias_lod();

  // interface
  virtual bool update();

  // event handler
  void OnAreasTxt(wxCommandEvent& event);
  void OnRenTypeRadio(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnPointSizeTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxTextCtrl* m_pAreasTxt;
  wxRadioBox* m_pRenTypeRadio;
  wxTextCtrl* m_pLineWidthTxt;
  wxTextCtrl* m_pPointSizeTxt;
  wxCheckBox* m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_ShapeLOD_trias_lod
//----------------------------------------------------------------
class vsnMethod_ShapeLOD_trias_lod
  : public vsnMethodObj {
public:
  vsnMethod_ShapeLOD_trias_lod(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_ShapeLOD_trias_lod();

  vsnLOD::AreaListType getAreaList() const;
  bool setAreaList(const vsnLOD::AreaListType& al);

  VFR::RenderType getRenderType() const {return m_renType;}
  virtual bool setRenderType(const VFR::RenderType rt);

  float getLineWidth() const {return m_lineWidth;}
  virtual bool setLineWidth(const float lw);

  float getPointSize() const {return m_pointSize;}
  virtual bool setPointSize(const float psz);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("ShapeLOD")); return r;
  }
  virtual std::string getMethodType() const {return std::string("trias_lod");}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setLighting(const bool mode);
  virtual bool getLighting() const {return m_lightMode;}

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  VFR::RenderType m_renType;
  float           m_lineWidth;
  float           m_pointSize;
  bool            m_lightMode;

  vsnLOD*         m_pLOD;
};

#endif // _VSN_METHOD_SHAPE_LOD_TRIAS_LOD_H_
