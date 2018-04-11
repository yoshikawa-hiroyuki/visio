//
// vsnMethod_Sv_setMask
//
#ifndef _VSN_METHOD_SV_SETMASK_H_
#define _VSN_METHOD_SV_SETMASK_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/radiobox.h"

#include "vsnMethodObj.h"
#include "vsnData_Sv.h"
#include "vsnGridUtilSv.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_setMask_FileTxt =5600,
	MPP_Sv_setMask_BrowsBtn,
	MPP_Sv_setMask_TypeRadio,
	MPP_Sv_setMask_VolRateMinTxt,
	MPP_Sv_setMask_VolRateMaxTxt,
	MPP_Sv_setMask_MediumTxt,
	MPP_Sv_setMask_SetMaskBtn,
	//MPP_Sv_setMask_UpdateMinMaxChk,
	MPP_Sv_setMask_ShowMaskChk,
	MPP_Sv_setMask_OffsetXTxt,
	MPP_Sv_setMask_OffsetYTxt,
	MPP_Sv_setMask_OffsetZTxt
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_setMask
//   method parameter-panel for vsnMethod_Sv_setMask
//----------------------------------------------------------------
class vsnMPP_Sv_setMask : public vsnMethodPP {
public:
  vsnMPP_Sv_setMask(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_setMask();

  // interface
  virtual bool update();

  // event handler
  void OnBrowsBtn(wxCommandEvent& event);
  void OnSetMaskBtn(wxCommandEvent& event);
  void OnOffsetTxts(wxCommandEvent& event);
  void OnShowMaskChk(wxCommandEvent& event);

private:
  wxTextCtrl* m_pFileTxt;
  wxButton*   m_pBrowsBtn;
  wxRadioBox* m_pTypeRadio;
  wxTextCtrl* m_pVolRateMinTxt;
  wxTextCtrl* m_pVolRateMaxTxt;
  wxTextCtrl* m_pMediumTxt;
  wxButton*   m_pSetMaskBtn;
  //wxCheckBox* m_pUpdateMinMaxChk;
  wxCheckBox* m_pShowMaskChk;
  wxTextCtrl* m_pOffsetXTxt;
  wxTextCtrl* m_pOffsetYTxt;
  wxTextCtrl* m_pOffsetZTxt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnSvMaskArea
//----------------------------------------------------------------
class vsnSvMaskArea : public vfrNode {
public:
  vsnSvMaskArea(vsnData_Sv* pdata,
		const std::string& name =std::string(VFR_NONAME),
		const Bool ssm =FALSE);
  virtual ~vsnSvMaskArea();

  // interface
  bool setData(vsnData_Sv* pdata);
  bool update();
  void setShowMode(const bool sm);
  bool getShowMode() const;

  // override vfrNode method
  virtual void generateBbox();
  virtual void generateBbox(CES::Vec3<float>&) {generateBbox();}
  virtual void renderSolid();
  virtual void renderWire();

protected:
  vsnData_Sv* p_data;
  vsnGridUtilSv _gus;
};


//----------------------------------------------------------------
// class vsnMethod_Sv_setMask
//----------------------------------------------------------------
class vsnMethod_Sv_setMask
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  enum FileType {FILE_None =0, FILE_SVX, FILE_SBX};

  vsnMethod_Sv_setMask(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_setMask();

  bool setMask(const std::string& mskPath, const vsnData_Sv::MaskType mskType,
	       const float* vrRange, const int medId, const bool updMinMax,
	       const int* ofst);

  std::string getMaskPath() const {return m_maskFile;}
  vsnData_Sv::MaskType getMaskType() const {return m_maskType;}
  void getMaskVolRateRange(float vrr[2]) const {
    vrr[0] = m_volRateRange[0]; vrr[1] = m_volRateRange[1];
  }
  int getMaskMediumID() const {return m_mediumId;}
  bool getUpdateMinMaxMode() const {return m_updMinMax;}
  void getOffset(int* ofst) const;
  bool getMaskShowMode() const {return m_showMask;}
  bool setMaskShowMode(const bool smm);
  FileType getFileType() const {return m_fileType;}
  void setFileType(const int ftype);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("setMask");
  }
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // static
  static FileType getSuffixType(const std::string& path);

private:
  std::string          m_maskFile;
  FileType             m_fileType;
  vsnData_Sv::MaskType m_maskType;
  float                m_volRateRange[2];
  int                  m_mediumId;
  bool                 m_updMinMax;
  bool                 m_showMask;
  int                  m_offset[3];
  vsnSvMaskArea*       m_mskArea;
};

#endif // _VSN_METHOD_SV_SETMASK_H_
