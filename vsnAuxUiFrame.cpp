//
// vsnAuxUiFrame
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

#include "vsnAuxUiFrame.h"
#include "vsnViewFrame.h"
#include "vsnApp.h"


using namespace std;
using namespace CES;
using namespace VSN;

#define FRAME_DEFAULT_W 240
#define FRAME_DEFAULT_H 512


BEGIN_EVENT_TABLE(vsnAuxUiFrame, wxFrame)
  EVT_CLOSE(vsnAuxUiFrame::OnClose)
END_EVENT_TABLE()


/* constructors / destructor */

vsnAuxUiFrame::vsnAuxUiFrame(vsnViewFrame* parent, const wxString& title,
			     const wxPoint& pos, const wxSize& size,
			     long style)
  : vsnFrameBase(parent, title, pos, wxDefaultSize, style),
    p_master(parent), m_pUiView(NULL)
{
  assert(p_master);

  /* prepare UiView */
  m_pUiView = new vsnUiView(this);
  assert(m_pUiView);

  /* set Sizer */
  if ( size == wxDefaultSize )
    SetSize(-1, -1, FRAME_DEFAULT_W, FRAME_DEFAULT_H);
  else
    SetSize(-1, -1, size.GetWidth(), size.GetHeight());
  wxBoxSizer* topsizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(m_pUiView, 0, wxEXPAND);
  SetSizer(topsizer);
#ifdef MacOSX
  topsizer->Layout(); // need on MacOSX
#endif
  if ( size == wxDefaultSize ) {
    wxSize uvSz = m_pUiView->GetSize();
    if ( uvSz.x != 0 )
      SetSize(-1, -1, uvSz.GetWidth(), -1);
  }

  /* set icon */
  SetIcon(vfrDrawAreaWx::GetVFRIcon());

  /* update scene list */
  p_app = vsnApp::GetApp();
  deque<vsnScene*> sceneLst;
  size_t i, nScn = p_app->getNumScene();
  for ( i = 0; i < nScn; i++ ) {
    vsnScene* psc = p_app->getScene(i);
    if ( ! psc ) continue;
    sceneLst.push_back(psc);
  } // end of for(i)
  updateSceneList(sceneLst);
}

vsnAuxUiFrame::~vsnAuxUiFrame() {
  if ( m_pUiView ) {
    vsnTreeItem ti(TI_Scene);
    m_pUiView->selectObj(&ti);
  }

  if ( p_master ) {
    p_master->invalidateAuxUiFrame(this);
  }
}


/* methods */

void vsnAuxUiFrame::refresh(const bool reSelObj) {
  // UiView part
  if ( m_pUiView ) {
    vsnDataObj* pDt = NULL;
    vsnMethodObj* pMtd = NULL;
    if ( reSelObj ) {
      pDt = m_pUiView->getCurrentData();
      pMtd = m_pUiView->getCurrentMethod();
    }
    if ( p_scene )
      m_pUiView->updateScene(p_scene);
    if ( pMtd )
      m_pUiView->selectObj(pMtd);
    else if ( pDt )
      m_pUiView->selectObj(pDt);
  }
}

bool vsnAuxUiFrame::updateSceneList(const std::deque<vsnScene*>& scl) {
  if ( ! m_pUiView ) return false;
  if ( ! m_pUiView->updateSceneList(scl) ) return false;
  return true;
}

void vsnAuxUiFrame::changeScene(class vsnScene* ps) {
  if ( p_scene == ps ) return;
  p_scene = ps;
  if ( m_pUiView )
    m_pUiView->updateScene(ps);
  // don't propagate to p_master
}

bool vsnAuxUiFrame::closeView() {
  if ( ! p_master ) return false;
  p_master->setShowUiView(false);
  return true;
}


/* from vsnFrameBase */

void vsnAuxUiFrame::setScene(class vsnScene* ps) {
  if ( p_scene == ps ) return;
  p_scene = ps;
  if ( m_pUiView )
    m_pUiView->updateScene(ps);
  // propagate to p_master
  if ( p_master )
    p_master->setScene(ps);
}

bool vsnAuxUiFrame::adjustSelection(vsnTreeItem* pti) {
  if ( ! pti ) return false;
  if ( ! p_master ) return false;
  vsnUiView* pmuiv = p_master->getUiView();
  if ( ! pmuiv ) return false;
  return pmuiv->selectObj(pti->getRefNode());
}


/* event handlers */

void vsnAuxUiFrame::OnClose(wxCloseEvent& event) {
  closeView();
}
