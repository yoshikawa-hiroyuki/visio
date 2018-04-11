//
// vsnMethod_plotContour
//
#ifndef _VSN_METHOD_PLOTCONTOUR_H_
#define _VSN_METHOD_PLOTCONTOUR_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnDataObj.h"
#include "vfrLines.h"

namespace VSN {
  // control ids
  enum {MPP_plotContour_SamplerLst = 2300,
	MPP_plotContour_SamplerTxt,
	MPP_plotContour_SetSamplerBtn,
	MPP_plotContour_SelDataLst,
	MPP_plotContour_VecDataChkLst,
	MPP_plotContour_NumLinesTxt,
	MPP_plotContour_MinTxt,
	MPP_plotContour_MaxTxt,
	MPP_plotContour_UseCMapChk,
	MPP_plotContour_UpdMinMaxChk,
	MPP_plotContour_LineWidthTxt,
	MPP_plotContour_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_plotContour
//   method parameter-panel for vsnMethod_plotContour
//----------------------------------------------------------------
class vsnMPP_plotContour : public vsnMethodPP {
public: 
  vsnMPP_plotContour(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_plotContour();

  // interface
  virtual bool update();

  // event handler
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnEnterSamplerTxt(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnNumLinesTxt(wxCommandEvent& event);
  void OnMinMaxTxt(wxCommandEvent& event);
  void OnUseCMapChk(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxListBox*      m_pSamplerLst;
  wxTextCtrl*     m_pSamplerTxt;
  wxButton*       m_pSetSamplerBtn;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pNumLinesTxt;
  wxTextCtrl*     m_pMinTxt;
  wxTextCtrl*     m_pMaxTxt;
  wxCheckBox*     m_pUseCMapChk;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_plotContour
//----------------------------------------------------------------
class vsnMethod_plotContour
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnRef_sampler {
public:
  vsnMethod_plotContour(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_plotContour();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setNumLines(const size_t nl);
  size_t getNumLines() const {return m_numLines;}

  bool setRange(const float min, const float max);
  void getRange(float& min, float& max) const {min = m_min; max = m_max;}

  bool setUseCMap(const bool ucm);
  bool getUseCMap() const {return m_useCMap;}

  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  bool getUseSampler() const {return m_useSampler;}

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotContour");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setUseLut(const bool ulm);
  virtual void setLut(const vsnLut& ol);

  // from vsnTimeSeriesMethodIF
  //  need to implement
  //virtual bool updateStep(const int stp,
  //                        const bool force =true, const bool cascade =true);

  // from vsnRef_sampler
  virtual void noticeUpdate();

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  size_t         m_numLines;
  size_t         m_numValidLines;
  float          m_min, m_max;
  bool           m_useCMap;
  bool           m_updateMinMax;
  float          m_lineWidth;
  bool           m_useSampler;

  void adjustRange();

  static
  bool calcLineSegment(const vector3 p0, const vector3 p1, const vector3 p2,
                       const float val0, const float val1, const float val2,
                       const float val, vector3 lp1, vector3 lp2);
};

#endif // _VSN_METHOD_PLOTCONTOUR_H_
