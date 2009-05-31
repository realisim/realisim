/*
 Cette classe sert à regrouper les primitives GL qui sont partagées par les
 différent widget (displayList, Texture et shaders). 
 */
#ifndef RealEdit_DisplayData_hh
#define RealEdit_DisplayData_hh

#include "Primitives.h"

namespace RealEdit{ class DisplayData; }
namespace RealEdit{ class axis; }

class RealEdit::DisplayData
{
public:
  DisplayData();
  ~DisplayData();
  
  void drawAxis() const;
  Realisim::Axis& getAxis() {return mAxis;}
  void drawCube() const;
  Realisim::Cube& getCube() {return mCube;}
private:
  Realisim::Axis mAxis;
  Realisim::Cube mCube;
};

#endif
