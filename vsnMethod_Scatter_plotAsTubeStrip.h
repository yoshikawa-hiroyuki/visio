//
// vsnMethod_Scatter_plotAsTubeStrip
//
#ifndef _VSN_METHOD_SCATTER_PLOTASTUBESTRIP_H_
#define _VSN_METHOD_SCATTER_PLOTASTUBESTRIP_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Scatter.h"

namespace VSN {
  // control ids
  enum {MPP_Scatter_plotAsTubeStrip_RadiusBiasTxt = 3800,
	MPP_Scatter_plotAsTubeStrip_SelColDataLst,
	MPP_Scatter_plotAsTubeStrip_SelRadDataLst,
	MPP_Scatter_plotAsTubeStrip_VecDataChkLst,
	MPP_Scatter_plotAsTubeStrip_UpdMinMaxChk,
	MPP_Scatter_plotAsTubeStrip_SplitWithNVChk,
  };
};


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotAsTubeStrip
//   method parameter-panel for vsnMethod_Scatter_plotAsTubeStrip
//----------------------------------------------------------------
class vsnMPP_Scatter_plotAsTubeStrip : public vsnMethodPP {
public:
  vsnMPP_Scatter_plotAsTubeStrip(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Scatter_plotAsTubeStrip();

  // interface
  virtual bool update();

  // event handler
  void OnRadiusBiasTxt(wxCommandEvent& event);
  void OnSelColDataLst(wxCommandEvent& event);
  void OnSelRadDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnSplitWithNVChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pRadiusBiasTxt;
  wxComboBox*     m_pSelColDataLst;
  wxComboBox*     m_pSelRadDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxCheckBox*     m_pSplitWithNVChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotAsTubeStrip
//----------------------------------------------------------------
class vsnMethod_Scatter_plotAsTubeStrip
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Scatter_plotAsTubeStrip(const std::string& nm
				    = std::string(VFR_NONAME));
  virtual ~vsnMethod_Scatter_plotAsTubeStrip();

  // methods
  float getRadiusBias() const {return m_radiusBias;}
  bool setRadiusBias(const float rb);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  VSN::WhichDataType getSelectedColData() const {return m_selectedColData;}
  bool setSelectedColData(const VSN::WhichDataType sd);

  VSN::WhichDataType getSelectedRadData() const {return m_selectedRadData;}
  bool setSelectedRadData(const VSN::WhichDataType sd);
  
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  bool getSplitWithNV() const {return m_splitWithNV;}
  bool setSplitWithNV(const bool split);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Scatter")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotAsTubeStrip");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  float              m_radiusBias;
  VSN::WhichDataType m_selectedColData;
  VSN::WhichDataType m_selectedRadData;
  CES::Vec3<int>     m_vecDataIdx;
  bool               m_updateMinMax;
  bool               m_splitWithNV;
  vfrGroup*          m_shape;

  void adjustRange();
};

#endif // _VSN_METHOD_SCATTER_PLOTASLINESTRIP_H_
