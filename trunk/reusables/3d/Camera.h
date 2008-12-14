/*
 *  Camera.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 06/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Realisim_E3d_Camera_hh
#define Realisim_E3d_Camera_hh

#include "Point.h"
#include "Vect.h"

namespace Realisim{ class Camera; }

class Realisim::Camera
{
public:
  
  enum Mode{ ORTHOGONAL = 0, PERSPECTIVE };

  Camera( Mode iMode = PERSPECTIVE );
  Camera( const Camera& iCam );
  virtual ~Camera();
  
  const Vector3d& getLat() const { return mLat; }
  const Point3d& getLook() const { return mLook; } 
  Mode getMode() const { return mMode; }
  const Point3d& getPos() const { return mPos; }
  const Vector3d& getUp() const { return mUp; }
  
  void lookAt();
  
  void move( int iDeltaX, int iDeltaY );
  
  Camera& operator=( const Camera& iCam );
  
  void projectionGL( int iWidth, int iHeight ) const;
  
  void set( const Point3d& iPos,
            const Point3d& iLook,
            const Vector3d& iUp );
  
  void setLook( const Point3d& iLook );
  void setMode( Mode iMode );
  void setPos( const Point3d& iPos );
  void setUp( const Vector3d& iUp );
  
protected:
private:
  void computeLatAndUp();
  
  Mode mMode;
  
  Point3d mPos;  //position de la caméra
  Vector3d mLat;  //vecteur latéral normalisé
  Point3d mLook;  //point visé
  Vector3d mUp;  //vecteur up normalisé
};

#endif
