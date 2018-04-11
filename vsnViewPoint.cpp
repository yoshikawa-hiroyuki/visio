//
// vsnViewPoint
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
#include "wx/statbmp.h"

#include "vsnViewPoint.h"
#include "vsnApp.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// struct vsnViewPoint
//----------------------------------------------------------------

/* quaternions utils */

CES::Quat4<float> vsnViewPoint::getRotQuat() const {
  CES::Quat4<float> rotQuat;
  const CES::Mat4<float>& m = mR;
  const double EPS = 1e-3;

  double q0, q1, q2, q3;
  q0 = ( m[0] + m[5] + m[10] + 1.0) / 4.0;
  q1 = ( m[0] - m[5] - m[10] + 1.0) / 4.0;
  q2 = (-m[0] + m[5] - m[10] + 1.0) / 4.0;
  q3 = (-m[0] - m[5] + m[10] + 1.0) / 4.0;
  if ( q0 < 0.0 ) q0 = 0.0;
  if ( q1 < 0.0 ) q1 = 0.0;
  if ( q2 < 0.0 ) q2 = 0.0;
  if ( q3 < 0.0 ) q3 = 0.0;
  q0 = sqrt(q0); q1 = sqrt(q1);
  q2 = sqrt(q2); q3 = sqrt(q3);
  if ( q0 < EPS && q1 < EPS ) {
    q0 = 0.0;
    q1 = 0.0;
    q2 *= +1.0;
    if ( m[6] + m[9] < 0.0 )
      q3 *= -1.0;
  }
  else if ( q0 < EPS ) {
    q0 = 0.0;
    q1 *= +1.0;
    if ( m[1] + m[4] < 0.0 )
      q2 *= -1.0;
    if ( m[8] + m[2] < 0.0 )
      q3 *= -1.0;
  }
  else {
    q0 *= +1.0;
    if ( m[6] - m[9] < 0.0 )
      q1 *= -1.0;
    if ( m[8] - m[2] < 0.0 )
      q2 *= -1.0;
    if ( m[1] - m[4] < 0.0 )
      q3 *= -1.0;
  }
  double r = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  rotQuat.m_w = (float)(q0 / r);
  rotQuat.m_v[0] = (float)(q1 / r);
  rotQuat.m_v[1] = (float)(q2 / r);
  rotQuat.m_v[2] = (float)(q3 / r);
  return rotQuat;
}

bool vsnViewPoint::importXMLNode(xmlNodePtr xnp) {
  if ( ! xnp ) return false;

  // is 'view_point' node?
  if ( strcmp((const char*)xnp->name, "view_point") ) return false;

  // name prop
  xmlChar* nm = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! nm || strlen((char*)nm) < 1 ) return false;
  name = (char*)nm; xmlFree(nm);

  // get children node
  xmlNodePtr cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {
      cur = cur->next;
      continue;
    }
    else if ( !strcmp((const char*)cur->name, "translate") ) {
      Vec3<float> val;
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      vT = val;
    } // end of "translate"
    else if ( !strcmp((const char*)cur->name, "center") ) {
      Vec3<float> val;
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      vC = val;
    } // end of "center"
    else if ( !strcmp((const char*)cur->name, "scale") ) {
      Vec3<float> val(1.f, 1.f, 1.f);
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      vS = val;
    } // end of "scale"
    else if ( !strcmp((const char*)cur->name, "rotate") ) {
      Mat4<float> m;
      xmlNodePtr rn = cur->xmlChildrenNode;
      while ( rn ) {
	if ( rn->type == XML_TEXT_NODE ) break;
	rn = rn->next;
      } // end of while(rn)
      if ( ! rn ) {cur = cur->next; continue;}
      if ( sscanf((const char*)rn->content, "%f %f %f %f %f %f %f %f %f",
		  &m.m_v[0], &m.m_v[4], &m.m_v[8],
		  &m.m_v[1], &m.m_v[5], &m.m_v[9],
		  &m.m_v[2], &m.m_v[6], &m.m_v[10]) != 9 ) {
	cur = cur->next; continue;
      }
      mR = m;
    } // end of "rotate"

    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnViewPoint::exportXMLNode(std::ostream& os, const size_t ts) const {
  string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts2(idts); idts2 += "  ";
  string idts4(idts2); idts4 += "  ";

  // output
  os << idts << "<view_point";
  if ( ! name.empty() ) os << " name=\"" << name << "\"";
  os << ">" << endl;

  if ( vT.m_v[0] != 0.f || vT.m_v[1] != 0.f || vT.m_v[2] != 0.f ) {
    os << idts2 << "<translate x=\"" << vT.m_v[0] << "\" y=\""
       << vT.m_v[1] << "\" z=\"" << vT.m_v[2] << "\" />" << endl;
  }
  if ( mR.m_v[0] != 1.f || mR.m_v[1] != 0.f || mR.m_v[2] != 0.f ||
       mR.m_v[4] != 0.f || mR.m_v[5] != 1.f || mR.m_v[6] != 0.f ||
       mR.m_v[8] != 0.f || mR.m_v[9] != 0.f || mR.m_v[10] !=1.f ) {
    os << idts2 << "<rotate>" << endl;
    os << idts4 << mR.m_v[0] << " " << mR.m_v[4] << " " << mR.m_v[8] << endl;
    os << idts4 << mR.m_v[1] << " " << mR.m_v[5] << " " << mR.m_v[9] << endl;
    os << idts4 << mR.m_v[2] << " " << mR.m_v[6] << " " << mR.m_v[10]<< endl;
    os << idts2 << "</rotate>" << endl;
  }
  if ( vS.m_v[0] != 1.f || vS.m_v[1] != 1.f || vS.m_v[2] != 1.f ) {
    os << idts2 << "<scale x=\"" << vS.m_v[0] << "\" y=\""
       << vS.m_v[1] << "\" z=\"" << vS.m_v[2] << "\" />" << endl;
  }
  if ( vC.m_v[0] != 0.f || vC.m_v[1] != 0.f || vC.m_v[2] != 0.f ) {
    os << idts2 << "<center x=\"" << vC.m_v[0] << "\" y=\""
       << vC.m_v[1] << "\" z=\"" << vC.m_v[2] << "\" />" << endl;
  }

  os << idts << "</view_point>" << endl;
  return true;
}


//----------------------------------------------------------------
// class vsnViewPointDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnViewPointDlg, wxDialog)
  EVT_BUTTON(ViewPointDlg_PxBtn, vsnViewPointDlg::OnRightViewBtn)
  EVT_BUTTON(ViewPointDlg_MxBtn, vsnViewPointDlg::OnLeftViewBtn)
  EVT_BUTTON(ViewPointDlg_PyBtn, vsnViewPointDlg::OnTopViewBtn)
  EVT_BUTTON(ViewPointDlg_MyBtn, vsnViewPointDlg::OnBottomViewBtn)
  EVT_BUTTON(ViewPointDlg_PzBtn, vsnViewPointDlg::OnFrontViewBtn)
  EVT_BUTTON(ViewPointDlg_MzBtn, vsnViewPointDlg::OnBackViewBtn)
  EVT_TEXT_ENTER(ViewPointDlg_ViewNameTxt, vsnViewPointDlg::OnViewNameTxt)
  EVT_BUTTON(ViewPointDlg_RegViewBtn, vsnViewPointDlg::OnRegViewBtn)
  EVT_LISTBOX(ViewPointDlg_ViewNameLst, vsnViewPointDlg::OnViewNameLstClick)
  EVT_LISTBOX_DCLICK(ViewPointDlg_ViewNameLst,
		     vsnViewPointDlg::OnViewNameLstDblClick)
  EVT_BUTTON(ViewPointDlg_ResViewBtn, vsnViewPointDlg::OnResViewBtn)
  EVT_BUTTON(ViewPointDlg_DelViewBtn, vsnViewPointDlg::OnDelViewBtn)
  EVT_BUTTON(ViewPointDlg_CloseBtn, vsnViewPointDlg::OnCloseBtn)
  EVT_CLOSE(vsnViewPointDlg::OnClose)
END_EVENT_TABLE()


/* resources */

#include "icon/view_front.xpm"
#include "icon/view_back.xpm"
#include "icon/view_left.xpm"
#include "icon/view_right.xpm"
#include "icon/view_top.xpm"
#include "icon/view_bottom.xpm"


/* constructors, destructor */

vsnViewPointDlg::vsnViewPointDlg(wxWindow *parent, vsnViewFrame* refViewFrame)
  : wxDialog(parent, -1, wxString(wxT("view point"))),
    p_viewFrame(refViewFrame)
{
  assert(parent);

  wxBoxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // standard view buttons
  sizerTop->Add(new wxStaticText(this, -1, wxT("Standard view points")),
                0, wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);
  wxBitmap* pbm;
  pbm = new wxBitmap(view_front);
  m_pPzBtn = new wxBitmapButton(this, ViewPointDlg_PzBtn, *pbm);
  pbm = new wxBitmap(view_back);
  m_pMzBtn = new wxBitmapButton(this, ViewPointDlg_MzBtn, *pbm);
  pbm = new wxBitmap(view_right);
  m_pPxBtn = new wxBitmapButton(this, ViewPointDlg_PxBtn, *pbm);
  pbm = new wxBitmap(view_left);
  m_pMxBtn = new wxBitmapButton(this, ViewPointDlg_MxBtn, *pbm);
  pbm = new wxBitmap(view_top);
  m_pPyBtn = new wxBitmapButton(this, ViewPointDlg_PyBtn, *pbm);
  pbm = new wxBitmap(view_bottom);
  m_pMyBtn = new wxBitmapButton(this, ViewPointDlg_MyBtn, *pbm);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(m_pPzBtn, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pMzBtn, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pPxBtn, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pMxBtn, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pPyBtn, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pMyBtn, 1, wxEXPAND|wxALL, 3);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);

  // registered views
  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);
  sizerTop->Add(new wxStaticText(this, -1, wxT("Registered view points")),
                0, wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);

  // viewpoint name
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pViewNameTxt = new wxTextCtrl(this, ViewPointDlg_ViewNameTxt, wxT(""),
				  wxDefaultPosition, wxDefaultSize,
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pViewNameTxt, 1, wxEXPAND|wxALL, 3);
  m_pRegViewBtn = new wxButton(this, ViewPointDlg_RegViewBtn, wxT("register"));
  sizerH->Add(m_pRegViewBtn, 0, wxALIGN_LEFT|wxALL, 3);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);

  // viewname list / restore, delete viewpoint
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  m_pViewNameLst = new wxListBox(this, ViewPointDlg_ViewNameLst,
                                 wxDefaultPosition, wxSize(200,150));
  sizerH->Add(m_pViewNameLst, 1, wxEXPAND|wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);
  wxBoxSizer* sizerV = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerV, 0, wxALIGN_LEFT);
  m_pResViewBtn = new wxButton(this, ViewPointDlg_ResViewBtn, wxT("restore"));
  sizerV->Add(m_pResViewBtn, 0, wxALIGN_CENTER|wxALL, 3);
  m_pDelViewBtn = new wxButton(this, ViewPointDlg_DelViewBtn, wxT("delete"));
  sizerV->Add(m_pDelViewBtn, 0, wxALIGN_CENTER|wxALL, 3);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);

  // close
  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);
  m_pCloseBtn = new wxButton(this, ViewPointDlg_CloseBtn, wxT("close"));
  sizerTop->Add(m_pCloseBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)updateViewList();
}

vsnViewPointDlg::~vsnViewPointDlg() {
}


/* methods */

bool vsnViewPointDlg::updateViewList() {
  if ( ! m_pViewNameLst ) return false;
  m_pViewNameLst->Clear();

  if ( ! p_viewFrame ) return false;
  map<string, vsnViewPoint>& viewPointLst = p_viewFrame->getViewPointList();
  map<string, vsnViewPoint>::iterator it;
  for ( it = viewPointLst.begin(); it != viewPointLst.end(); it++ ) {
    if ( it->first.empty() )
      m_pViewNameLst->Append(wxT(VFR_NONAME));
    else
      m_pViewNameLst->Append(vsnApp::ConvSysToWx(it->first));
  } // end of for(it)

  return true;
}


/* interface */

void vsnViewPointDlg::setRefViewFrame(vsnViewFrame* pvf) {
  if ( p_viewFrame == pvf ) return;
  p_viewFrame = pvf;

  (void)updateViewList();
}


/* event handler */

void vsnViewPointDlg::OnLeftViewBtn(wxCommandEvent& event) {
  if ( ! p_viewFrame ) return;
  vsnGfxView* pgv = p_viewFrame->getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();
  prot->roty(HALF_PI);
  if ( ! pgv->normalize(p_viewFrame->getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }

  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();
}

void vsnViewPointDlg::OnRightViewBtn(wxCommandEvent& event) {
  if ( ! p_viewFrame ) return;
  vsnGfxView* pgv = p_viewFrame->getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();
  prot->roty(-HALF_PI);
  if ( ! pgv->normalize(p_viewFrame->getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }

  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();
}

void vsnViewPointDlg::OnFrontViewBtn(wxCommandEvent& event) {
  if ( ! p_viewFrame ) return;
  vsnGfxView* pgv = p_viewFrame->getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();
  if ( ! pgv->normalize(p_viewFrame->getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }

  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();
}

void vsnViewPointDlg::OnBackViewBtn(wxCommandEvent& event) {
  if ( ! p_viewFrame ) return;
  vsnGfxView* pgv = p_viewFrame->getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();
  prot->roty((float)M_PI);
  if ( ! pgv->normalize(p_viewFrame->getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }

  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();
}

void vsnViewPointDlg::OnTopViewBtn(wxCommandEvent& event) {
  if ( ! p_viewFrame ) return;
  vsnGfxView* pgv = p_viewFrame->getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();
  prot->rotx(HALF_PI);
  if ( ! pgv->normalize(p_viewFrame->getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }

  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();
}

void vsnViewPointDlg::OnBottomViewBtn(wxCommandEvent& event) {
  if ( ! p_viewFrame ) return;
  vsnGfxView* pgv = p_viewFrame->getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();
  prot->rotx(-HALF_PI);
  if ( ! pgv->normalize(p_viewFrame->getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }

  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();
}

void vsnViewPointDlg::OnViewNameTxt(wxCommandEvent& event) {
  if ( ! m_pViewNameTxt || ! m_pViewNameLst ) return;
  if ( ! p_viewFrame ) return;

  wxString valStr = m_pViewNameTxt->GetValue();
  if ( valStr.IsEmpty() ) return;

  string vpname = vsnApp::ConvWxToSys(valStr);
  vsnViewPoint xfm = p_viewFrame->getViewPoint(vpname);
  if ( xfm.name == vpname ) { // vpname already exists
    wxString msg = wxT("View point named ") + valStr + wxT(" exists,\n")
      + wxT("Are you sure to override ?");
    wxMessageDialog dlg(this, msg, wxT("set view point"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  if ( ! p_viewFrame->addViewPoint(vsnApp::ConvWxToSys(valStr)) ) return;
  if ( ! updateViewList() ) return;
  m_pViewNameLst->SetSelection(m_pViewNameLst->FindString(valStr));
  p_viewFrame->refresh(true);
}

void vsnViewPointDlg::OnRegViewBtn(wxCommandEvent& event) {
  if ( ! m_pViewNameTxt || ! m_pViewNameLst ) return;
  if ( ! p_viewFrame ) return;

  wxString valStr = m_pViewNameTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    wxMessageDialog dlg(this, wxT("View point with no name"),
                        wxT("set view point failed"),
			wxOK|wxCENTRE|wxICON_ERROR);
    dlg.ShowModal();
    return;
  }

  string vpname = vsnApp::ConvWxToSys(valStr);
  vsnViewPoint xfm = p_viewFrame->getViewPoint(vpname);
  if ( xfm.name == vpname ) { // vpname already exists
    wxString msg = wxT("View point named ") + valStr + wxT(" exists,\n")
      + wxT("Are you sure to override ?");
    wxMessageDialog dlg(this, msg, wxT("set view point"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  if ( ! p_viewFrame->addViewPoint(vsnApp::ConvWxToSys(valStr)) ) return;
  if ( ! updateViewList() ) return;
  m_pViewNameLst->SetSelection(m_pViewNameLst->FindString(valStr));
  p_viewFrame->refresh(true);
}

void vsnViewPointDlg::OnViewNameLstClick(wxCommandEvent& event) {
  if ( ! m_pViewNameTxt || ! m_pViewNameLst ) return;

  wxString valStr = m_pViewNameLst->GetStringSelection();
  if ( valStr.IsEmpty() ) return;
  m_pViewNameTxt->SetValue(valStr);
}

void vsnViewPointDlg::OnViewNameLstDblClick(wxCommandEvent& event) {
  if ( ! m_pViewNameTxt || ! m_pViewNameLst ) return;
  if ( ! p_viewFrame ) return;

  wxString valStr = m_pViewNameLst->GetStringSelection();
  if ( valStr.IsEmpty() ) return;
  m_pViewNameTxt->SetValue(valStr);

  if ( ! p_viewFrame->updateViewPoint(vsnApp::ConvWxToSys(valStr)) )
    return;
  p_viewFrame->refresh(true);
}

void vsnViewPointDlg::OnResViewBtn(wxCommandEvent& event) {
  if ( ! m_pViewNameTxt || ! m_pViewNameLst ) return;
  if ( ! p_viewFrame ) return;

  wxString valStr = m_pViewNameLst->GetStringSelection();
  if ( valStr.IsEmpty() ) return;
  m_pViewNameTxt->SetValue(valStr);

  if ( ! p_viewFrame->updateViewPoint(vsnApp::ConvWxToSys(valStr)) )
    return;
  p_viewFrame->refresh(true);
}

void vsnViewPointDlg::OnDelViewBtn(wxCommandEvent& event) {
  if ( ! m_pViewNameLst ) return;
  if ( ! p_viewFrame ) return;

  wxString valStr = m_pViewNameLst->GetStringSelection();
  if ( valStr.IsEmpty() ) return;

  wxString msg = wxT("Are you sure to delete viewpoint ")
    + valStr + wxT(" ?");
  wxMessageDialog dlg(this, msg, wxT("delete view point"),
		      vsn_wxOK_CANCEL|wxICON_QUESTION);
  if ( dlg.ShowModal() != vsn_wxIDOK ) return;

  if ( p_viewFrame->delViewPoint(vsnApp::ConvWxToSys(valStr)) )
    (void)updateViewList();
}

void vsnViewPointDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnViewPointDlg::OnClose(wxCloseEvent& event) {
}

