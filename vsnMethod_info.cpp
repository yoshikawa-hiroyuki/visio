//
// vsnMethod_info
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

#include "vsnMethod_info.h"
#include "vsnData_FdvStr.h"
#include "vsnData_OctVol.h"
#include "vsnData_Shape.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMPP_info::vsnMPP_info(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm), m_pInfoTxt(NULL)
{
  assert(parent);

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  m_pInfoTxt = new wxTextCtrl(this, -1, wxT("out of data\n"),
			      wxDefaultPosition, wxSize(-1, 250),
			      wxTE_MULTILINE|wxTE_READONLY);
  topsizer->Add(m_pInfoTxt, 0, wxEXPAND|wxALL, 3);

  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_info::~vsnMPP_info() {
}


/* interface */

bool vsnMPP_info::update() {
  if ( ! m_pInfoTxt ) return false;

  vsnMethod_info* pmtd = dynamic_cast<vsnMethod_info*>(p_method);
  if ( ! pmtd ) {
    m_pInfoTxt->SetValue(wxT("invalid method reference"));
    return false;
  }

  string repStr = pmtd->getRepStr();
  if ( repStr.empty() ) {
    m_pInfoTxt->SetValue(wxT("out of data"));
    return false;
  }

  m_pInfoTxt->SetValue(vsnApp::ConvSysToWx(repStr));
  return true;
}


//----------------------------------------------------------------
// class vsnMethod_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_info::vsnMethod_info(const string& name)
  : vsnMethodObj(name)
{
}

vsnMethod_info::~vsnMethod_info() {
}


/* vsnMethodObj methods */

bool vsnMethod_info::update(const bool force) {
  if ( ! updateRepStr() ) {
    updateUI();
    return false;
  }

  updateUI();
  return true;
}

vsnMethodPP* vsnMethod_info::getParamPanel(wxPanel* pp) {
  vsnMPP_info* pp_info = new vsnMPP_info(pp, this);
  if ( ! pp_info ) return NULL;
  return pp_info;
}


/* serialize : from vsnIoObject */

bool vsnMethod_info::parseXML(xmlNodePtr xnp) {
  if ( ! vsnMethodObj::parseXML(xnp) ) return false;
  return true;
}

bool vsnMethod_info::outputXML(std::ostream& os, const size_t ts) {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  // output
  os << idts << "<method type=\"" << getMethodType() << "\"";
  if ( !_name.empty() && _name != string(VFR_NONAME) )
    os << " name=\"" << _name << "\"";
  os << " >" << endl;

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts +2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // no original params

  os << idts << "</method>" << endl;

  return ret;
}
