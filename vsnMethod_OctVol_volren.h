//
// vsnMethod_OctVol_volren
//
#ifndef _VSN_METHOD_OCTVOL_VOLREN_H_
#define _VSN_METHOD_OCTVOL_VOLREN_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_OctVol.h"
#include "vsnRsmplOctVol.h"
#include "vsnNvrVolumeRender.h"

namespace VSN {
  // control ids
  enum {MPP_OctVol_volren_SelDataLst = 4700,
        MPP_OctVol_volren_VecDataChkLst,
        MPP_OctVol_volren_UpdMinMaxChk,
        MPP_OctVol_volren_InterpolateChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_OctVol_volren
//   method parameter-panel for vsnMethod_OctVol_volren
//----------------------------------------------------------------
class vsnMPP_OctVol_volren : public vsnMethodPP {
public: 
  vsnMPP_OctVol_volren(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_OctVol_volren();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnInterpolateChk(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxCheckBox*     m_pInterpolateChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_OctVol_volren
//----------------------------------------------------------------
class vsnMethod_OctVol_volren
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_OctVol_volren(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_volren();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setInterpolateMode(const bool mode);
  bool getInterpolateMode() const {return m_interpolate;}

  vsnNvrVolumeRender* getVolumeRender() {return m_pRender;}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
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
  bool                m_interpolate;

  vsnNvrVolumeRender* m_pRender;
  std::deque<unsigned char*>
                      m_pdl;
  std::deque<float>   m_minLst;

  vsnRsmplOctVol      m_rsmpl;
  long                m_rsmpl_updStamp;

  void adjustRange();
  bool doResample(vsnOctTree* poct, deque<vsnRsmplOctVol::RsmplBlk*>& bl);
};

#endif // _VSN_METHOD_OCTVOL_VOLREN_H_
