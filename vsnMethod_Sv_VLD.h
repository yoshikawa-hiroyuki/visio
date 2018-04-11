//
// vsnMethod_Sv_VLD
//
#ifndef _VSN_METHOD_SV_VLD_H_
#define _VSN_METHOD_SV_VLD_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnData_Sv.h"
#include "vfrLines.h"

#include "VLDContourMaker.h"

namespace VSN {
  // control ids
  enum {
	MPP_Sv_VLD_SelDataLst = 6800,
	MPP_Sv_VLD_VecDataChkLst,
	MPP_Sv_VLD_ValueSld,
	MPP_Sv_VLD_ValueTxt,
	MPP_Sv_VLD_UseCMapChk,
	MPP_Sv_VLD_UpdMinMaxChk,
  MPP_Sv_VLD_AntiAliasChk,
  MPP_Sv_VLD_LineWidthTxt,
  
  MPP_Sv_VLD_EnableFastMethodChk,
	MPP_Sv_VLD_EnableVisibilityTestingChk,

	MPP_Sv_VLD_SelIsoValueLst,
	MPP_Sv_VLD_ShowContourChk,
	MPP_Sv_VLD_ShowSuggestiveContourChk,

	MPP_Sv_VLD_EnableCuttingPlaneChk,
  MPP_Sv_VLD_SamplerLst,
  MPP_Sv_VLD_SamplerTxt,
	MPP_Sv_VLD_SetSamplerBtn
  };
};

//----------------------------------------------------------------
// class vsnMPP_Sv_VLD
//   method parameter-panel for vsnMethod_Sv_VLD
//----------------------------------------------------------------
class vsnMPP_Sv_VLD : public vsnMethodPP {
public: 
  vsnMPP_Sv_VLD(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_VLD();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnValueTxt(wxCommandEvent& event);
  void OnUseCMapChk(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);

  void OnEnableFastMethodChk(wxCommandEvent& event);
  void OnEnableVisibilityTestingChk(wxCommandEvent& event);

  void OnSelIsoValueLst(wxCommandEvent& event);
  void OnShowContourChk(wxCommandEvent& event);
  void OnShowSuggestiveContourChk(wxCommandEvent& event);

  void OnEnableCuttingPlaneChk(wxCommandEvent& event);
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnEnterSamplerTxt(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pValueTxt;
  wxCheckBox*     m_pUseCMapChk;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxCheckBox*     m_pAntiAliasChk;
  wxTextCtrl*     m_pLineWidthTxt;

  wxCheckBox*     m_pEnableFastMethodChk;
  wxCheckBox*     m_pEnableVisibilityTestingChk;
  
  wxComboBox*     m_pSelIsoValueLst;
  wxCheckBox*     m_pShowContourChk;
  wxCheckBox*     m_pShowSuggestiveContourChk;

  wxCheckBox*     m_pEnableCuttingPlaneChk;
  wxListBox*      m_pSamplerLst;
  wxTextCtrl*     m_pSamplerTxt;
  wxButton*       m_pSetSamplerBtn;

  DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------
// class vsnMethod_Sv_VLD
//----------------------------------------------------------------
class vsnMethod_Sv_VLD
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnRef_sampler {
public:
  vsnMethod_Sv_VLD(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_VLD();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setUseCMap(const bool ucm);
  bool getUseCMap() const {return m_useCMap;}

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}

  bool setLineWidth(const float lw);
  float getLineWidth() const {return m_lineWidth;}

  bool setEnableFastMethod(const bool efm);
  bool getEnableFastMethod() const { return m_enableFastMethod;}

  bool setEnableVisibilityTesting(const bool evt);
  bool getEnableVisibilityTesting() const { return m_enableVisibilityTesting;}

  bool setSelectedIsoValue(const int siv);
  int getSelectedIsoValue() const {return m_selectedIsoValue;}
  int getMaxIsoValues() const {return MaxIsoValues;}

  bool setIsoValue(const int iv, const float val);
  float getIsoValue(const int iv) const {return m_isoValue[iv];}

  bool setShowContour(const int iv, const bool sc);
  bool getShowContour(const int iv) const { return m_showContour[iv];}

  bool setShowSuggestiveContour(const int iv, const bool ssc);
  bool getShowSuggestiveContour(const int iv) const { return m_showSuggestiveContour[iv];}

  bool setEnableCuttingPlane(const int iv, const bool ecp);
  bool getEnableCuttingPlane(const int iv) const { return m_enableCuttingPlane[iv];}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("VLD");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);
  virtual bool canLighting() const {return false;}

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from vsnRef_sampler
  virtual void noticeUpdate();


  virtual void render(const Bool transpMode, vfrMaterialStack *mstk){
    if ( ! m_show ) return;
    _updateContour();
    for ( register int c = 0; c < nChild; c++ ) {
      _children[c]->render(transpMode, mstk);
    }
  }

private:
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  bool           m_useCMap;
  bool           m_updateMinMax;

  bool           m_enableFastMethod;
  bool           m_enableVisibilityTesting;

  enum{MaxIsoValues = 3};
  int  m_selectedIsoValue;
  float          m_isoValue[MaxIsoValues];
  bool           m_showContour[MaxIsoValues];
  bool           m_showSuggestiveContour[MaxIsoValues];
  float          m_lineWidth;
  bool           m_enableCuttingPlane[MaxIsoValues];

  vfrLines*      m_contours[MaxIsoValues];
  vfrLines*      m_suggestiveContours[MaxIsoValues];
  vfrLines*      m_intersectPlane[MaxIsoValues];

  float*         m_pd;

  void adjustRange(const bool updval =true);
  void updateColor();

  bool           _isEqualIntervals();
  void           _outPutErrorMessage(const std::string &msg);
  // for contour
  bool           _updateIsoValueContour[MaxIsoValues]; // for contour
  bool           _updateIsoValueSuggestiveContour[MaxIsoValues]; // for suggestive contour 
  bool           _updateIsoValueIntersectPlane[MaxIsoValues]; // for intersect plane
  bool           _updateSampler[MaxIsoValues];
  bool           _needRemake;
  bool           _updateVisibility;

  VLD::V3f          _viewPosContour[MaxIsoValues]; // for contour
  VLD::V3f          _viewPosSuggestiveContour[MaxIsoValues]; // for suggestive contour
  VLD::ContourMaker _contourMaker[MaxIsoValues];
  VLD::Lines        _linesContour[MaxIsoValues];
  VLD::Lines        _linesSuggestiveContour[MaxIsoValues];
  VLD::Lines        _linesIntersectPlane[MaxIsoValues];

  enum{ACCEPTABLE_FOUND_RATE = 10};
  void _updateContour();
  void _updateContour(const int iv);
  void _updateContourByFastMethod(const int iv);
  void _updateSuggestiveContour(const int iv);
  void _updateIntersectPlane(const int iv);
  void _setLinesContour(const int iv, const bool enableVisibilityTesting, const bool enableCuttingPlane);
  void _setLinesSuggestiveContour(const int iv, const bool enableVisibilityTesting, const bool enableCuttingPlane);
  void _setLinesIntersectPlane(const int iv);

  bool _enableSampler();
  VLD::V3i _getDataSize();
  VLD::V3f _getOrigin();
  float    _getCellScale();
  VLD::V3f _getViewPosition();
  VLD::V3f _getCuttingPlaneNormal();
  VLD::V3f _getCuttingPlanePosition();


//  bool _getCuttinPlaneNormalAndPosition(VLD::V3f &normal, VLD::V3f &position);

};

#endif // _VSN_METHOD_SV_VLD_H_
