//
// vsnData_ShapeLOD
//
#ifndef _VSN_DATA_SHAPE_LOD_H_
#define _VSN_DATA_SHAPE_LOD_H_

#include "vsnData_Shape.h"


class vsnData_ShapeLOD : public vsnDataObj {
public:
  enum ShapeType {
    ShapeNONE =0, ShapeOBJ, ShapeSLA, ShapeSLB, ShapeSLB_BE
  };

  vsnData_ShapeLOD(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_ShapeLOD();

  bool init(const std::deque<std::string>& path_lst);
  
  // for vsnLOD
  size_t getNumLevels() const;
  ShapeType getShapeType(const int level) const;
  vfrNode* getShape(const int level);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("ShapeLOD");}
  virtual std::string getFilePath() const {return m_path;}
  virtual std::deque<std::string> getSupportMethodList() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
				     const std::string& mname =std::string());
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const {return m_lodPathes;}

  // override vsnDataObj method
  virtual void generateBbox() {}
  virtual void generateBbox(CES::Vec3<float>&) {}
  virtual bool updateBbox();

private:
  std::string             m_path;
  std::deque<std::string> m_lodPathes;
  std::deque<ShapeType>   m_lodShapeTypes;
  vfrGroup*               m_pShapes;

  bool parseXML_LodFiles(xmlNodePtr xnp, std::string& baseDir,
			 std::deque<std::string>& lodFiles);
    
  //friend class vsnMethod_ShapeLOD_trias;
};

#endif // _VSN_DATA_SHAPE_H_
