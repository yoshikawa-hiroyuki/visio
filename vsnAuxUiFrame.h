//
// vsnAuxUiFrame
//
#ifndef _VSN_AUX_UI_FRAME_H_
#define _VSN_AUX_UI_FRAME_H_

#include "wx/defs.h"
#include "wx/dcclient.h"

#include "vsnFrameBase.h"
#include "vsnUiView.h"


//----------------------------------------------------------------
// class vsnAuxUiFrame
//----------------------------------------------------------------
class vsnAuxUiFrame : public vsnFrameBase {
public:
  vsnAuxUiFrame(class vsnViewFrame* parent, const wxString& title,
		const wxPoint& pos =wxDefaultPosition,
		const wxSize& size =wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE);
  virtual ~vsnAuxUiFrame();

  void refresh(const bool reSelObj =false);
  vsnUiView* getUiView() {return m_pUiView;}
  bool updateSceneList(const std::deque<vsnScene*>& scl);
  void changeScene(class vsnScene* ps);
  class vsnViewFrame* getMasterViewFrame() {return p_master;}
  bool closeView();

  // from vsnFrameBase
  virtual void setScene(class vsnScene* ps);
  virtual bool adjustSelection(vsnTreeItem* pti);

private:
  class vsnViewFrame* p_master;
  vsnUiView*          m_pUiView; // Wx object, don't delete

  // event handlers
  void OnClose(wxCloseEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_AUX_UI_FRAME_H_
