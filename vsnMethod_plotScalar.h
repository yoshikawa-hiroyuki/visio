//
// vsnMethod_plotScalar
//
#ifndef _VSN_METHOD_PLOTSCALAR_H_
#define _VSN_METHOD_PLOTSCALAR_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnDataObj.h"
#include "vfrMesh2D.h"

namespace VSN {
  // control ids
  enum {MPP_plotScalar_SamplerLst = 2100,
	MPP_plotScalar_SamplerTxt,
	MPP_plotScalar_SetSamplerBtn,
	MPP_plotScalar_SelDataLst,
	MPP_plotScalar_VecDataChkLst,
        MPP_plotScalar_UpdMinMaxChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_plotScalar
//   method parameter-panel for vsnMethod_plotScalar
//----------------------------------------------------------------
class vsnMPP_plotScalar : public vsnMethodPP {
public: 
  vsnMPP_plotScalar(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_plotScalar();

  // interface
  virtual bool update();

  // event handler
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnEnterSamplerTxt(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);

private:
  wxListBox*      m_pSamplerLst;
  wxTextCtrl*     m_pSamplerTxt;
  wxButton*       m_pSetSamplerBtn;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_plotScalar
//----------------------------------------------------------------
class vsnMethod_plotScalar
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnRef_sampler {
public:
  vsnMethod_plotScalar(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_plotScalar();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  bool getUseSampler() const {return m_useSampler;}

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotScalar");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

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
  bool           m_updateMinMax;
  bool           m_useSampler;

  vfrMesh2D*     m_mesh;

  void adjustRange();
};

#endif // _VSN_METHOD_PLOTSCALAR_H_
