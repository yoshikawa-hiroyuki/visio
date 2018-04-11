//
// vsnDataObj
//
#ifndef _VSN_DATA_OBJ_H_
#define _VSN_DATA_OBJ_H_

#include "vfrGroup.h"
#include "vsnXForm.h"
#include "vsnDataSeqFiles.h"
#include "vsnMethodObj.h"
#include <deque>

namespace VSN {
  // numerical data type
  enum {DATA_None =0, DATA_Veclen =-1};
  typedef int WhichDataType;
};


//----------------------------------------------------------------
// class vsnDataObj
//----------------------------------------------------------------
class vsnDataObj
  : public vfrGroup, public vsnXForm, public vsnIoObject {
public:
  virtual ~vsnDataObj();

  // DataObj interface
  virtual std::string getDataType() const {return string();}
  virtual std::string getFilePath() const {return string();}
  virtual std::deque<std::string> getSupportMethodList() const {
    std::deque<std::string> r;
    return r;
  }
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL) =0;
  virtual bool reload();

  // XForm interface
  virtual void updateXForm();

  // serialize interface
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // multi-file interface
  virtual bool hasMultiFiles() const {return false;}
  virtual std::deque<std::string> getFileList() const {
    std::deque<std::string> r; r.push_back(getFilePath());
    return r;
  }

  // XML node interface
  void setXmlNode(xmlNodePtr xnp);
  xmlNodePtr getXmlNode() {return m_pXmlNode;}

  // MethodObj operation interface
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
				     const std::string& mname =std::string());
  virtual bool addMethod(vsnMethodObj* pMtd);
  virtual bool delMethod(vsnMethodObj* pMtd);
  size_t getNumMethod() const;
  vsnMethodObj* getMethod(const size_t n);
  vsnMethodObj* getMethod(const std::string& name);
  vsnMethodObj* getNextMethod(vsnMethodObj* pm);
  bool adjustMethodOrder(vsnMethodObj* beforeMtd, vsnMethodObj* afterMtd);

  // show mode interface
  bool getShow() const {return m_show;}
  void setShow(const bool mode);

  // status interface
  bool ready() const {return m_ready;}

  // sequence number
  unsigned long getDataSeq() const {return m_dataSeq;}

  // scene interface
  class vsnScene* getAncestorScene() const;

  // reloader interface
  class vsnDataReload* getReloader(const bool alc =true);

  // override vfrGroup method
  virtual void generateBbox();
  virtual void generateBbox(CES::Vec3<float>&) {generateBbox();}

protected:
  vsnDataObj(const std::string& name =std::string(VFR_NONAME));

  Bool addChild(vfrNode* a) {return vfrGroup::addChild(a);}
  Bool remChild(vfrNode* a) {return vfrGroup::remChild(a);}

  // XML tree data (last traversed)
  xmlNodePtr m_pXmlNode;

  // show mode
  bool m_show;

  // initialized flag
  volatile mutable bool m_ready;

  // reloader
  class vsnDataReload* m_pReloader;

  // sequence number
  unsigned long        m_dataSeq;
  static unsigned long s_dataSeq;
};


//----------------------------------------------------------------
// class vsnTSDataRefer
//   refers to vsnTimeSeriesDataIF
//----------------------------------------------------------------
class vsnTSDataRefer {
public:
  vsnTSDataRefer() : p_tsd(NULL) {}
  virtual ~vsnTSDataRefer();

  std::deque< std::pair<class vsnTimeSeriesDataIF*, class vsnScene*> >
    getTimeSeriesDataList(class vsnDataObj* pdata);

  class vsnTimeSeriesDataIF* getTimeSeriesData() {return p_tsd;}
  void setTimeSeriesData(class vsnTimeSeriesDataIF* tsd);
  bool setTimeSeriesData(class vsnDataObj* pdata, const std::string& tgtName);

  void noticeDie();
  virtual void noticeUpdate() =0;

  // XML util
  bool exportXMLCommand(const std::string& myName,
			std::ostream& os, const size_t ts =0) const;

protected:
  class vsnTimeSeriesDataIF* p_tsd;
};


//----------------------------------------------------------------
// class vsnTimeSeriesDataIF
//----------------------------------------------------------------
class vsnTimeSeriesDataIF {
public:
  struct StpUnit {
    int   step;
    float time;
    StpUnit(const int stp =0, const float tm =0.f) : step(stp), time(tm) {}
    StpUnit(const StpUnit& org) {*this = org;}
    void operator=(const StpUnit& org) {step = org.step; time = org.time;}
    bool operator==(const StpUnit& x) {return (step==x.step && time==x.time);}
    bool operator<(const StpUnit& x) {return (step < x.step);}
  };

  vsnTimeSeriesDataIF() : m_currentStepIdx(0) {}
  virtual ~vsnTimeSeriesDataIF();

  void updateTSDataRefers();

  virtual size_t getNumSteps() const {return 0;}
  virtual bool getStepIdx(const int tsNo, size_t& stpIdx) const
    {return false;}
  virtual bool getTimeStepNo(const size_t stpIdx, int& tsNo) const
    {return false;}
  virtual float getTime(const size_t stpIdx) const {return 0.f;}

  virtual bool setCurrentStepIdx(const size_t stpIdx);
  size_t getCurrentStepIdx() const {return m_currentStepIdx;}

  // TSDataRefer control
  void appendTSDataRefer(vsnTSDataRefer* tsdr);
  void removeTSDataRefer(vsnTSDataRefer* tsdr);

protected:
  size_t m_currentStepIdx;
  std::set<vsnTSDataRefer*> m_tsdrLst;

  friend class vsnDataObj;
};


//----------------------------------------------------------------
// class vsnNumericalDataIF
//----------------------------------------------------------------
class vsnNumericalDataIF {
public:
  vsnNumericalDataIF();

  virtual size_t getDataLen() const {return m_dataLen;}
  virtual bool getMinMax(const size_t n, float minmax[2]) const;
  virtual bool getVectorMinLen(const CES::Vec3<int>& vidx, float& vml);
  virtual bool getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml);
  virtual bool checkMinMax(const bool wholeStp =false,
			   const bool progress =true) = 0;

  virtual bool hasMinMax() const {return m_hasMinMax;}
  virtual bool parseMinMaxXml(xmlNodePtr xnp);
  virtual bool outputMinMaxXml(std::ostream& os, const size_t ts =0);

  size_t              m_dataLen;
  std::deque<float>   m_minVals; // [m_dataLen]
  std::deque<float>   m_maxVals; // [m_dataLen]
  float               m_minVecLen012;
  float               m_maxVecLen012;
  volatile bool       m_hasMinMax;
};

#endif // _VSN_DATA_OBJ_H_
