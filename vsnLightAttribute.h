//
// vsnLightAttribute
//
#ifndef _VSN_LIGHTATTRIBUTE_H_
#define _VSN_LIGHTATTRIBUTE_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/radiobox.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/statline.h"
#include "wx/stattext.h"

#include "vsnIoObject.h"
#include "utilMath.h"
#include "vfrDefs.h"

namespace VSN {
  enum {LightAttDlg_Close =1550,
	LightAttDlg_Type,
	LightAttDlg_X,
	LightAttDlg_Y,
	LightAttDlg_Z,
	LightAttDlg_ColorBtn,
	LightAttDlg_ResetBtn,
	LightAttDlg_CloseBtn
  };
};

//----------------------------------------------------------------
// class vsnLightAttribute
//----------------------------------------------------------------
class vsnLightAttribute
  : public vsnIoObject
{
public:
  enum LightType {Light_BiDirection =1, Light_Direction, Light_Point};

  vsnLightAttribute(class vsnScene* psc);
  virtual ~vsnLightAttribute();

  void setRefScene(class vsnScene* psc);
  class vsnScene* getRefScene() {return p_scene;}

  bool applyLight() const;
  void resetLight();
  bool isStandardLight() const;

  LightType getLightType() const {return m_lightType;}
  void setLightType(const LightType lightType);
  CES::Vec3<float> getDirection() const {return m_direction;}
  void setDirection(const CES::Vec3<float> xyz);
  CES::Vec3<float> getPosition() const {return m_position;}
  void setPosition(const CES::Vec3<float> xyz);
  void getColor(VFR::vector4 color) const {
    color[0] = m_color[0]; color[1] = m_color[1];
    color[2] = m_color[2]; color[3] = m_color[3];
  }
  void setColor(const VFR::vector4 color);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  // command implemented in vsnScene::commandXML

protected:
  class vsnScene*   p_scene;

  LightType         m_lightType;
  CES::Vec3<float>  m_direction;
  CES::Vec3<float>  m_position;
  VFR::vector4      m_color;
};



//----------------------------------------------------------------
// class vsnLightAttributeDlg
//----------------------------------------------------------------
class vsnLightAttributeDlg : public wxDialog {
public:
  vsnLightAttributeDlg(wxWindow *parent, vsnLightAttribute* refLightAtt);
  ~vsnLightAttributeDlg();

  /* interface */
  bool update(); // copy values from p_lightAtt to this

  vsnLightAttribute* getRefLightAtt() {return p_lightAtt;}
  void setRefLightAtt(vsnLightAttribute* refLightAtt);

  /* event handler */
  void OnChangeRadio(wxCommandEvent& event);
  void OnChangeValues(wxCommandEvent& event);
  void OnColorBtn(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxRadioBox   *m_pTypeRadio;
  wxStaticText *m_pXLbl, *m_pYLbl, *m_pZLbl;
  wxTextCtrl   *m_pXTxt, *m_pYTxt, *m_pZTxt;
  wxButton     *m_pColorBtn;
  wxButton     *m_pResetBtn;
  wxButton     *m_pCloseBtn;

  vsnLightAttribute* p_lightAtt;
  VFR::vector4       m_colour;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_LIGHTATTRIBUTE_H_
