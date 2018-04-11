//
// vsnData_OctVol
//
#ifndef _VSN_DATA_OCT_VOL_H_
#define _VSN_DATA_OCT_VOL_H_

#include "vsnDataObj.h"
#include "vsnOctTree.h"

// MHIR append begin
#include "vsnDataParallelFiles.h"
#include <vector>
// MHIR append end


//----------------------------------------------------------------
// class vsnData_OctVol
//----------------------------------------------------------------
class vsnData_OctVol
  : public vsnDataObj, public vsnTimeSeriesDataIF, public vsnNumericalDataIF {
public:
  vsnData_OctVol(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_OctVol();

  const CES::Vec3<size_t>& getRootDims() const {return m_otv.m_rootDims;}
  CES::Vec3<float> getPitch() const {return m_otv.m_pitch;}

  size_t getMaxLevel() const {return m_otv.m_maxLevel;}
  vsnOctTree::Node* getNode(const CES::Vec3<float>& pos, const int level =-1);
  bool interpolateData(const CES::Vec3<float>& pos, vsnOctTree::Node* pn,
                       const CES::Vec3<int>& didx, CES::Vec3<float>& dval) {
    return m_otv.InterpolateData(pos, pn, didx, dval);
  }

  const std::set<vsnOctTree::Node*>& getWholeNodeList() const {
    return m_otv.m_pWholeLst;}
  const std::deque<vsnOctTree::RootNode*>& getRootNodeList() const {
    return m_otv.m_pRootLst;}
  const std::set<vsnOctTree::Node*>& getLeafNodeList() const {
    return m_otv.m_pLeafLst;}

  const vsnOctTree* getOctTree() const {return &m_otv;}
  long getOctTreeUpdatedStamp() const {return m_otv_stamp;}

  bool getVectorMaxLen012(float& vml) const;
  std::deque<std::string> getSeqFilePathes() const;
  bool init(const std::deque<std::string>& path_lst);
  // MHIR append begin
  bool init(const std::vector<VSN::ParallelFileInfo>& path_lst);
  const std::vector<VSN::ParallelFileInfo>& getParaPathLst(void) const {
    return m_para_path_lst;}
  // MHIR append end

  // from vsnTimeSeriesDataIF
  virtual size_t getNumSteps() const {return m_numStps;}
  virtual bool   getStepIdx(const int NSTEP, size_t& stp) const;
  virtual bool   getTimeStepNo(const size_t stpIdx, int& tsNo) const;
  virtual float  getTime(const size_t stp) const;
  virtual bool   setCurrentStepIdx(const size_t stpIdx);

  // from vsnNumericalDataIF
  virtual size_t getDataLen() const {return m_otv.m_dataLen;}
  virtual bool getMinMax(const size_t didx, float minmax[2]) const;
  virtual bool getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml);
  virtual bool checkMinMax(const bool wholeStp =false,
			   const bool progress =true);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("OctVol");}
  virtual std::string getFilePath() const;
  virtual std::deque<std::string> getSupportMethodList() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
                                     const std::string& mname =std::string());
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const {return m_seqPathes;}

  // override vsnDataObj method
  virtual void generateBbox() {}
  virtual void generateBbox(CES::Vec3<float>&) {}
  virtual bool updateBbox();

private:
  vsnOctTree          m_otv;
  mutable long        m_otv_stamp;
  std::string         m_path;

  // MHIR append begin
  std::vector<VSN::ParallelFileInfo> m_para_path_lst;
  // MHIR append end

  size_t              m_numStps;
  std::deque<std::string>
                      m_seqPathes; // [m_numSteps]
  std::deque<StpUnit> m_stpList;   // [m_numSteps]

  std::deque<std::string> setupLists(const std::deque<std::string>& path_lst);
  bool adjustStepList(const std::deque<std::string>& path_lst,
                      const std::string& baseDir, xmlNodePtr xnp);

  static std::deque<std::string> s_methodList;
};

#endif // _VSN_OCT_VOL_H_
