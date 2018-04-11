//
// vsnData_TriaCells
//
#ifndef _VSN_DATA_TRIACELLS_H_
#define _VSN_DATA_TRIACELLS_H_

#include "vsnDataObj.h"


class vsnData_TriaCells
  : public vsnDataObj,
    public vsnTimeSeriesDataIF,
    public vsnNumericalDataIF
{
public:
  enum TriaCellsType {
    TriaCellsNONE =0, TriaCellsVTK
  };

  vsnData_TriaCells(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_TriaCells();

  TriaCellsType getTriaCellsType() const {return m_triaCellsType;}
  float* getData() {return m_pData;}
  std::deque<std::string> getSeqFilePathes() const;
  bool init(const std::deque<std::string>& path_lst);
  size_t getMaxNumPts() const {return m_maxNumPts;}
  size_t getMaxNumCells() const {return m_maxNumCells;}

  // from vsnTimeSeriesDataIF
  virtual size_t getNumSteps() const {return m_numStps;}
  virtual bool   getStepIdx(const int NSTEP, size_t& stp) const;
  virtual bool   getTimeStepNo(const size_t stpIdx, int& tsNo) const;
  virtual float  getTime(const size_t stp) const;
  virtual bool   setCurrentStepIdx(const size_t stpIdx);

  // from vsnNumericalDataIF
  virtual bool getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml);
  virtual bool checkMinMax(const bool wholeStp =false,
			   const bool progress =true);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("TriaCells");}
  virtual std::string getFilePath() const {return m_path;}
  virtual std::deque<std::string> getSupportMethodList() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
				     const std::string& mname =std::string());
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const {return m_seqPathes;}

  // override Bbox methods
  virtual void generateBbox() {vfrNode::generateBbox();}
  virtual void generateBbox(CES::Vec3<float>& p) {vfrNode::generateBbox(p);}

private:
  std::string         m_path;
  TriaCellsType       m_triaCellsType;
  size_t              m_maxNumPts;
  size_t              m_maxNumCells;
  float*              m_pData;

  size_t              m_numStps;
  std::deque<std::string>
                      m_seqPathes; // [m_numSteps]
  std::deque<StpUnit> m_stpList;   // [m_numSteps]

  std::deque<std::string> setupLists(const std::deque<std::string>& path_lst);
  bool adjustStepList(const std::deque<std::string>& path_lst,
                      const std::string& baseDir, xmlNodePtr xnp);

  TriaCellsType checkType(const std::string& path, std::string& path_bd) const;
  bool readVTK(const std::string& path);
};

#endif // _VSN_DATA_TRIACELLS_H_
