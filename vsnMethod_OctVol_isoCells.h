//
// vsnMethod_OctVol_isoCells
//
#ifndef _VSN_METHOD_OCTVOL_ISOCELLS_H_
#define _VSN_METHOD_OCTVOL_ISOCELLS_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_OctVol.h"
#include "vfrTriangles.h"

namespace VSN {
  // control ids
  enum {MPP_OctVol_isoCells_SelDataLst = 4300,
        MPP_OctVol_isoCells_VecDataChkLst,
        MPP_OctVol_isoCells_ValueSld,
        MPP_OctVol_isoCells_ValueTxt,
        MPP_OctVol_isoCells_UseCMapChk,
        MPP_OctVol_isoCells_UseCMapAlphaChk,
        MPP_OctVol_isoCells_UpdMinMaxChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_OctVol_isoCells
//   method parameter-panel for vsnMethod_OctVol_isoCells
//----------------------------------------------------------------
class vsnMPP_OctVol_isoCells : public vsnMethodPP {
public: 
  vsnMPP_OctVol_isoCells(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_OctVol_isoCells();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnValueTxt(wxCommandEvent& event);
  void OnUseCMapChk(wxCommandEvent& event);
  void OnUseCMapAlphaChk(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pValueTxt;
  wxCheckBox*     m_pUseCMapChk;
  wxCheckBox*     m_pUseCMapAlphaChk;
  wxCheckBox*     m_pUpdMinMaxChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_OctVol_isoCells
//----------------------------------------------------------------
class vsnMethod_OctVol_isoCells
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_OctVol_isoCells(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_isoCells();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setIsoValue(const float val);
  float getIsoValue() const {return m_isoValue;}

  bool setUseCMap(const bool ucm);
  bool getUseCMap() const {return m_useCMap;}

  bool setUseCMapAlpha(const bool ucm);
  bool getUseCMapAlpha() const {return m_useCMapAlpha;}

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("isoCells");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // override vfrGroup method
  virtual void generateBbox();
  virtual void generateBbox(CES::Vec3<float>&) {generateBbox();}

private:
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  float          m_isoValue;
  bool           m_useCMap;
  bool           m_useCMapAlpha;
  bool           m_updateMinMax;

  vfrTriangles*  m_isoTrias;
  float*         m_pd;

  void adjustRange(const bool updval =true);
  void updateColor();

  struct FaceWall {
    enum WallType {FaceNone =0,
		   FaceMX =(1<<0), FaceMY =(1<<1), FaceMZ =(1<<2),
		   FacePX =(1<<3), FacePY =(1<<4), FacePZ =(1<<5)};
    vsnOctTree::Node* p_node;
    WallType m_wall;
    FaceWall(vsnOctTree::Node* pn, const WallType w =FaceNone)
      : p_node(pn), m_wall(w) {}
  };
};

#endif // _VSN_METHOD_OCTVOL_ISOCELLS_H_
