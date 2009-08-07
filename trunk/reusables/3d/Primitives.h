/*
 *  Created by Pierre-Olivier Beaudoin on 10/05/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 */
 
/*
  Cette classe donne un contrôle sur l'affichage des primitves GL. Grâce aux
  flags de contrôle, on peut contrôler la position (scene ou écran),
  l'orientation (scene ou écran) et le zoom (scene ou écran). La position
  écran est en pixel, l'orientation écran est tout simplement l'annulation de
  l'orientation scene et le zoom écran est l'annulation du zoom scene.
  
  Ex: Afficher un icone dans le coin supérieur gauche de l'écran. La primitive
  devra faire face a la camera en tout temps, et ne devra pas cahnger de
  dimension lorsque la caméra est zoomée.
  
  //position en pixel ( coin supérieur gauche )
  p->setPosition(5, 5, 0);
  //position viewport
  p->setDisplayFlag(pViewport);
  //annulation de l'orientation de scene, la primitive fera toujours face à la
  //camera
  p->setDisplayFlag(oViewport);
  //annulation du zoom de scene
  p->setDisplayFlag(zViewport);
  glScale(x,x,x); //x = taille en pixel.
  
 mPositionFlag: Flag de position qui permet de positionner la primitive en
   coordonné d'écran ou de scene. 
   -pViewport: positionne la primitive en coordonnée d'écran. La coordonnée z
     de Primitives::getPosition() est ignorée et x et y répresentent la coordonnée
     d'écran
   -pScene: positionne la primitive dans la scene a la position 
     Primitives::getPosition()
 
 mOrientationFlag :Flag d'orientation qui permet d'appliquer ou d'enlever 
 l'orientation de la primitive.
   -oViewport: enlève l'orientation de la primitive, donc celle-ci fait toujours
     face à la camera (billboard).
   -oScene: l'orientation de la primitive est conservée.
 
 mZoomFlag: flag de zoom qui permet d'appliquer ou pas le zoom.
   -zViewport: le zoom ne s'applique pas a la primitive. Dans ce mode, la taille
     de la primitive est en pixel.
   -zScene: le zoom s'applique. Dans ce mode, la taille de primitive est en 
     unitées gl.
*/
#ifndef Realisim_Primitives_hh
#define Realisim_Primitives_hh

#include "Point.h"
#include <qgl.h>
#include "Vect.h"
 
namespace realisim
{
namespace treeD
{
using namespace realisim::math;

class Primitives
{
public:
  Primitives();
  virtual ~Primitives(){;}
  
  enum Orientation
  {oViewport, oScene};
  enum Position
  {pViewport, pScene};
  enum Zoom
  {zViewport, zScene};

  virtual void draw() const = 0;
  GLuint getDisplayList() const;
  Orientation getOrientationFlag() const {return mOrientationFlag;}
  const Point3d& getPosition() const {return mPosition;}
  Position getPositionFlag() const {return mPositionFlag;}
  Zoom getZoomFlag() const {return mZoomFlag;}
  void setDisplayFlag(Orientation iF){mOrientationFlag = iF;}
  void setDisplayFlag(Position iF){mPositionFlag = iF;}
  void setDisplayFlag(Zoom iF){mZoomFlag = iF;}
  void setPosition(const Point3d& iP){mPosition = iP;}
  
protected:
  mutable GLuint mDisplayList;
  Orientation mOrientationFlag;
  Point3d mPosition;
  Position mPositionFlag;
  Zoom mZoomFlag;
};

//------------------------------------------------------------------------------
class Axis : public Primitives
{
public:
  Axis() : Primitives(){;}
  virtual ~Axis(){;}
  virtual void draw() const;
};

//------------------------------------------------------------------------------
class BoundingBox : public Primitives
{
public:
  explicit BoundingBox (const Point3d&, const Point3d&);
  virtual ~BoundingBox ();
  
  virtual void draw() const;
protected:
  Point3d mMin;
  Point3d mMax;
};

//------------------------------------------------------------------------------
class Cube : public Primitives
{
public:
  Cube() : Primitives(){;}
  virtual ~Cube(){;}
  virtual void draw() const;
};

} //namespace treeD
} //namepsace realisim

#endif

