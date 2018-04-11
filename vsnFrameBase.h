//
// vsnFrameBase
//
#ifndef _VSN_FRAME_BASE_H_
#define _VSN_FRAME_BASE_H_

#include "wx/defs.h"
#include "wx/dcclient.h"
#include "wx/frame.h"
#include "wx/dialog.h"
#include <string>
#include <set>


//----------------------------------------------------------------
// class vsnFrameRefer
//----------------------------------------------------------------
class vsnFrameRefer : public wxDialog {
public:
  vsnFrameRefer(class vsnFrameBase* parent,
		wxWindowID id, const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE);
  virtual ~vsnFrameRefer();
};


//----------------------------------------------------------------
// class vsnFrameBase
//----------------------------------------------------------------
class vsnFrameBase : public wxFrame {
public:  
  vsnFrameBase(wxWindow* parent, const wxString& title,
	       const wxPoint& pos =wxDefaultPosition,
	       const wxSize& size =wxDefaultSize,
	       long style = wxDEFAULT_FRAME_STYLE)
    : wxFrame(parent, -1, title, pos, size, style),
      p_app(NULL), p_scene(NULL) {
  }
  virtual ~vsnFrameBase();

  virtual void setApp(class vsnApp* pp) {p_app = pp;}
  class vsnApp* getApp() {return p_app;}

  virtual void setScene(class vsnScene* ps) {p_scene = ps;}
  class vsnScene* getScene() {return p_scene;}

  virtual bool adjustSelection(class vsnTreeItem* pti) {return false;}
  virtual bool setStatusText(const std::string& msg) {return true;}

  virtual bool addRefer(vsnFrameRefer* pref);
  virtual bool delRefer(vsnFrameRefer* pref);

protected:
  class vsnApp*    p_app;
  class vsnScene*  p_scene;

  std::set<vsnFrameRefer*> m_referLst;
};

#endif // _VSN_FRAME_BASE_H_
