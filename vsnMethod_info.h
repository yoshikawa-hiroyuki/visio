//
// vsnMethod_info
//
#ifndef _VSN_METHOD_INFO_H_
#define _VSN_METHOD_INFO_H_

#include "wx/defs.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"


//----------------------------------------------------------------
// class vsnMPP_info
//----------------------------------------------------------------
class vsnMPP_info : public vsnMethodPP {
public:
  vsnMPP_info(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_info();

  // interface
  virtual bool update();

  // event handler
  /* no handlers */

private:
  wxTextCtrl* m_pInfoTxt;

  //DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_info
//----------------------------------------------------------------
class vsnMethod_info : public vsnMethodObj {
public:
  vsnMethod_info(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnMethod_info();

  // report string
  std::string getRepStr() const {return m_repStr;}

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {return std::string("info");}
  virtual bool hasBbox() const {return false;}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);

protected:
  virtual bool updateRepStr() =0;

  std::string  m_repStr;
};

#endif // _VSN_METHOD_INFO_H_
