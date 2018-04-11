//
// vsnShapeExporter
//
#ifndef _VSN_SHAPE_EXPORTER_H_
#define _VSN_SHAPE_EXPORTER_H_

#include <string>
#include "vfrTriangles.h"
#include "vsnPtSet.h"
#include "vsnDataObj.h"

namespace VSN {
  // Triangles exporters
  bool SaveWfo(vfrTriangles* pTria, const std::string& path,
	       const bool outNorm =true);
  bool SaveSla(vfrTriangles* pTria, const std::string& path);
  bool SaveSlb(vfrTriangles* pTria, const std::string& path);

  // PtSet exporters
  bool SaveScat(vsnPtSet* pPts, const std::string& path);
  bool SaveScatAsLines(vsnPtSet* pPts, const std::string& path);

  // TimeStep path converter
  std::string ConvTimeStepPath(const std::string& path,
			       vsnDataObj* pData,
			       const bool useIdx =false);
};

#endif // _VSN_SHAPE_EXPORTER_H_
