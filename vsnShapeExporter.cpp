//
// vsnShapeExporter
//
#include "vsnShapeExporter.h"
#include "vfrTriangles.h"
#include "utilPath.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;


//--------------------------------------------------------------------------
// Triangles exporters
//--------------------------------------------------------------------------

// Wavefront OBJ exporter
bool
VSN::SaveWfo(vfrTriangles* pTria, const std::string& path, const bool outNorm)
{
  if ( path.empty() ) return false;
  if ( ! pTria ) return false;

  ofstream os(path.c_str());
  if ( ! os ) return false;

  register int j, vc, nc, nV, nN, nF;

  nV = pTria->getNumVerts();
  nF = nV / 3; nV = nF * 3;
  nN = pTria->getNumNormals();
  if ( nF < 1 || nN < nF ) {os.close(); return true;}
  vector3* pv = pTria->getVerts();
  vector3* pn = pTria->getNormals();
  register AppearType nmlMode = pTria->getNormalMode();

  // v
  for ( j = 0; j < nV; j++ ) {
    os << "v " << pv[j][0] << " " << pv[j][1] << " " << pv[j][2] << endl;
  } // end of for(j)
  
  // vn
  if ( outNorm && (nmlMode == AT_PER_FACE || nmlMode == AT_PER_VERTEX) ) {
    for ( j = 0; j < nN; j++ ) {
      os << "vn " << pn[j][0] << " " << pn[j][1] << " " << pn[j][2] << endl;
    } // end of for(j)
  }

  // f
  vc = nc = 1;
  if ( outNorm && (nmlMode == AT_PER_FACE || nmlMode == AT_PER_VERTEX) ) {
    if ( nmlMode == AT_PER_FACE ) {
      for ( j = 0; j < nF; j++ ) {
        os << "f " << vc   << "//" << nc
           << " "  << vc+1 << "//" << nc
           << " "  << vc+2 << "//" << nc << endl;
        vc += 3; nc += 1;
      } // end of for(j)
    }
    else if ( nmlMode == AT_PER_VERTEX ) {
      for ( j = 0; j < nF; j++ ) {
        os << "f " << vc   << "//" << nc
           << " "  << vc+1 << "//" << nc+1
           << " "  << vc+2 << "//" << nc+2 << endl;
        vc += 3; nc += 3;
      } // end of for(j)
    }
  }
  else {
    for ( j = 0; j < nF; j++ ) {
      os << "f " << vc
         << " "  << vc+1
         << " "  << vc+2 << endl;
      vc += 3;
    } // end of for(j)
  }

  os.close();
  return true;
}


// STL Ascii exporter
bool VSN::SaveSla(vfrTriangles* pTria, const std::string& path) {
  if ( path.empty() ) return false;
  if ( ! pTria ) return false;
  vfrTriangles* ptri = pTria;

  ofstream os(path.c_str());
  if ( ! os ) return false;

  register int j, k, nF;
  nF = ptri->getNumVerts() / 3;
  if ( ! ptri || nF < 1 ) {os.close(); return true;}
  if ( ptri->getNumNormals() < nF ) {os.close(); return true;}
  vector3* pv = ptri->getVerts();
  vector3* pn = ptri->getNormals();
  if ( ! pv || ! pn ) {os.close(); return true;}
  register AppearType nmlMode = ptri->getNormalMode();

  os << "solid " << CES::BaseName(path, "stl") << endl;
  for ( j = 0; j < nF; j++ ) {
    k = j*3;
    switch (  nmlMode ) {
    case AT_PER_FACE:
      os << "  facet normal " << pn[j][0] << " " << pn[j][1] << " "
         << pn[j][2] << endl;
      break;
    case AT_PER_VERTEX:
      {
        vector3 nv = {pn[k][0] +pn[k+1][0] +pn[k+2][0],
                      pn[k][1] +pn[k+1][1] +pn[k+2][1],
                      pn[k][2] +pn[k+1][2] +pn[k+2][2]};
        nv[0] /= 3.f; nv[1] /= 3.f; nv[2] /= 3.f;
        os << "  facet normal " << nv[0] << " " << nv[1] << " "
           << nv[2] << endl;
      }
      break;
    default:
      os << "  facet normal " << pn[0][0] << " " << pn[0][1] << " "
         << pn[0][2] << endl;
    } // end of switch

    os << "  outer " << "loop" << endl;
    os << "    vertex " << pv[k][0] << " " << pv[k][1] << " "
       << pv[k][2] << endl;
    os << "    vertex " << pv[k+1][0] << " " << pv[k+1][1] << " "
       << pv[k+1][2] << endl;
    os << "    vertex " << pv[k+2][0] << " " << pv[k+2][1] << " "
       << pv[k+2][2] << endl;
    os << "  endloop" << endl;
    os << "  endfacet" << endl;
  } // end of for(j)
  os << "endsolid " << CES::BaseName(path, "stl") << endl;

  os.close();
  return true;
}


// STL Binary exporter
bool VSN::SaveSlb(vfrTriangles* pTria, const std::string& path) {
  if ( path.empty() ) return false;
  if ( ! pTria ) return false;
  vfrTriangles* ptri = pTria;

  FILE* ofp = fopen(path.c_str(), "wb");
  if ( ! ofp ) return false;
  char buf[80], buf2[2] = {0, 0};
  memset(buf, 0, 80);
  sprintf(buf, "%s", CES::BaseName(path, "stl").c_str());
  fwrite(buf, 1, 80, ofp);

  register int j, k, nF;
  unsigned short padding = 0;
  nF = ptri->getNumVerts() / 3;
  if ( ! ptri || nF < 1 ) {fclose(ofp); return true;}
  if ( ptri->getNumNormals() < nF ) {fclose(ofp); return true;}
  fwrite(&nF, 4, 1, ofp);

  vector3* pv = ptri->getVerts();
  vector3* pn = ptri->getNormals();
  if ( ! pv || ! pn ) {fclose(ofp); return true;}
  register AppearType nmlMode = ptri->getNormalMode();

  for ( j = 0; j < nF; j++ ) {
    k = j*3;
    switch (  nmlMode ) {
    case AT_PER_FACE:
      fwrite(pn[j], 4, 3, ofp);
      break;
    case AT_PER_VERTEX:
      {
        vector3 nv = {pn[k][0] +pn[k+1][0] +pn[k+2][0],
                      pn[k][1] +pn[k+1][1] +pn[k+2][1],
                      pn[k][2] +pn[k+1][2] +pn[k+2][2]};
        nv[0] /= 3.f; nv[1] /= 3.f; nv[2] /= 3.f;
        fwrite(nv, 4, 3, ofp);
      }
      break;
    default:
      fwrite(pn[0], 4, 3, ofp);
    } // end of switch

    fwrite(pv[k  ], 4, 3, ofp);
    fwrite(pv[k+1], 4, 3, ofp);
    fwrite(pv[k+2], 4, 3, ofp);

    fwrite(buf2, 2, 1, ofp);
  } // end of for(j)

  fclose(ofp);
  return true;
}


//--------------------------------------------------------------------------
// PtSet exporters
//--------------------------------------------------------------------------

// Scat exporter
bool VSN::SaveScat(vsnPtSet* pPts, const std::string& path) {
  if ( path.empty() ) return false;
  if ( ! pPts ) return false;

  ofstream os(path.c_str());
  if ( ! os ) return false;

  register int nV, nC, nI, dlen;
  nV = pPts->getNumVerts();
  nC = pPts->getNumColors();
  nI = pPts->getNumIndices();
  if ( nV < 1 ) {os.close(); return true;}

  vector3* pV = pPts->getVerts();
  vector4* pC = pPts->getColors();
  int* pI = pPts->getIndices();

  VFR::AppearType colorMode = pPts->getColorMode();
  if ( colorMode == AT_PER_FACE || colorMode == AT_PER_VERTEX )
    dlen = 4; // RGBA
  else
    dlen = 1; // dummy, all zero
  float data[4] = {0.f, 0.f, 0.f, 0.f};

  // output header
  os << nV << " " << dlen << endl;

  // output vertices
  register char c = ' ';
  if ( nI > 0 ) {
    register int i, l, index = 0;
    for ( l = 0; l < nI; l++ ) {
      if ( colorMode == AT_PER_FACE && l < nC )
	memcpy(data, pC[l], sizeof(vector4));
      register int lindex;
      for ( lindex = 0; lindex < pI[l]; lindex++ ) {
	if ( index < nV ) {
	  if ( colorMode == AT_PER_VERTEX && index < nC )
	    memcpy(data, pC[index], sizeof(vector4));
	  os << pV[index][0] << c << pV[index][1] << c << pV[index][2];
	  for ( i = 0; i < dlen; i++ )
	    os << c << data[i];
	  os << endl;
	  index ++;
	}
      } // end of for(lindex)
    } // end of for(l)
  }
  else {
    register int i, index;
    for ( index = 0; index < nV; index++ ) {
      if ( colorMode == AT_PER_VERTEX && index < nC )
	memcpy(data, pC[index], sizeof(vector4));
      os << pV[index][0] << c << pV[index][1] << c << pV[index][2];
      for ( i = 0; i < dlen; i++ )
	os << c << data[i];
      os << endl;
      index ++;
    } // end of for(index)
  }

  os.close();
  return true;
}


// Scat exporter (As Lines)
bool VSN::SaveScatAsLines(vsnPtSet* pPts, const std::string& path) {
  if ( path.empty() ) return false;
  if ( ! pPts ) return false;

  ofstream os(path.c_str());
  if ( ! os ) return false;

  register int nV, nC, nN, nI, dlen;
  nV = pPts->getNumVerts();
  nC = pPts->getNumColors();
  nN = pPts->getNumNormals();
  nI = pPts->getNumIndices();
  vector3* pV = pPts->getVerts();
  vector4* pC = pPts->getColors();
  vector3* pN = pPts->getNormals();
  int* pI = pPts->getIndices();

  register int i, nVerts = 0;
  if ( nI > 0 ) {
    for ( i = 0; i < nI; i++ )
      nVerts += pI[i] -1; 
  } else {
    nVerts = nV -1;
  }
  if ( nVerts < 1 ) {os.close(); return true;}

  VFR::AppearType colorMode = pPts->getColorMode();
  dlen = 0;
  if ( colorMode == AT_PER_FACE || colorMode == AT_PER_VERTEX ) {
    dlen += 3; // RGB
    if ( nN > 0 ) dlen += 1; // W
  }
  float rgbw[4] = {0.f, 0.f, 0.f, 0.f};

  // output header
  os << nVerts << " " << dlen + 3 << endl;

  // output vertices
  register char c = ' ';
  if ( nI > 0 ) {
    register int index = 0;
    for ( register int l = 0; l < nI; l++ ) {
      if ( colorMode == AT_PER_FACE && l < nC )
	memcpy(rgbw, pC[l], sizeof(vector3));
      if ( l < nN )
	rgbw[3] = pN[l][0];
      register int lindex;
      for ( lindex = 0; lindex < pI[l] -1; lindex++ ) {
        if ( index < nV ) {
	  if ( colorMode == AT_PER_VERTEX && index < nC )
	    memcpy(rgbw, pC[index], sizeof(vector3));
	  os << pV[index][0] << c << pV[index][1] << c << pV[index][2] << c;
	  if ( colorMode == AT_PER_VERTEX && index+1 < nC )
	    memcpy(rgbw, pC[index+1], sizeof(vector3));
	  os << pV[index+1][0] << c << pV[index+1][1] << c << pV[index+1][2];
	  for ( i = 0; i < dlen; i++ )
	    os << c << rgbw[i];
	  os << endl;
	  index ++;
	}
      } // end of for(lindex)
      index ++;
    } // end of for(l)
  }
  else {
    register int index;
    for ( index = 0; index < nVerts; index++ ) {
      if ( colorMode == AT_PER_VERTEX && index < nC )
	memcpy(rgbw, pC[index], sizeof(vector3));
      os << pV[index][0] << c << pV[index][1] << c << pV[index][2] << c;
      if ( colorMode == AT_PER_VERTEX && index+1 < nC )
	memcpy(rgbw, pC[index+1], sizeof(vector3));
      os << pV[index+1][0] << c << pV[index+1][1] << c << pV[index+1][2];
      for ( i = 0; i < dlen; i++ )
	os << c << rgbw[i];
      os << endl;
      index ++;
    } // end of for(index)
  }

  os.close();
  return true;
}


//--------------------------------------------------------------------------
// PtSet exporters
//--------------------------------------------------------------------------

// TimeStep path converter
std::string VSN::ConvTimeStepPath(const std::string& path,
				  vsnDataObj* pData, const bool useIdx) {
  string exPath;
  if ( path.empty() ) return exPath;
  exPath = path;

  vsnTimeSeriesDataIF* pTsData = dynamic_cast<vsnTimeSeriesDataIF*>(pData);
  if ( ! pTsData ) return exPath;

  string sfPrefix, sfPostfix; int sfIdxPrec;
  sfPrefix = sfPostfix = ""; sfIdxPrec = 0;
  istringstream strBuff(path); char c;
  while ( strBuff.get(c) && c != '#' )
    sfPrefix.push_back(c);
  if ( ! strBuff.eof() ) sfIdxPrec++;
  while ( strBuff.get(c) && c == '#' )
    sfIdxPrec++;
  if ( ! strBuff.eof() ) sfPostfix.push_back(c);
  while ( strBuff.get(c) )
    sfPostfix.push_back(c);
  size_t tsIdx = pTsData->getCurrentStepIdx();
  int tsNo; pTsData->getTimeStepNo(tsIdx, tsNo);
  exPath = sfPrefix;
  if ( sfIdxPrec > 0 ) {
    char fmtBuff[16], idxBuff[64];
    sprintf(fmtBuff, "%%0%dd", sfIdxPrec);
    sprintf(idxBuff, fmtBuff, tsNo);
    exPath += idxBuff;
  }
  exPath += sfPostfix;

  return exPath;
}
