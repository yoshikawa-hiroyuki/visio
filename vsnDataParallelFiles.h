//
// vsnDataParallelFiles
//
#ifndef _VSN_DATA_PARALLEL_FILES_H_
#define _VSN_DATA_PARALLEL_FILES_H_

#include "vsnIoObject.h"
#include <vector>

namespace VSN {
  // sequential file keyword
  static const char vsn_parallelFiles[] = "%%parallelFiles%%";
  typedef std::pair<std::string, bool> ParallelFileInfo;
};


//----------------------------------------------------------------
// class vsnDataParallelFiles
//----------------------------------------------------------------
class vsnDataParallelFiles : public vsnIoObject {
public:
  vsnDataParallelFiles() {}
  virtual ~vsnDataParallelFiles() {}

  bool parseXML_ParallelFiles(xmlNodePtr xnp,
			      std::string& baseDir,
			      std::vector<VSN::ParallelFileInfo>* dataFiles);
};


#endif // _VSN_DATA_PARALLEL_FILES_H_
