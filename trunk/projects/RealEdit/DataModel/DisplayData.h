/*
 Cette classe sert à regrouper les primitives GL qui sont partagées par les
 différent widget (displayList, Texture et shaders). 
 */
#ifndef RealEdit_DisplayData_hh
#define RealEdit_DisplayData_hh

#include <qgl.h>
#include "Point.h"

namespace realEdit
{
using namespace realisim::math;

class DisplayData
{
public:
  DisplayData();
  ~DisplayData();
  
  void drawAxis() const;
  void drawBoundingBox(const Point3d& iMin, const Point3d& iMax) const;
  void drawCube() const;
private:
  mutable GLuint mAxisDisplayList;
  mutable GLuint mCubeDisplayList;
};

} //realEdit

#endif
