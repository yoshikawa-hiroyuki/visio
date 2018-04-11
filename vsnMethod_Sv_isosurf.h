//
// vsnMethod_Sv_isosurf
//
#ifndef _VSN_METHOD_SV_ISOSURF_H_
#define _VSN_METHOD_SV_ISOSURF_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"
#include "vfrTriangles.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_isosurf_SelDataLst = 5800,
	MPP_Sv_isosurf_VecDataChkLst,
	MPP_Sv_isosurf_ValueSld,
	MPP_Sv_isosurf_ValueTxt,
	MPP_Sv_isosurf_UseCMapChk,
	MPP_Sv_isosurf_UseCMapAlphaChk,
	MPP_Sv_isosurf_UpdMinMaxChk,
	MPP_Sv_isosurf_ShowFaceRadio,
	MPP_Sv_isosurf_ExportBtn,
	MPP_Sv_isosurf_AutoExportChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_isosurf
//   method parameter-panel for vsnMethod_Sv_isosurf
//----------------------------------------------------------------
class vsnMPP_Sv_isosurf : public vsnMethodPP {
public: 
  vsnMPP_Sv_isosurf(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_isosurf();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnValueTxt(wxCommandEvent& event);
  void OnUseCMapChk(wxCommandEvent& event);
  void OnUseCMapAlphaChk(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnShowFaceRadio(wxCommandEvent& event);
  void OnExportBtn(wxCommandEvent& event);
  void OnAutoExportChk(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pValueTxt;
  wxCheckBox*     m_pUseCMapChk;
  wxCheckBox*     m_pUseCMapAlphaChk;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxRadioBox*     m_pShowFaceRadio;
  wxButton*       m_pExportBtn;
  wxCheckBox*     m_pAutoExportChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_isosurf
//----------------------------------------------------------------
class vsnMethod_Sv_isosurf
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Sv_isosurf(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_isosurf();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setIsoValue(const float val);
  float getIsoValue() const {return m_isoValue;}

  bool setUseCMap(const bool ucm);
  bool getUseCMap() const {return m_useCMap;}

  bool setUseCMapAlpha(const bool ucm);
  bool getUseCMapAlpha() const {return m_useCMapAlpha;}

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}

  bool setShowFaceMode(const PolyFaceType fm);
  PolyFaceType getShowFaceMode() const {return m_faceMode;}

  bool exportFaces(const std::string& path);
  std::string getExportedPath() const {return m_exportPath;}
  bool setAutoExport(const bool aem);
  bool getAutoExport() const {return m_autoExport;}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("isosurf");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // override vfrGroup method
  virtual void generateBbox();
  virtual void generateBbox(CES::Vec3<float>&) {generateBbox();}

private:
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  float          m_isoValue;
  bool           m_useCMap;
  bool           m_useCMapAlpha;
  bool           m_updateMinMax;
  PolyFaceType   m_faceMode;

  vfrTriangles*  m_isoTrias;
  float*         m_pd;

  std::string    m_exportPath;
  bool           m_autoExport;

  void adjustRange(const bool updval =true);
  void updateColor();
};

#endif // _VSN_METHOD_SV_ISOSURF_H_
