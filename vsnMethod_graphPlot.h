//
// vsnMethod_graphPlot
//
#ifndef _VSN_METHOD_GRAPHPLOT_H_
#define _VSN_METHOD_GRAPHPLOT_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnDataObj.h"
#include "vsnGnuplotIF.h"
#include "vsnMethod_sampler.h"

namespace VSN {
  // control ids
  enum {MPP_graphPlot_ProgramTxt = 2000,
	MPP_graphPlot_ProgBrwsBtn,
	MPP_graphPlot_PreCommandTxt,
	MPP_graphPlot_SamplerLst,
	MPP_graphPlot_SamplerTxt,
	MPP_graphPlot_SetSamplerBtn,
	MPP_graphPlot_SelDataLst,
	MPP_graphPlot_VecDataChkLst,
	MPP_graphPlot_ShowTitleChk,
	MPP_graphPlot_ShowLegendChk,
	MPP_graphPlot_ExportBtn,
        MPP_graphPlot_AutoExportChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_graphPlot
//   method parameter-panel for vsnMethod_graphPlot
//----------------------------------------------------------------
class vsnMPP_graphPlot : public vsnMethodPP {
public: 
  vsnMPP_graphPlot(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_graphPlot();

  // interface
  virtual bool update();

  // event handler
  void OnEnterProgramTxt(wxCommandEvent& event);
  void OnProgBrwsBtn(wxCommandEvent& event);
  void OnEnterSamplerTxt(wxCommandEvent& event);
  void OnEnterPreCommandTxt(wxCommandEvent& event);
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnShowTitleChk(wxCommandEvent& event);
  void OnShowLegendChk(wxCommandEvent& event);
  void OnExportBtn(wxCommandEvent& event);
  void OnAutoExportChk(wxCommandEvent& event);
  
private:
  wxTextCtrl*     m_pProgramTxt;
  wxButton*       m_pProgBrwsBtn;
  wxTextCtrl*     m_pPreCommandTxt;
  wxListBox*      m_pSamplerLst;
  wxTextCtrl*     m_pSamplerTxt;
  wxButton*       m_pSetSamplerBtn;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pShowTitleChk;
  wxCheckBox*     m_pShowLegendChk;
  wxButton*       m_pExportBtn;
  wxCheckBox*     m_pAutoExportChk;
  
  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_graphPlot
//----------------------------------------------------------------
class vsnMethod_graphPlot
  : public vsnMethodObj,
    public vsnRef_sampler {
public:
  vsnMethod_graphPlot(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_graphPlot();

  std::string getProgramName() const {return m_progName;}
  bool setProgramName(const std::string& progName);

  std::string getPreCommand() const {return m_preCommand;}
  bool setPreCommand(const std::string& preCmd);

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setShowTitle(const bool stm);
  bool getShowTitle() const {return m_showTitle;}

  bool setShowLegend(const bool slm);
  bool getShowLegend() const {return m_showLegend;}

  // export interface
  virtual bool exportCsv(const std::string& path) {return false;}
  std::string getExportedPath() const {return m_exportPath;}
  bool setAutoExport(const bool aem);
  bool getAutoExport() const {return m_autoExport;}
  
  // from vsnMethodObj
  //   need to implement getDataTypes() in derived class
  //   need to implement update() in derived class
  virtual std::string getMethodType() const {
    return std::string("graphPlot");
  }
  virtual bool canLighting() const {return false;}
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnRef_sampler
  virtual void noticeUpdate();

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  vsnGnuplotIF*  m_pGnuPlotIF;

  std::string    m_progName;
  std::string    m_preCommand;
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  bool           m_showTitle;
  bool           m_showLegend;
  std::string    m_tmpPath;

  std::string    m_exportPath;
  bool           m_autoExport;
  
  bool DoPlot(const Point2& sampleSize,
	      const vector3* samplePos, const float* values,
	      const float range[2], const std::string& titleStr) const;
  bool ExportCsv(const std::string& path, const Point2& sampleSize,
		 const vector3* samplePos, const float* values);
};

#endif // _VSN_METHOD_GRAPHPLOT_H_
