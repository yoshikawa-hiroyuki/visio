//
// vsnMethod_Sv_orthoContour
//
#ifndef _VSN_METHOD_SV_ORTHOCONTOUR_H_
#define _VSN_METHOD_SV_ORTHOCONTOUR_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_Sv_orthoSlicer.h"
#include "vsnData_Sv.h"
#include "vfrLines.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_orthoContour_SlicerLst = 5700,
	MPP_Sv_orthoContour_SlicerTxt,
	MPP_Sv_orthoContour_SetSlicerBtn,
	MPP_Sv_orthoContour_SelDataLst,
	MPP_Sv_orthoContour_VecDataChkLst,
	MPP_Sv_orthoContour_NumLinesTxt,
	MPP_Sv_orthoContour_BiLinearChk,
	MPP_Sv_orthoContour_MinTxt,
	MPP_Sv_orthoContour_MaxTxt,
	MPP_Sv_orthoContour_UseCMapChk,
	MPP_Sv_orthoContour_UpdMinMaxChk,
	MPP_Sv_orthoContour_LineWidthTxt,
	MPP_Sv_orthoContour_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoContour
//   method parameter-panel for vsnMethod_Sv_orthoContour
//----------------------------------------------------------------
class vsnMPP_Sv_orthoContour : public vsnMethodPP {
public: 
  vsnMPP_Sv_orthoContour(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_orthoContour();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnSetSlicerBtn(wxCommandEvent& event);
  void OnSlicerTxt(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnNumLinesTxt(wxCommandEvent& event);
  void OnBiLinearChk(wxCommandEvent& event);
  void OnMinMaxTxt(wxCommandEvent& event);
  void OnUseCMapChk(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pSlicerTxt;
  wxListBox*      m_pSlicerList;
  wxButton*       m_pSetSlicerBtn;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pNumLinesTxt;
  wxCheckBox*     m_pBiLinearChk;
  wxTextCtrl*     m_pMinTxt;
  wxTextCtrl*     m_pMaxTxt;
  wxCheckBox*     m_pUseCMapChk;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  vsnRefOrthoSlicerKeyAct gfxAct_KI;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoContour
//----------------------------------------------------------------
class vsnMethod_Sv_orthoContour
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnRef_Sv_orthoSlicer {
public:
  vsnMethod_Sv_orthoContour(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_orthoContour();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setNumLines(const size_t nl);
  size_t getNumLines() const {return m_numLines;}

  bool setBiLinearMode(const bool blm);
  bool getBiLinearMode() const {return m_biLinear;}

  bool setRange(const float min, const float max);
  void getRange(float& min, float& max) const {min = m_min; max = m_max;}

  bool setUseCMap(const bool ucm);
  bool getUseCMap() const {return m_useCMap;}

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("orthoContour");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setUseLut(const bool ulm);
  virtual void setLut(const vsnLut& ol);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from vsnRef_Sv_orthoSlicer
  virtual void noticeUpdate();

private:
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  size_t         m_numLines;
  size_t         m_numValidLines;
  bool           m_biLinear;
  float          m_min, m_max;
  bool           m_useCMap;
  bool           m_updateMinMax;
  float          m_lineWidth;

  void adjustRange();
  size_t countValidCells(const float val, const float* dptr,
			 const VFR::Point2& sliceSize,
			 const int* const sliceIndices,
			 const size_t dlen) const;

  static
  bool calcLineSegment(const vector3 p0, const vector3 p1, const vector3 p2,
		       const float val0, const float val1, const float val2,
		       const float val, vector3 lp1, vector3 lp2);

  static
  void calcCP(const vector3 p1, const vector3 p2,
	      const vector3 p3, const vector3 p4, vector3 cp);
			  
};

#endif // _VSN_METHOD_SV_ORTHOCONTOUR_H_
