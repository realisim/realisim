/*
 Cette classe sert à regrouper les primitives GL qui sont partagées par les
 différent widget (displayList, Texture et shaders). 
 */
#ifndef RealEdit_DisplayData_hh
#define RealEdit_DisplayData_hh

#include "Primitives.h"

namespace realEdit
{
using namespace realisim::treeD;

class DisplayData
{
public:
  DisplayData();
  ~DisplayData();
  
  void drawAxis() const;
  Axis& getAxis() {return mAxis;}
  void drawCube() const;
  Cube& getCube() {return mCube;}
private:
  Axis mAxis;
  Cube mCube;
};

} //realEdit

#endif
