//
// vsnMethod_Shape_trias_slbExt
//
#ifndef _VSN_METHOD_SHAPE_TRIAS_SLBEXT_H_
#define _VSN_METHOD_SHAPE_TRIAS_SLBEXT_H_

#include "wx/defs.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vfrTriangles.h"

namespace VSN {
  // control ids
  enum {MPP_Shape_trias_slbExt_ColorChk = 3200,
	MPP_Shape_trias_slbExt_ScalarChk,
	MPP_Shape_trias_slbExt_ShrinkFac,
	MPP_Shape_trias_slbExt_SmoothChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Shape_trias_slbExt
//   method parameter-panel for vsnMethod_Shape_trias_slbExt
//----------------------------------------------------------------
class vsnMPP_Shape_trias_slbExt : public vsnMethodPP {
public: 
  vsnMPP_Shape_trias_slbExt(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Shape_trias_slbExt();

  // interface
  virtual bool update();

  // event handler
  void OnShowChk(wxCommandEvent& event);
  void OnShrinkFacTxt(wxCommandEvent& event);
  void OnSmoothChk(wxCommandEvent& event);

private:
  wxCheckBox* m_pColorChk;
  wxCheckBox* m_pScalarChk;
  wxTextCtrl* m_pShrinkFacTxt;
  wxCheckBox* m_pSmoothChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Shape_trias_slbExt
//----------------------------------------------------------------
class vsnMethod_Shape_trias_slbExt
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  enum {Colored =0x1, ScalarData =(0x1<<1)};
  typedef int StlExtType;

  vsnMethod_Shape_trias_slbExt(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Shape_trias_slbExt();

  StlExtType getShowObjs() const {return m_showObjs;}
  bool setShowObjs(const StlExtType et);
  float getShrinkFac() const {return m_shrinkFac;}
  bool setShrinkFac(const float sf);
  bool getSmoothMode() const {return m_smooth;}
  bool setSmoothMode(const bool sm);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Shape")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("trias_slbExt");
  }
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  StlExtType    m_showObjs;
  float         m_shrinkFac;
  bool          m_smooth;

  vfrTriangles* m_trias;
};

#endif // _VSN_METHOD_SHAPE_TRIAS_SLBEXT_H_
