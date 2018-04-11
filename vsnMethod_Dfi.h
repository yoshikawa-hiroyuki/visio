//
// vsnMethod_Dfi
//
#ifndef _VSN_METHOD_DFI_H_
#define _VSN_METHOD_DFI_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Dfi.h"
#include "vsnMethod_info.h"


//----------------------------------------------------------------
// class vsnMethod_Dfi_info
//----------------------------------------------------------------
class vsnMethod_Dfi_info
  : public vsnMethod_info
{
public:
  vsnMethod_Dfi_info(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Dfi_info();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Dfi")); return r;
  }
  virtual bool update(const bool force =true);

protected:
  virtual bool updateRepStr();
};


//----------------------------------------------------------------
// class vsnMPP_Dfi_brickLoader
//   method parameter-panel for vsnMethod_Dfi_brickLoader
//----------------------------------------------------------------
namespace VSN {
  // control ids
  enum {MPP_Dfi_brickLoader_LoadBtn =7100,
	MPP_Dfi_brickLoader_RegionResetBtn,
        MPP_Dfi_brickLoader_RegionX1Txt,
        MPP_Dfi_brickLoader_RegionX2Txt,
        MPP_Dfi_brickLoader_RegionY1Txt,
        MPP_Dfi_brickLoader_RegionY2Txt,
        MPP_Dfi_brickLoader_RegionZ1Txt,
        MPP_Dfi_brickLoader_RegionZ2Txt
  };
};

class vsnMPP_Dfi_brickLoader : public vsnMethodPP {
public:
  vsnMPP_Dfi_brickLoader(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Dfi_brickLoader();

  // interface
  virtual bool update();

  // event handler
  void OnRegionResetBtn(wxCommandEvent& event);
  void OnRegionTxt(wxCommandEvent& event);
  void OnLoadBtn(wxCommandEvent& event);

private:
  wxButton*       m_pLoadBtn;
  wxButton*       m_pRegionResetBtn;
  wxTextCtrl*     m_pRegionX1Txt;
  wxTextCtrl*     m_pRegionX2Txt;
  wxTextCtrl*     m_pRegionY1Txt;
  wxTextCtrl*     m_pRegionY2Txt;
  wxTextCtrl*     m_pRegionZ1Txt;
  wxTextCtrl*     m_pRegionZ2Txt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Dfi_brickLoader
//----------------------------------------------------------------
class vsnMethod_Dfi_brickLoader
  : public vsnMethodObj
{
public:
  vsnMethod_Dfi_brickLoader(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Dfi_brickLoader();

  bool setRegion(const CES::Vec3<size_t>* preg);
  bool getRegion(CES::Vec3<size_t>* preg) const;
  bool loadRegion(const std::string& dname =std::string());
  bool addloadedDataRef(class vsnData_DfiSv* pd);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Dfi")); return r;
  }
  virtual std::string getMethodType() const {return std::string("brickLoader");}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual bool canLighting() const {return false;}
  virtual void setRefData(vsnDataObj* prd);
  virtual void setBaseColor(const vector4 cv);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  CES::Vec3<size_t> m_regionIdx[2];
  vfrGroup* m_loadedDataGrp;
  volatile mutable size_t m_loadCnt;
};

#endif // _VSN_METHOD_DFI_H_
