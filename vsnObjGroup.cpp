//
// vsnObjGroup
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

#include "vsnObjGroup.h"

using namespace std;
using namespace VFR;
using namespace VSN;


bool vsnObjGroup::addData(vsnDataObj* pData) {
  if ( getNumData() < 1 ) {
    return (addChild(pData) == TRUE);
  }
  if ( getData(0)->getDataType() != pData->getDataType() )
    return false;
  return (addChild(pData) == TRUE);
}

bool vsnObjGroup::delData(vsnDataObj* pData) {
  return (remChild(pData) == TRUE);
}

