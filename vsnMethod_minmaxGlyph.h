//
// vsnMethod_minmaxGlyph
//
#ifndef _VSN_METHOD_MINMAX_GLYPH_H_
#define _VSN_METHOD_MINMAX_GLYPH_H_

#include "wx/defs.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"

#include "vsnMethodObj.h"
#include "vsnDataObj.h"
#include "vfrTriangles.h"

namespace VSN {
  // control ids
  enum {MPP_minmaxGlyph_SelDataLst = 1750,
	MPP_minmaxGlyph_VecDataChkLst,
	MPP_minmaxGlyph_ToleranceTxt,
	MPP_minmaxGlyph_ShowMinChk,
	MPP_minmaxGlyph_ShowMaxChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_minmaxGlyph
//   method parameter-panel for vsnMethod_minmaxGlyph
//----------------------------------------------------------------
class vsnMPP_minmaxGlyph : public vsnMethodPP {
public: 
  vsnMPP_minmaxGlyph(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_minmaxGlyph();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnToleranceTxt(wxCommandEvent& event);
  void OnShowMinChk(wxCommandEvent& event);
  void OnShowMaxChk(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pToleranceTxt;
  wxCheckBox*     m_pShowMinChk;
  wxCheckBox*     m_pShowMaxChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_minmaxGlyph
//----------------------------------------------------------------
class vsnMethod_minmaxGlyph
  : public vsnMethodObj, public vsnTimeSeriesMethodIF {
public:
  vsnMethod_minmaxGlyph(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_minmaxGlyph();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  float getTolerance() const {return m_tolerance;}
  bool setTolerance(const float t);
  bool getShowMin() const {return m_showMin;}
  bool setShowMin(const bool sm);
  bool getShowMax() const {return m_showMax;}
  bool setShowMax(const bool sm);

  // delivered class must implements:
  virtual bool getMinMaxCurStp(float minmax[2]) =0;

  // from vsnTimeSeriesMethodIF
  //   delivered class must implements:
  //   virtual bool updateStep();

  // from vsnMethodObj
  //   delivered class must implements:
  //   virtual std::deque<std::string> getDataTypes() const;
  virtual std::string getMethodType() const {
    return std::string("minmaxGlyph");
  }
  virtual void setShow(const bool mode);
  virtual void setBaseColor(const vector4 cv);
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  void updateShowGlyph();
  void updateColor();
  void updateLutRange();

  vfrTriangles*  m_minGlyph;
  vfrTriangles*  m_maxGlyph;

  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  float          m_tolerance;
  bool           m_showMin;
  bool           m_showMax;
};

#endif // _VSN_METHOD_MINMAX_GLYPH_H_
