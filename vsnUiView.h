//
// vsnUiView
//
#ifndef _VSN_UI_VIEW_H_
#define _VSN_UI_VIEW_H_

#include "wx/defs.h"
#include "wx/dcclient.h"
#include "wx/panel.h"
#include "wx/combobox.h"
#include "wx/treectrl.h"
#include "wx/checkbox.h"
#include "wx/splitter.h"

#include "vsnCMap.h"
#include "vsnMethodEditProp.h"
#include <deque>
#include <map>

namespace VSN {
  // TreeCtrl item types
  enum TreeItemType {TI_Scene =0, TI_ObjGrp, TI_Data, TI_Method};

  // control ids
  enum {UiView_SceneCombo =1200,
	UiView_TreeCtrl,
	UiView_DataName,
	UiView_DataReload,
	UiView_DataAutoReload,
	UiView_DataDelete,
	UiView_DataShow,
	UiView_DataXForm,
	UiView_DataMtdCombo,
	UiView_DataAddMtd,
	UiView_DataShowBbox,
	UiView_MethodName,
	UiView_MethodDelete,
	UiView_MethodShow,
	UiView_MethodLighting,
	UiView_MethodPropEdit,
	UiView_MethodCMapEdit,
	UiView_MethodCMapMin,
	UiView_MethodCMapMax,
	UiView_MethodParamArea,

	TreeCtrl_CtxMenu_Show,
	TreeCtrl_CtxMenu_Name
  };
};


//----------------------------------------------------------------
// class vsnTreeItem
//----------------------------------------------------------------
class vsnTreeItem : public wxTreeItemData {
public:
  vsnTreeItem(const VSN::TreeItemType tit, class vfrNode* prf =NULL)
    : m_type(tit), p_ref(prf) {}

  virtual ~vsnTreeItem() {}

  VSN::TreeItemType getType() const {return m_type;}
  class vfrNode* getRefNode() {return p_ref;}

protected:
  VSN::TreeItemType m_type;
  class vfrNode*    p_ref;
};

namespace VSN {
  // TreeItem map type
  typedef std::multimap<wxTreeItemId, vsnTreeItem*>
    vsnTreeItemMapType;
};


//----------------------------------------------------------------
// class vsnTreeCtrl
//----------------------------------------------------------------
class vsnTreeCtrl : public wxTreeCtrl {
public:
  vsnTreeCtrl() {}
  vsnTreeCtrl(wxWindow* parent, const wxWindowID id,
	      const wxPoint& pos, const wxSize& size,
	      long style =wxTR_DEFAULT_STYLE);
  virtual ~vsnTreeCtrl() {}

  // methods
  class vsnUiView* getUiView();
  bool updateScene(class vsnScene* psc, VSN::vsnTreeItemMapType& itmLst);

  // event handlers
  void OnSelChanged(wxTreeEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnContextMenu(wxContextMenuEvent& event);
  void OnCtxMenuShow(wxCommandEvent& event);
  void OnCtxMenuName(wxCommandEvent& event);

private:
  unsigned int m_selectedId;

  DECLARE_DYNAMIC_CLASS(vsnTreeCtrl)
  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnUiView
//----------------------------------------------------------------
class vsnUiView : public wxPanel {
public:
  vsnUiView(class vsnFrameBase* parent,
	    const wxPoint& pos =wxDefaultPosition,
	    const wxSize& size =wxSize(200,480));
  virtual ~vsnUiView();

  class vsnXFormDlg* getXFormDlg() {return m_pXFormDlg;}
  class vsnCMapDlg*  getCMapDlg() {return m_pCMapDlg;}

  void reset();
  bool updateScene(class vsnScene* psc);
  bool updateSceneList(const std::deque<vsnScene*>& scl);

  bool selectObj(vsnTreeItem* pti);
  bool selectObj(vfrNode* pnode);
  bool updateData(class vsnDataObj* pdo);
  bool updateMethod(class vsnMethodObj* pmo, const bool doMPP =true);

  bool enableDataArea(const bool mode =TRUE);
  bool enableMethodArea(const bool mode =TRUE);

  class vsnScene*     getCurrentScene();
  class vsnDataObj*   getCurrentData();
  class vsnMethodObj* getCurrentMethod();

protected:
  void OnSceneComboBox(wxCommandEvent& event);
  void OnDataReloadBtn(wxCommandEvent& event);
  void OnDataAutoReloadBtn(wxCommandEvent& event);
  void OnDataDeleteBtn(wxCommandEvent& event);
  void OnDataNameTxt(wxCommandEvent& event);
  void OnDataShowChk(wxCommandEvent& event);
  void OnDataXFormBtn(wxCommandEvent& event);
  void OnDataAddMethodBtn(wxCommandEvent& event);
  void OnDataShowBboxChk(wxCommandEvent& event);
  void OnMethodDeleteBtn(wxCommandEvent& event);
  void OnMethodNameTxt(wxCommandEvent& event);
  void OnMethodShowChk(wxCommandEvent& event);
  void OnMethodLightingChk(wxCommandEvent& event);
  void OnMethodPropEditBtn(wxCommandEvent& event);
  void OnMethodCMapEditBtn(wxCommandEvent& event);
  void OnMethodCMapMinTxt(wxCommandEvent& event);
  void OnMethodCMapMaxTxt(wxCommandEvent& event);

private:
  class vsnFrameBase*         p_parent;

  // dialogs
  class vsnXFormDlg*          m_pXFormDlg;
  class vsnCMapDlg*           m_pCMapDlg;
  class vsnMethodEditPropDlg* m_pMtdPropDlg;

  // splitter
  wxSplitterWindow*           m_splitter1;
  wxSplitterWindow*           m_splitter2;

  // scene widgets
  wxPanel*                    m_pPanelScene;
  wxComboBox*                 m_pSceneCombo;
  vsnTreeCtrl*                m_pTreeCtrl;

  // data widgets
  wxPanel*                    m_pPanelData;
  wxTextCtrl*                 m_pDataName;
  wxButton*                   m_pDataReload;
  wxButton*                   m_pDataAutoReload;
  wxButton*                   m_pDataDelete;
  wxCheckBox*                 m_pDataShow;
  wxButton*                   m_pDataXform;
  wxComboBox*                 m_pMethodCombo;
  wxButton*                   m_pAddMethodBtn;
  wxCheckBox*                 m_pDataShowBbox;

  // method widgets
  wxPanel*                    m_pPanelMethod;
  wxTextCtrl*                 m_pMethodName;
  wxButton*                   m_pMethodDelete;
  wxCheckBox*                 m_pMethodShow;
  wxCheckBox*                 m_pMethodLight;
  vsnCMapBar*                 m_pColorBar;
  wxButton*                   m_pPropEditBtn;
  vsnCMapBar*                 m_pCMapBar;
  wxButton*                   m_pCMapEditBtn;
  wxTextCtrl*                 m_pCMapMin;
  wxTextCtrl*                 m_pCMapMax;
  wxScrolledWindow*           m_pMethodParamArea;
  class vsnMethodPP*          p_mpp;

  // TreeItem list
  VSN::vsnTreeItemMapType     m_treeItemLst;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_UI_VIEW_H_
