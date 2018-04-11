//
// vsnData_Shape
//
#ifndef _VSN_DATA_SHAPE_H_
#define _VSN_DATA_SHAPE_H_

#include "vsnDataObj.h"


class vsnData_Shape : public vsnDataObj, public vsnTimeSeriesDataIF {
public:
  enum ShapeType {
    ShapeNONE =0, ShapeOBJ, ShapeSLA, ShapeSLB, ShapeSLB_BE
  };

  vsnData_Shape(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_Shape();

  ShapeType getShapeType() const {return m_shapeType;}
  vfrNode* getShape() {return m_pShape;}
  std::deque<std::string> getSeqFilePathes() const;
  bool init(const std::deque<std::string>& path_lst);
  
  // from vsnTimeSeriesDataIF
  virtual size_t getNumSteps() const {return m_numStps;}
  virtual bool   getStepIdx(const int NSTEP, size_t& stp) const;
  virtual bool   getTimeStepNo(const size_t stpIdx, int& tsNo) const;
  virtual float  getTime(const size_t stp) const;
  virtual bool   setCurrentStepIdx(const size_t stpIdx);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("Shape");}
  virtual std::string getFilePath() const {return m_path;}
  virtual std::deque<std::string> getSupportMethodList() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
				     const std::string& mname =std::string());
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const {return m_seqPathes;}

  // from vsnIoObject
  virtual bool outputXML(std::ostream& os, const size_t ts =0);

  // override vsnDataObj method
  virtual void generateBbox() {}
  virtual void generateBbox(CES::Vec3<float>&) {}
  virtual bool updateBbox();

private:
  std::string         m_path;
  ShapeType           m_shapeType;
  vfrNode*            m_pShape;

  size_t              m_numStps;
  std::deque<std::string>
                      m_seqPathes; // [m_numSteps]
  std::deque<StpUnit> m_stpList;   // [m_numSteps]

  std::deque<std::string> setupLists(const std::deque<std::string>& path_lst);
  bool adjustStepList(const std::deque<std::string>& path_lst,
                      const std::string& baseDir, xmlNodePtr xnp);

  friend class vsnMethod_Shape_trias;
};

#endif // _VSN_DATA_SHAPE_H_
