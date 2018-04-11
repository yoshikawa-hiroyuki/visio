//
// vsnPtSet
//
#ifndef _VSN_PT_SET_H_
#define _VSN_PT_SET_H_

#include "vfrNode.h"


class vsnPtSet : public vfrNode {
public:
  vsnPtSet(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnPtSet();

protected:
  virtual void renderPoint();
  virtual void renderWire();
};

#endif // _VSN_PT_SET_H_
