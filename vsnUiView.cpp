//
// vsnUiView
//
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/statline.h"
#include "wx/scrolwin.h"

#include "vsnUiView.h"
#include "vsnFrameBase.h"
#include "vsnAuxUiFrame.h"
#include "vsnApp.h"
#include "vsnColorBar.h"
#include "vsnDataReload.h"
#include "vsnTextEntryDlg.h"
#include "vsnError.h"

using namespace std;
using namespace VSN;


//----------------------------------------------------------------
// class vsnTreeCtrl
//----------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(vsnTreeCtrl, wxTreeCtrl)

BEGIN_EVENT_TABLE(vsnTreeCtrl, wxTreeCtrl)
  EVT_TREE_SEL_CHANGED(UiView_TreeCtrl, vsnTreeCtrl::OnSelChanged)
  EVT_SIZE(vsnTreeCtrl::OnSize)
  EVT_CONTEXT_MENU(vsnTreeCtrl::OnContextMenu)
  EVT_MENU(TreeCtrl_CtxMenu_Show, vsnTreeCtrl::OnCtxMenuShow)
  EVT_MENU(TreeCtrl_CtxMenu_Name, vsnTreeCtrl::OnCtxMenuName)
END_EVENT_TABLE()


vsnTreeCtrl::vsnTreeCtrl(wxWindow* parent, const wxWindowID id,
			 const wxPoint& pos, const wxSize& size, long style)
  : wxTreeCtrl(parent, id, pos, size, style), m_selectedId(0)
{
  wxTreeItemId rootId
    = AddRoot(wxT("Root"), -1, -1, new vsnTreeItem(TI_Scene));
  //SetItemFont(rootId, *wxITALIC_FONT);
}

/* methods */

vsnUiView* vsnTreeCtrl::getUiView() {
  vsnUiView* puiv = NULL;
  wxWindow* pp = GetParent();
  while ( pp ) {
    puiv = dynamic_cast<vsnUiView*>(pp);
    if ( puiv ) break;
    pp = pp->GetParent();
  } // end of while(pp)
  return puiv;
}


bool vsnTreeCtrl::updateScene(vsnScene* psc, VSN::vsnTreeItemMapType& itmLst) {
  // clean
  wxTreeItemId rootId = GetRootItem();
  DeleteChildren(rootId);
  itmLst.clear();

  if ( ! psc ) return false;

  // create tree
  register size_t i, j, k;
  vsnTreeItem* pItem;
  size_t nmtd, ndt, nog = psc->getNumObjGroup();
  for ( i = 0; i < nog; i++ ) {
    vsnObjGroup* pog = psc->getObjGroup(i);
    if ( ! pog ) continue;
    pItem = new vsnTreeItem(TI_ObjGrp, pog);
    wxTreeItemId gid
      = AppendItem(rootId, vsnApp::ConvSysToWx(pog->getName()), -1,-1, pItem);
    itmLst.insert(make_pair(gid, pItem));

    ndt = pog->getNumData();
    for ( j = 0; j < ndt; j++ ) {
      vsnDataObj* pdt = pog->getData(j);
      if ( ! pdt ) continue;
      wxString dtName = vsnApp::ConvSysToWx(pdt->getName());
      dtName += wxT("[") + vsnApp::ConvSysToWx(pdt->getDataType()) + wxT("]");
      pItem = new vsnTreeItem(TI_Data, pdt);
      wxTreeItemId did = AppendItem(gid, dtName, -1,-1, pItem);
      itmLst.insert(make_pair(did, pItem));

      nmtd = pdt->getNumMethod();
      for ( k = 0; k < nmtd; k++ ) {
	vsnMethodObj* pmtd = pdt->getMethod(k);
	if ( ! pmtd ) continue;
	wxString mtdName = vsnApp::ConvSysToWx(pmtd->getName());
	mtdName +=
	  wxT("[") + vsnApp::ConvSysToWx(pmtd->getMethodType()) + wxT("]");
	pItem = new vsnTreeItem(TI_Method, pmtd);
	wxTreeItemId mid = AppendItem(did, mtdName, -1,-1, pItem);
	itmLst.insert(make_pair(mid, pItem));
      } // end of for(k)
    } // end of for(j)
  } // end of for(i)

  SelectItem(GetRootItem());
  Refresh(); // need on MacOSX
  return true;
}

/* event handler */

void vsnTreeCtrl::OnSelChanged(wxTreeEvent& event) {
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  vsnTreeItem* pitem = dynamic_cast<vsnTreeItem*>(GetItemData(GetSelection()));
  if ( ! pitem ) return;
  unsigned int newSelId = 0;
  vfrNode* newSelNode = pitem->getRefNode();
  if ( newSelNode ) newSelId = newSelNode->getID();
  if ( m_selectedId == newSelId ) return;

  puiv->selectObj(pitem);
  m_selectedId = newSelId;

  vsnFrameBase* pfb = dynamic_cast<vsnFrameBase*>(puiv->GetParent());
  if ( pfb ) pfb->adjustSelection(pitem);
}

void vsnTreeCtrl::OnSize(wxSizeEvent& event) {
  event.Skip();
  Refresh();
}

void vsnTreeCtrl::OnContextMenu(wxContextMenuEvent& event) {
  wxPoint pt = event.GetPosition();
  wxTreeItemId item;
  if ( ! HasFlag(wxTR_MULTIPLE) )
    item = GetSelection();

  if ( pt.x == -1 && pt.y == -1 ) {
    if ( item.IsOk() ) {
      wxRect rect;
      GetBoundingRect(item, rect, true /* only the label */);
      pt = wxPoint(rect.GetRight(), rect.GetTop());
    } else {
      pt = wxPoint(0, 0);
    }
  }
  else {
    pt = ScreenToClient(pt);
  }

  bool hasObj = false;
  wxArrayTreeItemIds selArr;
  size_t i, cnt = GetSelections(selArr);
  for ( i = 0; i < cnt; i++ ) {
    vsnTreeItem* xitem
      = dynamic_cast<vsnTreeItem*>(GetItemData(selArr.Item(i)));
    if ( ! xitem ) continue;
    VSN::TreeItemType xtype = xitem->getType();
    if ( xtype == TI_Data || xtype == TI_Method )
      hasObj = true;
  } // end of for(i)
  if ( ! hasObj ) return;

  wxMenu menu(wxT("Menu for selected"));
  if ( hasObj ) {
    menu.Append(TreeCtrl_CtxMenu_Show, wxT("toggle show/hide"));
    menu.Append(TreeCtrl_CtxMenu_Name, wxT("change name"));
  }
  PopupMenu(&menu, pt);
}

void vsnTreeCtrl::OnCtxMenuShow(wxCommandEvent& event) {
  wxArrayTreeItemIds selArr;
  size_t i, cnt = GetSelections(selArr);
  size_t xcnt = 0;
  for ( i = 0; i < cnt; i++ ) {
    vsnTreeItem* xitem
      = dynamic_cast<vsnTreeItem*>(GetItemData(selArr.Item(i)));
    if ( ! xitem ) continue;
    VSN::TreeItemType xtype = xitem->getType();
    vsnDataObj* pDt = dynamic_cast<vsnDataObj*>(xitem->getRefNode());
    vsnMethodObj* pMtd = dynamic_cast<vsnMethodObj*>(xitem->getRefNode());
    if ( pDt ) {
      pDt->setShow(! pDt->getShow());
      xcnt ++;
    } else if ( pMtd ) {
      pMtd->setShow(! pMtd->getShow());
      xcnt ++;
    }
  } // end of for(i)

  if ( xcnt > 0 )
    vsnApp::GetApp()->update(true);
}

void vsnTreeCtrl::OnCtxMenuName(wxCommandEvent& event) {
  wxArrayTreeItemIds selArr;
  size_t cnt = GetSelections(selArr);
  if ( cnt < 1 ) return;

  vsnTreeItem* xitem
    = dynamic_cast<vsnTreeItem*>(GetItemData(selArr.Item(0)));
  if ( ! xitem ) return;
  VSN::TreeItemType xtype = xitem->getType();
  if ( xtype != VSN::TI_Data && xtype != VSN::TI_Method ) return;
  vfrNode* pRef = xitem->getRefNode();
  if ( ! pRef ) return;

  string oldName = pRef->getName();
  if ( oldName.empty() ) oldName = VFR_NONAME;

  vsnTextEntryDlg dlg(getUiView(),
			wxT("Enter new name of the current object\n"),
                        wxT("rename object"),
                        vsnApp::ConvSysToWx(oldName));
  if ( dlg.ShowModal() != wxID_OK ) return;

  wxString newNameStr = dlg.GetValue();
  if ( newNameStr.IsEmpty() ) return;
  string newName = vsnApp::ConvWxToSys(newNameStr);
  if ( oldName == newName ) return;

  pRef->setName(newName);
  vsnApp::GetApp()->update(true);
}


//----------------------------------------------------------------
// class vsnUiView
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnUiView, wxPanel)
  EVT_COMBOBOX(UiView_SceneCombo, vsnUiView::OnSceneComboBox)
  EVT_BUTTON(UiView_DataReload, vsnUiView::OnDataReloadBtn)
  EVT_BUTTON(UiView_DataAutoReload, vsnUiView::OnDataAutoReloadBtn)
  EVT_BUTTON(UiView_DataDelete, vsnUiView::OnDataDeleteBtn)
  EVT_TEXT_ENTER(UiView_DataName, vsnUiView::OnDataNameTxt)
  EVT_CHECKBOX(UiView_DataShow, vsnUiView::OnDataShowChk)
  EVT_BUTTON(UiView_DataXForm, vsnUiView::OnDataXFormBtn)
  EVT_BUTTON(UiView_DataAddMtd, vsnUiView::OnDataAddMethodBtn)
  EVT_CHECKBOX(UiView_DataShowBbox, vsnUiView::OnDataShowBboxChk)
  EVT_BUTTON(UiView_MethodDelete, vsnUiView::OnMethodDeleteBtn)
  EVT_TEXT_ENTER(UiView_MethodName, vsnUiView::OnMethodNameTxt)
  EVT_CHECKBOX(UiView_MethodShow, vsnUiView::OnMethodShowChk)
  EVT_CHECKBOX(UiView_MethodLighting, vsnUiView::OnMethodLightingChk)
  EVT_BUTTON(UiView_MethodPropEdit, vsnUiView::OnMethodPropEditBtn)
  EVT_BUTTON(UiView_MethodCMapEdit, vsnUiView::OnMethodCMapEditBtn)
  EVT_TEXT_ENTER(UiView_MethodCMapMin, vsnUiView::OnMethodCMapMinTxt)
  EVT_TEXT_ENTER(UiView_MethodCMapMax, vsnUiView::OnMethodCMapMaxTxt)
END_EVENT_TABLE()


/* constructors, destructor */

vsnUiView::vsnUiView(vsnFrameBase* parent,
		     const wxPoint& pos, const wxSize& size)
  : wxPanel(parent, -1, pos, size), p_parent(parent), p_mpp(NULL),
    m_pXFormDlg(NULL), m_pCMapDlg(NULL), m_pMtdPropDlg(NULL)
{
  // top-sizer
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *panelSizer, *sizerH;

  //------------- Splitter1 -------------
  m_splitter1 = new wxSplitterWindow(this, -1, wxDefaultPosition,
				     wxSize(240, 330));
  m_splitter1->SetMinimumPaneSize(60);
  topSizer->Add(m_splitter1, 1, wxEXPAND, 0);


  //------------- Scene area -------------
  m_pPanelScene = new wxPanel(m_splitter1);
  panelSizer = new wxBoxSizer(wxVERTICAL);
  m_pPanelScene->SetSizer(panelSizer);

  // scene combo list
  m_pSceneCombo = new wxComboBox(m_pPanelScene, UiView_SceneCombo, wxT(""),
				 wxDefaultPosition, wxDefaultSize, 0, NULL,
				 wxCB_READONLY);
  panelSizer->Add(m_pSceneCombo, 0, wxEXPAND|wxALL, 3);

  // tree control
  wxSize szCombo = m_pSceneCombo->GetSize();
  m_pTreeCtrl = new vsnTreeCtrl(m_pPanelScene, UiView_TreeCtrl,
				wxDefaultPosition, wxDefaultSize);
  panelSizer->Add(m_pTreeCtrl, 1, wxEXPAND|wxALL, 3);

  panelSizer->Layout();

  //------------- Splitter2 -------------
  m_splitter2 = new wxSplitterWindow(m_splitter1, -1,
				     wxDefaultPosition, wxSize(240, 250),
				     wxSP_3D|wxSP_LIVE_UPDATE);
  m_splitter2->SetMinimumPaneSize(60);

  //------------- Data Panel -------------
  m_pPanelData = new wxPanel(m_splitter2);
  panelSizer = new wxBoxSizer(wxVERTICAL);
  m_pPanelData->SetSizer(panelSizer);

  panelSizer->Add(
    new wxStaticLine(m_pPanelData, -1, wxDefaultPosition,
		     wxSize(3,3), wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);

  // title, name
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(new wxStaticText(m_pPanelData, -1, wxT("Data")),
	      0, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pDataName = new wxTextCtrl(m_pPanelData, UiView_DataName, wxT(""),
			       wxDefaultPosition, wxDefaultSize,
			       wxTE_PROCESS_ENTER);
  sizerH->Add(m_pDataName, 1, wxEXPAND|wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  // show, delete
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pDataShow = new wxCheckBox(m_pPanelData, UiView_DataShow, wxT("show"));
  m_pDataShow->SetValue(TRUE);
  sizerH->Add(m_pDataShow, 0, wxALL, 3);
  m_pDataShowBbox
    = new wxCheckBox(m_pPanelData, UiView_DataShowBbox, wxT("show bbox"));
  sizerH->Add(m_pDataShowBbox, 0, wxALIGN_LEFT|wxALL, 3);
  m_pDataDelete = new wxButton(m_pPanelData, UiView_DataDelete, wxT("delete"));
  sizerH->Add(5, 5, 1, wxEXPAND);
  sizerH->Add(m_pDataDelete, 0, wxALIGN_RIGHT|wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  // reload, xform
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pDataReload = new wxButton(m_pPanelData, UiView_DataReload,
			       wxT(" reload "), wxDefaultPosition,
			       wxSize(-1,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pDataReload, 0, wxALIGN_LEFT|wxALL, 3);
  m_pDataAutoReload = new wxButton(m_pPanelData, UiView_DataAutoReload,
				   wxT("auto reload..."), wxDefaultPosition,
				   wxSize(-1,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pDataAutoReload, 0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(5, 5, 1, wxEXPAND);
  m_pDataXform = new wxButton(m_pPanelData, UiView_DataXForm, wxT("xform"));
  sizerH->Add(m_pDataXform, 0, wxALIGN_RIGHT|wxALL, 3);
  panelSizer->Add(sizerH, 0, wxALIGN_RIGHT|wxEXPAND);

  // method combo list
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pMethodCombo = new wxComboBox(m_pPanelData, UiView_DataMtdCombo, wxT(""),
				  wxDefaultPosition, wxDefaultSize, 0, NULL,
				  wxCB_READONLY);
  sizerH->Add(m_pMethodCombo, 1, wxEXPAND|wxALL, 3);
  m_pAddMethodBtn = new wxButton(m_pPanelData, UiView_DataAddMtd,
				 wxT("add method"));
  sizerH->Add(m_pAddMethodBtn, 0, wxALIGN_RIGHT|wxALL, 3);  
  panelSizer->Add(sizerH, 0, wxEXPAND);

  panelSizer->Layout();

  //------------- Method Panel -------------
  m_pPanelMethod = new wxPanel(m_splitter2);
  panelSizer = new wxBoxSizer(wxVERTICAL);
  m_pPanelMethod->SetSizer(panelSizer);

  panelSizer->Add(
    new wxStaticLine(m_pPanelMethod, -1, wxDefaultPosition,
		     wxSize(3,3), wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);

  // title, name
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(new wxStaticText(m_pPanelMethod, -1, wxT("Method") ),
	      0, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pMethodName = new wxTextCtrl(m_pPanelMethod, UiView_MethodName, wxT(""),
				 wxDefaultPosition, wxSize(100,25),
				 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pMethodName, 1, wxEXPAND|wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  // show, lighting, delete
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pMethodShow = new wxCheckBox(m_pPanelMethod, UiView_MethodShow,
				 wxT("show"));
  m_pMethodShow->SetValue(TRUE);
  sizerH->Add(m_pMethodShow, 0, wxALL, 3);
  m_pMethodLight = new wxCheckBox(m_pPanelMethod, UiView_MethodLighting,
				  wxT("lighting"));
  m_pMethodLight->SetValue(TRUE);
  sizerH->Add(m_pMethodLight, 0, wxALL, 3);
  sizerH->Add(5, 5, 1, wxEXPAND);
  m_pMethodDelete = new wxButton(m_pPanelMethod, UiView_MethodDelete,
				 wxT("delete"));
  sizerH->Add(m_pMethodDelete, 0, wxALIGN_RIGHT|wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  // color
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pColorBar = new vsnCMapBar(m_pPanelMethod,
			       wxDefaultPosition, wxSize(160,25));
  m_pColorBar->setUseLut(false);
  sizerH->Add(m_pColorBar, 1, wxEXPAND|wxALL, 3);
  m_pPropEditBtn = new wxButton(m_pPanelMethod, UiView_MethodPropEdit,
				wxT("edit"));
  sizerH->Add(m_pPropEditBtn, 0, wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  // lut
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pCMapBar = new vsnCMapBar(m_pPanelMethod,
			      wxDefaultPosition, wxSize(160,25));
  sizerH->Add(m_pCMapBar, 1, wxEXPAND|wxALL, 3);
  m_pCMapEditBtn = new wxButton(m_pPanelMethod, UiView_MethodCMapEdit,
				wxT("edit"));
  sizerH->Add(m_pCMapEditBtn, 0, wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(new wxStaticText(m_pPanelMethod, -1, wxT("min") ),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pCMapMin = new wxTextCtrl(m_pPanelMethod, UiView_MethodCMapMin, wxT("0.0"),
			      wxDefaultPosition, wxSize(75,-1),
			      wxTE_PROCESS_ENTER);
  sizerH->Add(m_pCMapMin, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(m_pPanelMethod, -1, wxT("max") ),
	      0, wxALIGN_RIGHT|wxALL, 3);
  m_pCMapMax = new wxTextCtrl(m_pPanelMethod, UiView_MethodCMapMax, wxT("1.0"),
			      wxDefaultPosition, wxSize(75,-1),
			      wxTE_PROCESS_ENTER);
  sizerH->Add(m_pCMapMax, 1, wxEXPAND|wxALL, 3);
  panelSizer->Add(sizerH, 0, wxEXPAND);

  // method param area
  panelSizer->Add(
    new wxStaticLine(m_pPanelMethod, -1, wxDefaultPosition,
		     wxSize(3,3), wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);
  m_pMethodParamArea
    = new wxScrolledWindow(m_pPanelMethod, UiView_MethodParamArea,
			   wxDefaultPosition, wxDefaultSize,
			   wxScrolledWindowStyle);
  m_pMethodParamArea->SetScrollbars(1, 1, size.GetWidth()-10, 600);
  panelSizer->Add(m_pMethodParamArea, 1, wxEXPAND|wxALL, 3);
  wxBoxSizer* ppaSizer = new wxBoxSizer(wxVERTICAL);
  m_pMethodParamArea->SetSizer(ppaSizer);

  panelSizer->Layout();

  // Layouts
  topSizer->Layout();
  m_splitter1->SplitHorizontally(m_pPanelScene, m_splitter2, 80);
  m_splitter2->SplitHorizontally(m_pPanelData, m_pPanelMethod, 140);

  // topSizer setup
  topSizer->SetSizeHints(this);
  SetSizer(topSizer);
}

vsnUiView::~vsnUiView() {
}


/* methods */

void vsnUiView::reset() {
  // clear scene combolist
  if ( m_pSceneCombo )
    m_pSceneCombo->Clear();

  // clear tree
  if ( m_pTreeCtrl )
    (void)m_pTreeCtrl->updateScene(NULL, m_treeItemLst);

  // clear method-panel
  if ( m_pMethodName )
    m_pMethodName->SetValue(wxT(""));
  if ( m_pColorBar ) {
    vector4 cv = {1.f, 1.f, 1.f, 1.f};
    m_pColorBar->setBaseColor(cv);
  }
  if ( m_pCMapBar )
    m_pCMapBar->setLut(vsnLut());
  if ( m_pCMapMin )
    m_pCMapMin->SetValue(wxT("0.0"));
  if ( m_pCMapMax )
    m_pCMapMax->SetValue(wxT("1.0"));
  if ( p_mpp ) {
    p_mpp->Destroy();
    p_mpp = NULL;
  }
  if ( m_pMethodParamArea )
    m_pMethodParamArea->GetSizer()->Layout();

  // clear data-panel
  if ( m_pDataName )
    m_pDataName->SetValue(wxT(""));
  if ( m_pMethodCombo )
    m_pMethodCombo->Clear();

  // clear status
  if ( p_parent )
    p_parent->setStatusText(string("no data selected"));

  GetSizer()->Layout();
  enableMethodArea(FALSE);
  enableDataArea(FALSE);
}

bool vsnUiView::updateScene(vsnScene* psc) {
  if ( ! psc ) return false;

  if ( m_pTreeCtrl ) {
    if ( ! m_pTreeCtrl->updateScene(psc, m_treeItemLst) )
      return false;
  }

  if ( m_pSceneCombo ) {
    int sel = m_pSceneCombo->FindString(vsnApp::ConvSysToWx(psc->getName()));
    if ( sel >= 0 )
      m_pSceneCombo->SetSelection(sel);
  }

  return true;
}

bool vsnUiView::updateSceneList(const deque<vsnScene*>& scl) {
  if ( ! m_pSceneCombo ) return false;
  wxString oldSceneName;
  int oldsel = m_pSceneCombo->GetSelection();
  if ( oldsel >= 0 ) oldSceneName = m_pSceneCombo->GetString(oldsel);

  // clean-up
  m_pSceneCombo->Clear();

  // create list
  deque<vsnScene*>::const_iterator it;
  for ( it = scl.begin(); it != scl.end(); it++ ) {
    if ( ! *it ) continue;
    m_pSceneCombo->Append(vsnApp::ConvSysToWx((*it)->getName()));
  } // end of for(it)
  if ( m_pSceneCombo->GetCount() != scl.size() )
    return false;

  // the current scene
  if ( oldsel >= 0 ) {
    int newsel = m_pSceneCombo->FindString(oldSceneName);
    if ( newsel >= 0 )
      m_pSceneCombo->SetSelection(newsel);
    else
      m_pSceneCombo->SetSelection(0);
  }

  return true;
}


bool vsnUiView::selectObj(vsnTreeItem* pti) {
  if ( ! enableMethodArea(FALSE) ) return false;
  if ( ! enableDataArea(FALSE) ) return false;
  if ( ! m_pTreeCtrl ) return false;
  if ( ! pti ) return false;

  TreeItemType type = pti->getType();
  if ( type == TI_Method ) {
    vsnTreeItem* ptiParent = dynamic_cast<vsnTreeItem*>
      (m_pTreeCtrl->GetItemData(m_pTreeCtrl->GetItemParent(pti->GetId())));
    if ( ! ptiParent || ptiParent->getType() != TI_Data )
      return false;
    if ( ! updateData(dynamic_cast<vsnDataObj*>(ptiParent->getRefNode())) )
      return false;
    if ( ! updateMethod(dynamic_cast<vsnMethodObj*>(pti->getRefNode())) )
      return false;
    enableMethodArea(TRUE);
    enableDataArea(TRUE);
  } // end of if(TI_Method)
  else if ( type == TI_Data ) {
    if ( ! updateData(dynamic_cast<vsnDataObj*>(pti->getRefNode())) )
      return false;
    enableDataArea(TRUE);
  } // end of if(TI_Data)
  else {
    if ( p_parent )
      p_parent->setStatusText(string("no data selected"));
  }

  return true;
}

bool vsnUiView::selectObj(vfrNode* pnode) {
  if ( ! m_pTreeCtrl ) return false;
  if ( ! pnode ) {
    m_pTreeCtrl->SelectItem(m_pTreeCtrl->GetRootItem());
    return true;
  }

  vfrNode* pref;
  vsnTreeItemMapType::iterator it;
  for ( it = m_treeItemLst.begin(); it != m_treeItemLst.end(); it++ ) {
    if ( ! it->second ) continue;
    pref = it->second->getRefNode();
    if ( ! pref ) continue;
    if ( pref == pnode ) break;
  } // end of for(it)
  if ( it == m_treeItemLst.end() )
    return false;

  m_pTreeCtrl->SelectItem(it->first);
  return true;
}


bool vsnUiView::updateData(vsnDataObj* pdo) {
  if ( ! pdo ) return false;

  // name
  if ( ! m_pDataName ) return false;
  string dname = pdo->getName();
  m_pDataName->SetValue(vsnApp::ConvSysToWx(dname));

  // show
  if ( ! m_pDataShow ) return false;
  bool dshow = pdo->getShow();
  m_pDataShow->SetValue(dshow);

  // method list
  if ( ! m_pMethodCombo ) return false;
  m_pMethodCombo->Clear();

  deque<vsnMethodTmpl*> dtmplLst
    = vsnApp::GetApp()->getTmplList(pdo->getDataType());
  deque<vsnMethodTmpl*>::iterator itdt;
  for ( itdt = dtmplLst.begin(); itdt != dtmplLst.end(); itdt++ ) {
    if ( ! *itdt ) continue;
    string tmplName = string("*") + (*itdt)->getName();
    m_pMethodCombo->Append(vsnApp::ConvSysToWx(tmplName));
  } // end of for(itdt)

  deque<string> dmtdLst = pdo->getSupportMethodList();
  deque<string>::iterator itdm;
  for ( itdm = dmtdLst.begin(); itdm != dmtdLst.end(); itdm++ ) {
    if ( itdm->empty() ) continue;
    m_pMethodCombo->Append(vsnApp::ConvSysToWx(*itdm));
  } // end of for(itdm)

  // bbox
  if ( ! m_pDataShowBbox ) return false;
  bool bbshow = pdo->getBboxShowMode();
  m_pDataShowBbox->SetValue(bbshow);

  // status
  if ( p_parent ) {
    string stsMsg = dname + "[" + pdo->getDataType() + "]";
    p_parent->setStatusText(stsMsg);
  }

  return true;
}

bool vsnUiView::updateMethod(vsnMethodObj* pmo, const bool doMPP) {
  if ( ! pmo ) return false;

  // name
  if ( ! m_pMethodName ) return false;
  string mname = pmo->getName();
  m_pMethodName->SetValue(vsnApp::ConvSysToWx(mname));

  // show
  if ( ! m_pMethodShow ) return false;
  bool mshow = pmo->getShow();
  m_pMethodShow->SetValue(mshow);

  // lighting
  if ( ! m_pMethodLight ) return false;
  if ( ! pmo->canLighting() ) {
    m_pMethodLight->SetValue(FALSE);
  } else {
    bool mlighting = pmo->getLighting();
    m_pMethodLight->SetValue(mlighting);
  }

  // color
  if ( ! m_pColorBar ) return false;
  vector4 cv; pmo->getBaseColor(cv);
  m_pColorBar->setBaseColor(cv);
  m_pColorBar->setHilight(pmo->getHilight());

  // lut
  if ( ! m_pCMapBar || ! m_pCMapMin || ! m_pCMapMax ) return false;
  vsnLut mlut = pmo->getLut();
  m_pCMapBar->setLut(mlut);
  char mlv[32];
  sprintf(mlv, "%g", mlut.minVal);
  m_pCMapMin->SetValue(vsnApp::ConvSysToWx(mlv));
  sprintf(mlv, "%g", mlut.maxVal);
  m_pCMapMax->SetValue(vsnApp::ConvSysToWx(mlv));

  // method param
  if ( ! doMPP ) return true;

  if ( ! m_pMethodParamArea ) return false;
  m_pMethodParamArea->Scroll(0, 0);
  if ( p_mpp ) {
    p_mpp->Destroy();
    p_mpp = NULL;
  }
  p_mpp = pmo->getParamPanel(m_pMethodParamArea);
  if ( ! p_mpp ) return false;

  // re-layout
  m_pMethodParamArea->GetSizer()->Layout(); // need on Windows
  GetSizer()->Layout(); // need on GTK

  // status
  vsnDataObj* pdo = pmo->getRefData();
  if ( p_parent && pdo ) {
    string stsMsg = pdo->getName() + "[" + pdo->getDataType() + "] / ";
    stsMsg += mname + "[" + pmo->getMethodType() + "]";
    p_parent->setStatusText(stsMsg);
  }

  return true;
}


bool vsnUiView::enableDataArea(const bool mode) {
  if ( ! m_pDataName ) return false;
  if ( ! m_pDataReload ) return false;
  if ( ! m_pDataAutoReload ) return false;
  if ( ! m_pDataDelete ) return false;
  if ( ! m_pDataShow ) return false;
  if ( ! m_pDataXform ) return false;
  if ( ! m_pMethodCombo ) return false;
  if ( ! m_pAddMethodBtn ) return false;
  if ( ! m_pDataShowBbox ) return false;

  m_pDataName->Enable(mode);
  m_pDataReload->Enable(mode);
  m_pDataAutoReload->Enable(mode);
  m_pDataDelete->Enable(mode);
  m_pDataShow->Enable(mode);
  m_pDataXform->Enable(mode);
  if ( m_pXFormDlg ) m_pXFormDlg->Hide();
  m_pMethodCombo->Enable(mode);
  m_pAddMethodBtn->Enable(mode);
  m_pDataShowBbox->Enable(mode);
  return true;
}

bool vsnUiView::enableMethodArea(const bool mode) {
  if ( ! m_pMethodName ) return false;
  if ( ! m_pMethodDelete ) return false;
  if ( ! m_pMethodShow ) return false;
  if ( ! m_pMethodLight ) return false;
  if ( ! m_pCMapBar ) return false;
  if ( ! m_pCMapEditBtn ) return false;
  if ( ! m_pCMapMin ) return false;
  if ( ! m_pCMapMax ) return false;
  if ( ! m_pMethodParamArea ) return false;

  m_pMethodName->Enable(mode);
  m_pMethodDelete->Enable(mode);
  m_pMethodShow->Enable(mode);
  m_pMethodLight->Enable(mode);
  m_pColorBar->Enable(mode);
  m_pPropEditBtn->Enable(mode);
  if ( m_pMtdPropDlg ) m_pMtdPropDlg->Hide();
  m_pCMapBar->Enable(mode);
  m_pCMapEditBtn->Enable(mode);
  if ( m_pCMapDlg ) m_pCMapDlg->Hide();
  m_pCMapMin->Enable(mode);
  m_pCMapMax->Enable(mode);
  m_pMethodParamArea->Enable(mode);
  m_pMethodParamArea->FitInside();

  if ( ! mode && p_mpp ) {
    p_mpp->settlement();
  }

  return true;
}


vsnScene* vsnUiView::getCurrentScene() {
  if ( ! p_parent ) return NULL;
  vsnScene* psc = p_parent->getScene();
  return psc;
}

vsnDataObj* vsnUiView::getCurrentData() {
  if ( ! m_pTreeCtrl ) return NULL;

  // get current item of TreeCtrl
  vsnTreeItem* item = dynamic_cast<vsnTreeItem*>
    (m_pTreeCtrl->GetItemData(m_pTreeCtrl->GetSelection()));
  if ( ! item ) return NULL;

  vsnDataObj* pdo = NULL;
  switch ( item->getType() ) {
  case TI_Data:
    pdo = dynamic_cast<vsnDataObj*>(item->getRefNode());
    break;
  case TI_Method: {
    vsnMethodObj* pm = dynamic_cast<vsnMethodObj*>(item->getRefNode());
    if ( ! pm ) break;
    pdo = pm->getRefData();
    break;
  }
  default:
    break;
  } // end of switch(item->getType())
  return pdo;
}

vsnMethodObj* vsnUiView::getCurrentMethod() {
  if ( ! m_pTreeCtrl ) return NULL;

  // get current item of TreeCtrl
  vsnTreeItem* item = dynamic_cast<vsnTreeItem*>
    (m_pTreeCtrl->GetItemData(m_pTreeCtrl->GetSelection()));
  if ( ! item ) return NULL;

  vsnMethodObj* pmo = NULL;
  switch ( item->getType() ) {
  case TI_Method: {
    pmo = dynamic_cast<vsnMethodObj*>(item->getRefNode());
    break;
  }
  default:
    break;
  } // end of swicth(item->getType())
  return pmo;
}


/* event handlers */

void vsnUiView::OnSceneComboBox(wxCommandEvent& event) {
  long sel = event.GetInt();
  if ( sel < 0 ) return;

  if ( ! p_parent ) return;
  vsnApp* p_app = p_parent->getApp();
  if ( ! p_app ) return;

  vsnScene* p_newScene = p_app->getScene((const size_t)sel);
  if ( p_newScene && p_newScene == p_parent->getScene() )
    return; // unchanged.

  p_parent->setScene(p_newScene);
}

void vsnUiView::OnDataReloadBtn(wxCommandEvent& event) {
  vsnDataObj* pDt = getCurrentData();
  if ( ! pDt ) return;

  string msg("reload data ");
  msg += pDt->getName() + string("[") + pDt->getDataType() + string("]");
  wxMessageDialog dlg(this, vsnApp::ConvSysToWx(msg), wxT("reload_data"),
		      vsn_wxOK_CANCEL|wxICON_QUESTION);
  if ( dlg.ShowModal() != vsn_wxIDOK ) return;

  // reload data
  vsnDataReload* pReloader = pDt->getReloader(false);
  if ( pReloader ) {
    if ( ! pReloader->checkFiles() ) {
      ErrMsg(MsgERR, string("UiView: ") + msg + " failed.");
      return;
    }
  } else {
    if ( ! pDt->reload() ) {
      ErrMsg(MsgERR, string("UiView: ") + msg + " failed.");
      return;
    }
  }

  vsnApp::GetApp()->update(false);
}

void vsnUiView::OnDataAutoReloadBtn(wxCommandEvent& event) {
  vsnDataObj* pDt = getCurrentData();
  if ( ! pDt ) return;

  vsnDataReloadDlg dlg(this, pDt);
  dlg.ShowModal();
}

void vsnUiView::OnDataDeleteBtn(wxCommandEvent& event) {
  vsnDataObj* pDt = getCurrentData();
  if ( ! pDt ) return;

  string msg("delete data ");
  msg += pDt->getName() + string("[") + pDt->getDataType() + string("]");
  wxMessageDialog dlg(this, vsnApp::ConvSysToWx(msg), wxT("del_data"),
		      vsn_wxOK_CANCEL|wxICON_QUESTION);
  if ( dlg.ShowModal() != vsn_wxIDOK ) return;

  // delete data and methods
  delete pDt;

  vsnApp::GetApp()->update(false);
}

void vsnUiView::OnDataNameTxt(wxCommandEvent& event) {
  if ( ! m_pDataName ) return;
  vsnDataObj* pDt = getCurrentData();
  if ( ! pDt ) return;

  wxString valStr = m_pDataName->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; string xnm = pDt->getName();
    sprintf(txt, "%s", (xnm.empty() ? VFR_NONAME : xnm.c_str()));
    m_pDataName->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  string nname = vsnApp::ConvWxToSys(valStr);
  if ( nname == pDt->getName() ) return;

  pDt->setName(nname);
  vsnApp::GetApp()->update(true);
}

void vsnUiView::OnDataShowChk(wxCommandEvent& event) {
  if ( ! m_pDataShow || ! m_pTreeCtrl ) return;
  bool val = m_pDataShow->GetValue();

  // get current data
  vsnDataObj* pdo = getCurrentData();
  if ( ! pdo ) return;

  pdo->setShow(val);
  pdo->chkNotice();
  vsnApp::GetApp()->update(true);
}

void vsnUiView::OnDataXFormBtn(wxCommandEvent& event) {
  // get current data
  vsnDataObj* pdo = getCurrentData();
  if ( ! pdo ) return;

  if ( ! m_pXFormDlg ) {
    m_pXFormDlg = new vsnXFormDlg(this, NULL);
    if ( ! m_pXFormDlg ) {
      ErrMsg(MsgERR, string("UiView: can't create XFormDlg"));
      return;
    }
    m_pXFormDlg->SetTitle(wxT("xform data"));
  } // end of if(!m_pXFormDlg)

  m_pXFormDlg->setRefXForm(pdo);
  m_pXFormDlg->Show(TRUE);
}

void vsnUiView::OnDataAddMethodBtn(wxCommandEvent& event) {
  if ( ! m_pMethodCombo ) return;
  wxString selStr = m_pMethodCombo->GetStringSelection();
  if ( selStr.IsEmpty() ) {
    ErrMsg(MsgERR, string("UiView: add_method: no method selected"));
    return;
  }
  m_pMethodCombo->SetSelection(-1);
  m_pMethodCombo->SetValue(wxT(""));

  vsnDataObj* pCurData = getCurrentData();
  if ( ! pCurData ) {
    ErrMsg(MsgERR, string("UiView: add_method: can't get data"));
    return;
  }

  // check tmpl
  if ( selStr[0] == '*' ) {
    string tmplName = vsnApp::ConvWxToSys(selStr).substr(1);
    if ( tmplName.empty() ) {
      ErrMsg(MsgERR, string("UiView: add_method: invalid tmpl name"));
      return;
    }
    vsnMethodTmpl* ptmpl
      = vsnApp::GetApp()->getTmpl(tmplName, pCurData->getDataType());
    if ( ! ptmpl ) {
      ErrMsg(MsgERR, string("UiView: add_method: can't get tmpl: ") +tmplName);
      return;
    }
    if ( ! ptmpl->apply(pCurData) ) {
      ErrMsg(MsgERR, string("UiView: add_method: apply tmpl failed"));
      return;
    }
    selectObj(pCurData);
    return;
  }

  // don't use XML command, so nonamed 'data' can add_method from here.
  string methodType = vsnApp::ConvWxToSys(selStr);
  vsnMethodObj* pMtd = pCurData->addNewMethod(methodType);
  if ( ! pMtd ) {
    ErrMsg(MsgERR, string("UiView: add_method failed"));
    return;
  }

  if ( pMtd->getName() == string(VFR_NONAME) ) {
    char txtbuf[64];
    sprintf(txtbuf, "%s_%lu",
	    pMtd->getMethodType().c_str(), pMtd->getMethodSeq());
    vsnScene* pscn = pCurData->getAncestorScene();
    if ( pscn && pscn->getNode(txtbuf) ) {
      string wkstr(txtbuf);
      register size_t xt;
      for ( xt = 1; ; xt++ ) {
	sprintf(txtbuf, "%s-%lu", wkstr.c_str(), xt);
	if ( ! pscn->getNode(txtbuf) ) break;
      } // end of for(xt)
    }
    pMtd->setName(txtbuf);
  }

  if ( ! pMtd->update(false) ) {
    ErrMsg(MsgERR, string("UiView: add_method failed to update new method"));
    delete pMtd;
    return;
  }

  vsnApp::GetApp()->update(false);
  if ( ! selectObj(pMtd) ) return;
}

void vsnUiView::OnDataShowBboxChk(wxCommandEvent& event) {
  if ( ! m_pDataShowBbox || ! m_pTreeCtrl ) return;
  Bool val = m_pDataShowBbox->GetValue();

  // get current data
  vsnDataObj* pdo = getCurrentData();
  if ( ! pdo ) return;
  if ( pdo->getBboxShowMode() == val ) return;

  pdo->setBboxShowMode(val);
  vsnApp::GetApp()->update(true);
}


void vsnUiView::OnMethodDeleteBtn(wxCommandEvent& event) {
  vsnMethodObj* pMtd = getCurrentMethod();
  if ( ! pMtd ) return;
  vsnDataObj* pRefData = pMtd->getRefData();
  if ( ! pRefData ) return;

  string msg("delete method ");
  msg += pMtd->getName() + string("[") + pMtd->getMethodType() + string("]");
  msg += string(" from data ") + pRefData->getName();
  msg += string("[") + pRefData->getDataType() + string("]");
  wxMessageDialog dlg(this, vsnApp::ConvSysToWx(msg), wxT("del_method"),
		      vsn_wxOK_CANCEL|wxICON_QUESTION);
  if ( dlg.ShowModal() != vsn_wxIDOK ) return;

  pRefData->delMethod(pMtd);
  delete pMtd;

  vsnApp::GetApp()->update(false);
  selectObj(pRefData);
}

void vsnUiView::OnMethodNameTxt(wxCommandEvent& event) {
  if ( ! m_pMethodName ) return;
  vsnMethodObj* pMtd = getCurrentMethod();
  if ( ! pMtd ) return;

  wxString valStr = m_pMethodName->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; string xnm = pMtd->getName();
    sprintf(txt, "%s", (xnm.empty() ? VFR_NONAME : xnm.c_str()));
    m_pMethodName->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  string nname = vsnApp::ConvWxToSys(valStr);
  if ( nname == pMtd->getName() ) return;

  pMtd->setName(nname);
  vsnApp::GetApp()->update(true);
}

void vsnUiView::OnMethodShowChk(wxCommandEvent& event) {
  if ( ! m_pMethodShow || ! m_pTreeCtrl ) return;
  bool val = m_pMethodShow->GetValue();

  // get current method
  vsnMethodObj* pm = getCurrentMethod();
  if ( ! pm ) return;

  pm->setShow(val);
  pm->chkNotice();
}

void vsnUiView::OnMethodLightingChk(wxCommandEvent& event) {
  if ( ! m_pMethodLight || ! m_pTreeCtrl ) return;
  bool val = m_pMethodLight->GetValue();

  // get current method
  vsnMethodObj* pm = getCurrentMethod();
  if ( ! pm ) return;

  if ( val && ! pm->canLighting() ) {
    m_pMethodLight->SetValue(false);
    return;
  }

  pm->setLighting(val);
  pm->chkNotice();
}

void vsnUiView::OnMethodPropEditBtn(wxCommandEvent& event) {
  // get current method
  vsnMethodObj* pMtd = getCurrentMethod();
  if ( ! pMtd ) return;

  // prepare dialog
  if ( ! m_pMtdPropDlg ) {
    m_pMtdPropDlg = new vsnMethodEditPropDlg(this, pMtd);
    if ( ! m_pMtdPropDlg ) {
      ErrMsg(MsgERR, string("UiView: can't create EditPropDlg"));
      return;
    }
  } // end of if(!m_pMtdPropDlg)
  else {
    m_pMtdPropDlg->setRefMethod(pMtd);
  } // end of if(m_pMtdPropDlg)

  // dialog title
  wxString titleStr = wxT("EditPropDlg of ");
  titleStr += vsnApp::ConvSysToWx(pMtd->getName());
  titleStr += wxT("[");
  titleStr += vsnApp::ConvSysToWx(pMtd->getMethodType());
  titleStr += wxT("]");
  m_pMtdPropDlg->SetTitle(titleStr);

  // show dlg
  m_pMtdPropDlg->ShowModal();
}

void vsnUiView::OnMethodCMapEditBtn(wxCommandEvent& event) {
  // get current method
  vsnMethodObj* pMtd = getCurrentMethod();
  if ( ! pMtd ) return;

  // prepare dialog
  if ( ! m_pCMapDlg ) {
    m_pCMapDlg = new vsnCMapDlg(this, pMtd);
    if ( ! m_pCMapDlg ) {
      ErrMsg(MsgERR, string("UiView: can't create CMapDlg"));
      return;
    }
  } // end of (!m_pCMapDlg)
  else {
    m_pCMapDlg->setRefMethod(pMtd);
  } // end of (m_pCMapDlg)

  // dialog title
  wxString titleStr = wxT("CMapDlg of ");
  titleStr += vsnApp::ConvSysToWx(pMtd->getName());
  titleStr += wxT("[");
  titleStr += vsnApp::ConvSysToWx(pMtd->getMethodType());
  titleStr += wxT("]");
  m_pCMapDlg->SetTitle(titleStr);

  // get display mode
  bool disp_mode = false;
  vsnScene* psc = getCurrentScene();
  if ( psc ) {
    vsnColorBar* pcb = pMtd->getColorBar();
    size_t nfo = psc->getNumFrontObj();
    for ( size_t i = 0; i < nfo; i++ ) {
      vsnColorBar* pfo = dynamic_cast<vsnColorBar*>(psc->getFrontObj(i));
      if ( ! pfo ) continue;
      if ( pfo == pcb ) {
	disp_mode = true;
	break;
      }
    } // end of for(i)
  } // end of if(psc)

  m_pCMapDlg->setDispCBar(disp_mode);
  m_pCMapDlg->ShowModal();
}

void vsnUiView::OnMethodCMapMinTxt(wxCommandEvent& event) {
  if ( ! m_pCMapMin ) return;
  vsnMethodObj* pMtd = getCurrentMethod();
  if ( ! pMtd ) return;
  vsnLut mlut = pMtd->getLut();

  wxString valStr = m_pCMapMin->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; sprintf(txt, "%g", mlut.minVal);
    m_pCMapMin->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  float val = atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == mlut.minVal ) return;
  if ( val > mlut.maxVal ) {
    char txt[64]; sprintf(txt, "%g", mlut.minVal);
    m_pCMapMin->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  mlut.minVal = val;
  mlut.isStdLut = false;
  pMtd->setLut(mlut);
}

void vsnUiView::OnMethodCMapMaxTxt(wxCommandEvent& event) {
  if ( ! m_pCMapMax ) return;
  vsnMethodObj* pMtd = getCurrentMethod();
  if ( ! pMtd ) return;
  vsnLut mlut = pMtd->getLut();

  wxString valStr = m_pCMapMax->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; sprintf(txt, "%g", mlut.maxVal);
    m_pCMapMax->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  float val = atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == mlut.maxVal ) return;
  if ( val < mlut.minVal ) {
    char txt[64]; sprintf(txt, "%g", mlut.maxVal);
    m_pCMapMax->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  mlut.maxVal = val;
  mlut.isStdLut = false;
  pMtd->setLut(mlut);
}
