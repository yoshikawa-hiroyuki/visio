//
// vsnDataSeqFiles
//
#ifndef _VSN_DATA_SEQ_FILES_H_
#define _VSN_DATA_SEQ_FILES_H_

#include "vsnIoObject.h"
#include <deque>

namespace VSN {
  // sequential file keyword
  static const char vsn_seqfile[] = "%%seqfile%%";
};


//----------------------------------------------------------------
// class vsnDataSeqFiles
//----------------------------------------------------------------
class vsnDataSeqFiles : public vsnIoObject {
public:
  vsnDataSeqFiles() {}
  virtual ~vsnDataSeqFiles() {}

  bool parseXML_SeqFiles(xmlNodePtr xnp,
			 std::string& baseDir,
			 std::deque<std::string>& seqFiles);
};

#endif // _VSN_DATA_SEQ_FILES_H_


