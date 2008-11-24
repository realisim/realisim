/*
 *  Primitives3d.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 22/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Realisim_Primitive3d_hh
#define Realisim_Primitive3d_hh

namespace Realisim
{
  namespace Primitive3d 
  {
    class Primitive3dBase;
    class Cube;
  }
  
}

class Realisim::Primitive3d::Primitive3dBase
{
public:
  Primitive3dBase();
  virtual ~Primitive3dBase();
  
  virtual void draw( bool iPickingDraw = false ) const = 0;
  
protected:
private:
};

class Realisim::Primitive3d::Cube : public Primitive3dBase
{
public:
  Cube();
  ~Cube();
  
  virtual void draw( bool iPickingDraw = false ) const;
  
private:
  void initDisplayList() const;
  
  static int mDisplayList;
};

#endif


