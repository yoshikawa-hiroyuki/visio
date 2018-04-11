//
// vsnFrameBase
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

#include "vsnFrameBase.h"


//----------------------------------------------------------------
// class vsnFrameRefer
//----------------------------------------------------------------

vsnFrameRefer::vsnFrameRefer(vsnFrameBase* parent,
			     wxWindowID id, const wxString& title,
			     const wxPoint& pos, const wxSize& size,
			     long style)
 : wxDialog(parent, id, title, pos, size, style)
{
  assert(parent);
  parent->addRefer(this);
}

vsnFrameRefer::~vsnFrameRefer() {
  vsnFrameBase* parent = dynamic_cast<vsnFrameBase*>(GetParent());
  if ( parent ) parent->delRefer(this);
}


//----------------------------------------------------------------
// class vsnFrameBase
//----------------------------------------------------------------

vsnFrameBase::~vsnFrameBase() {
  std::set<vsnFrameRefer*> refLst = m_referLst;
  std::set<vsnFrameRefer*>::iterator it;
  for ( it = refLst.begin(); it != refLst.end(); it++ ) {
    if ( *it ) (*it)->Destroy();
  } // end of for(it)
}

bool vsnFrameBase::addRefer(vsnFrameRefer* pref) {
  if ( ! pref ) return false;
  if ( ! m_referLst.insert(pref).second ) return false;
  return true;
}

bool vsnFrameBase::delRefer(vsnFrameRefer* pref) {
  if ( ! pref ) return false;
  if ( m_referLst.erase(pref) < 1 ) return false;
  return true;
}

