//
// vsnObjGroup
//
#ifndef _VSN_OBJ_GROUP_H_
#define _VSN_OBJ_GROUP_H_

#include "vfrGroup.h"
#include "vsnIoObject.h"
#include "vsnDataObj.h"


class vsnObjGroup : public vfrGroup, public vsnIoObject {
public:
  vsnObjGroup(const std::string& name =std::string(VFR_NONAME))
    : vfrGroup(name, FALSE), vsnIoObject() {
    alcMaterial();
  }

  virtual ~vsnObjGroup() {
    register int n = getNumChildren();
    while ( n > 0 ) {
      vfrNode* p = getChild(n - 1);
      if ( p ) delete p;
      n = getNumChildren();
    }
  }

  bool addData(vsnDataObj* pData);
  bool delData(vsnDataObj* pData);

  size_t getNumData() const {return (size_t)getNumChildren();}
  vsnDataObj* getData(const size_t n) {
    return dynamic_cast<vsnDataObj*>(getChild((const int)n));
  }
  vsnDataObj* getData(const std::string& name) {
    return dynamic_cast<vsnDataObj*>(getNode(name));
  }

protected:
  Bool addChild(vfrNode* a) {return vfrGroup::addChild(a);}
  Bool remChild(vfrNode* a) {return vfrGroup::remChild(a);}
};

#endif // _VSN_OBJ_GROUP_H_
