//
// vsnMethod_Sv_volren
//
#ifndef _VSN_METHOD_SV_VOLREN_H_
#define _VSN_METHOD_SV_VOLREN_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"
#include "vsnNvrVolumeRender.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_volren_SelDataLst = 5900,
        MPP_Sv_volren_VecDataChkLst,
        MPP_Sv_volren_UpdMinMaxChk,
	MPP_Sv_volren_ReduceChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_volren
//   method parameter-panel for vsnMethod_Sv_volren
//----------------------------------------------------------------
class vsnMPP_Sv_volren : public vsnMethodPP {
public: 
  vsnMPP_Sv_volren(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_volren();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnReduceChk(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxCheckBox*     m_pReduceChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_volren
//----------------------------------------------------------------
class vsnMethod_Sv_volren
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Sv_volren(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_volren();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}

  bool setReduceMode(const bool mode);
  bool getReduceMode() const {return m_doReduce;}

  vsnNvrVolumeRender* getVolumeRender() {return m_pRender;}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("volren");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);
  virtual void setRefData(vsnDataObj* prd);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  VSN::WhichDataType  m_selectedData;
  CES::Vec3<int>      m_vecDataIdx;
  bool                m_updateMinMax;
  bool                m_doReduce;

  vsnNvrVolumeRender* m_pRender;
  mutable bool        m_inited;
  mutable bool        m_crdWarned;
  float*              m_pd;

  void adjustRange();
};

#endif // _VSN_METHOD_SV_VOLREN_H_
