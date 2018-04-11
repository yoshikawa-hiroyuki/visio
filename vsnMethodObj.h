//
// vsnMethodObj
//
#ifndef _VSN_METHOD_OBJ_H_
#define _VSN_METHOD_OBJ_H_

#include "wx/defs.h"
#include "wx/dcclient.h"
#include "wx/panel.h"

#include "vfrGroup.h"
#include "vsnLut.h"
#include "vsnIoObject.h"
#include "vsnAnchor.h"
#include <set>
#include <deque>

class vsnDataObj;

#define OPAC_THRESH 0.991f


//----------------------------------------------------------------
// class vsnMethodPP
//   parameter-panel for vsnMethodObj
//----------------------------------------------------------------
class vsnMethodPP : public wxPanel {
public:
  vsnMethodPP(wxPanel* parent, class vsnMethodObj* pm);
  virtual ~vsnMethodPP();

  class vsnUiView* getUiView();
  class vsnMethodParamCanvas* getMPC();
  void methodDeleted(class vsnMethodObj* pm);

  // interface
  virtual bool update() =0;
  virtual void settlement() {}

protected:
  class vsnMethodObj* p_method;

  // methods
  void addTo(wxPanel* parent);
};


//----------------------------------------------------------------
// class vsnMethodLutRefer
//----------------------------------------------------------------
class vsnMethodLutRefer {
public:
  vsnMethodLutRefer(class vsnMethodObj* pm);
  virtual ~vsnMethodLutRefer();

  // interface
  virtual bool updateLut() =0;

  // methods
  void methodDeleted(class vsnMethodObj* pm);
  bool setMethodObj(class vsnMethodObj* pm);
  class vsnMethodObj* getMethodObj() {return p_method;}

protected:
  class vsnMethodObj* p_method;
};


//----------------------------------------------------------------
// class vsnMethodObj
//----------------------------------------------------------------
class vsnMethodObj
  : public vfrGroup, public vsnIoObject, public vsnAnchor
{
public:
  virtual ~vsnMethodObj();

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool commandXML(xmlNodePtr xnp);
  // no outputXML() in this class

  // MethodObj interface
  bool isAcceptableData(const std::string& dtype) const;
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; return r;
  }
  virtual std::string getMethodType() const {return std::string();}
  virtual bool hasBbox() const {return true;}
  virtual bool canLighting() const {return true;}
  virtual bool update(const bool force =true) =0;
  virtual void reloaded() {if ( update() ) updateUI();}

  // parameter-panel interface
  virtual vsnMethodPP* getParamPanel(wxPanel* pp) {return NULL;}
  void appendMPP(vsnMethodPP* pmpp);
  void removeMPP(vsnMethodPP* pmpp);
  void updateUI();

  // LutRefer interface
  void appendLutRefer(vsnMethodLutRefer* pmlr);
  void removeLutRefer(vsnMethodLutRefer* pmlr);

  // set reference DataObj, be called from DataObj::addMethod()
  vsnDataObj* getRefData() {return p_refData;}
  virtual void setRefData(vsnDataObj* prd);

  // show mode interface
  bool getShow() const {return m_show;}
  virtual void setShow(const bool mode);

  // lighting mode interface
  virtual bool getLighting() const;
  virtual void setLighting(const bool mode);

  // anti-alias
  bool getAntiAliasMode() const {return m_antiAlias;}
  virtual bool setAntiAliasMode(const bool aam);

  // color/lut interface
  virtual void setBaseColor(const vector4 cv);
  void getBaseColor(vector4 cv) const;
  virtual void setHilight(const float hl);
  float getHilight() const {return m_hilight;}
  virtual void setUseLut(const bool ulm);
  bool getUseLut() const {return m_useLut;}
  virtual void setLut(const vsnLut& ol);
  vsnLut getLut() const {return m_lut;}

  // colorbar for display
  class vsnColorBar* getColorBar();
  bool showColorBar(const bool scbm);

  // sequence number
  unsigned long getMethodSeq() const {return m_mtdSeq;}

  // XML util
  bool exportXMLNode(std::ostream& os, const size_t ts =0) const;

protected:
  vsnMethodObj(const std::string& name =std::string(VFR_NONAME));

  Bool addChild(vfrNode* a) {return vfrGroup::addChild(a);}
  Bool remChild(vfrNode* a) {return vfrGroup::remChild(a);}

  // modes
  bool m_show;
  VFR::RenderType m_showType;

  // color/lut
  vector4 m_colour;
  float   m_hilight;
  bool    m_antiAlias;
  vsnLut  m_lut;
  bool    m_useLut;
  class vsnColorBar*
          m_pColorBar;

  // reference to DataObj
  vsnDataObj* p_refData;

  // reference list to MethodPP
  std::set<vsnMethodPP*> m_mppLst;

  // reference list to MethodLutRefer
  std::set<vsnMethodLutRefer*> m_mlrLst;

  // sequence number
  unsigned long        m_mtdSeq;
  static unsigned long s_mtdSeq;
};


//----------------------------------------------------------------
// class vsnTimeSeriesMethodIF
//----------------------------------------------------------------
class vsnTimeSeriesMethodIF {
public:
  vsnTimeSeriesMethodIF() : m_updatedStp(-1), m_requestedStp(-1) {}

  virtual bool
  updateStep(const int stp,
	     const bool force =true, const bool cascade =true) =0;

  int getUpdatedStep() const {return m_updatedStp;}

protected:
  int  m_updatedStp;   // in stpIdx
  int  m_requestedStp; // in stpIdx
};

#endif // _VSN_METHOD_OBJ_H_
