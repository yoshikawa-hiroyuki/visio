//
// vsnMethodParamCanvas
//
#ifndef _VSN_METHOD_PARAM_CANVAS_H_
#define _VSN_METHOD_PARAM_CANVAS_H_

#include "wx/defs.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/dialog.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"

#include "vsnFrameBase.h"
#include "vsnMethodObj.h"
#include <deque>

namespace VSN {
  // control ids
  enum {MethodParamCanvas_NameTxt = 1350,
	MethodParamCanvas_AddMethodBtn,
	MethodParamCanvas_DelMethodBtn,

	// for MPC List Dialog
	MPCListDlg_MPCListCtl,
	MPCListDlg_NewBtn,
	MPCListDlg_DeleteBtn,
	MPCListDlg_CloseBtn
  };
};


//----------------------------------------------------------------
// class vsnMethodParamCanvas
//----------------------------------------------------------------
class vsnMethodParamCanvas : public wxFrame, public vsnIoObject {
public:
  vsnMethodParamCanvas(wxWindow* parent, const wxString& title,
		       const wxPoint& pos =wxDefaultPosition,
		       const wxSize& size =wxSize(-1,600),
		       long style = wxDEFAULT_FRAME_STYLE,
		       const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnMethodParamCanvas();

  size_t getSeqNo() const {return m_seq;}
  void setName(const std::string& name);
  std::string getName() const {return m_name;}

  void updateLayout();
  bool addMethod(vsnMethodObj* pMtd);
  bool delMethod(vsnMethodObj* pMtd);

  bool setShowMode(const bool sm =true);
  bool setGeometry(const int x, const int y, const int w, const int h);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  static size_t     s_mpcSeq;

  size_t            m_seq;
  std::string       m_name;
  std::deque<vsnMethodObj*>
                    m_mtdLst;

  wxTextCtrl*       m_pNameTxt;
#ifndef LINUX
  wxPanel*          m_pRootPanel;
#else
  wxScrolledWindow* m_pRootPanel;
#endif

  // event handlers
  void OnNameTxt(wxCommandEvent& event);
  void OnAddMethodBtn(wxCommandEvent& event);
  void OnDelMethodBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMPCListCtl
//----------------------------------------------------------------
class vsnMPCListCtl : public wxListCtrl {
public:
  vsnMPCListCtl(wxWindow *parent, const wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style)
    : wxListCtrl(parent, id, pos, size, style) {
  }

private:
  //DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMPCListDlg
//----------------------------------------------------------------
class vsnMPCListDlg : public wxDialog {
public:
  vsnMPCListDlg(wxWindow* parent, std::deque<vsnMethodParamCanvas*>* plst);

  void setRefList(std::deque<vsnMethodParamCanvas*>* plst) {
    p_refLst = plst; updateList();
  }
  std::deque<vsnMethodParamCanvas*>* getRefList() {return p_refLst;}

  // event handler
  void OnItemSelected(wxListEvent& event);
  void OnNewBtn(wxCommandEvent& event);
  void OnDeleteBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);

private:
  std::deque<vsnMethodParamCanvas*>* p_refLst;

  vsnMPCListCtl* m_pMPCListCtl;
  wxButton*      m_pNewBtn;
  wxButton*      m_pDeleteBtn;
  wxButton*      m_pCloseBtn;
  wxImageList*   m_pImgLst;

  bool updateList();

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_METHOD_PARAM_CANVAS_H_
