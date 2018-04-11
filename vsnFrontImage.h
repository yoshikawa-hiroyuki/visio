//
// vsnFrontImage
//
#ifndef _VSN_FRONT_IMAGE_H_
#define _VSN_FRONT_IMAGE_H_

#include "vsnFrontObj.h"


class vsnFrontImage : public vsnFrontObj {
public:
  vsnFrontImage(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnFrontImage();

  // methods
  bool setImageData(const unsigned char* pdata,
		    const int w, const int h, const int d);

  // override vsnFrontObj::render method
  virtual void render(const Bool transpMode, vfrMaterialStack *mstk =NULL);  

protected:
  unsigned char* m_pImage;
  Point2 m_size;
  int m_depth;
  vector2 m_validArea;

  static void GetWrapSize(const int x, const int y, Point2& sz);
};

#endif // _VSN_FRONT_IMAGE_H_
