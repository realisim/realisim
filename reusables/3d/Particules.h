/* Particules.h */

#ifndef Realisim_TreeD_Particules_hh
#define Realisim_TreeD_Particules_hh

#include "3d/Texture.h"
#include "math/Vect.h"
#include "math/Point.h"
#include "QColor"
#include "QTime"
#include <time.h>
#include <vector>

/*
	Cette classe permet de manipuler une source de particule et [0, n] attrateurs.
  Les particules répondent aux forces des attracteurs.
  
  Le comportement des sources est modificable par le biais des méthodes 
  set...(). Les sources peuvent être de type:
  	- tOmnidirectional: les particules sont émises dans toutes les directions.
    - tDirectional: les particules sont émises dans une seule direction.
    - tCone: les particules sont émises sur la surface du disque de rayon
      spécifié par get/setRadius. Ce disque est perpendiculaire à la direction
      spécifiée par get/setDirection() et est situé a 1 unité de distance de la
      position spécifiée par get/setPosition.
    - tDisk: les particules sont émises en direction spécifiées par 
    get/setDirection() a partir de la surface du disque orienté dans la même 
    direction en position spécifiée par get/setDirection() et en position
    spécifiée par get/setPosition;

	structures:
  	particule:
    	v: vélocité (le vecteur represente, l'orientation et la magnitude 
        représente la vitesse )
      p: position
      t: temps de vie restant en ms.

		attractor:
    	t: le type
      	- atPositional: applique une force sur toutes les particules vers
          le point spécifiée par get/setAttractorPosition().
        - atGlobal: applique une force spécifiée par get/setAttractorForce() 
          de magnitude m sur toutes les particules .
  		p: la position
    	f: le vecteur de force (direction)
    	m: magnitude de la force. On utilise la magnitude séparément dans le 
        cas des attracteurs parce que ceux qui sont de type tOmnidirectional
        n'ont pas de vecteur de force, puisque cette force s'applique dans
        toutes les directions.
  membres:
  
*/

namespace realisim
{
namespace treeD
{
using namespace math;

class Particules
{
public:
  enum attractorType{ atPositional, atGlobal };
	enum type{ tOmniDirectional, tDirectional, tCone, tDisk };

  Particules();
  Particules( const Particules& );
  Particules& operator=( const Particules& );
  virtual ~Particules();
  
	virtual void addAttractor( attractorType, Point3d, Vector3d, double );
  virtual void draw() const;
  virtual void enableDecay( bool );
  virtual const Vector3d& getAttractorForce( int ) const;
  virtual double getAttractorMagnitude( int ) const;
  virtual const Point3d& getAttractorPosition( int ) const;
  virtual attractorType getAttractorType( int ) const;
  virtual QColor getColor() const;
  virtual const Vector3d& getDirection() const;
  virtual std::pair< int, int > getLifeRange() const;
	virtual int getNumberOfAttractors() const;
  virtual int getNumberOfParticules() const;
  virtual const Point3d& getPosition() const;
  virtual double getRadius() const;
  virtual int getRate() const;
  virtual double getSize() const;
  virtual type getType() const;
  virtual std::pair< double, double > getVelocityRange() const;
  virtual bool isDecayEnabled() const;
  virtual void removeAttractor( int );
  virtual void setAttractorForce( int, const Vector3d& );
  virtual void setAttractorMagnitude( int, double );
  virtual void setAttractorPosition( int, const Point3d& );
  virtual void setAttractorType( int, attractorType );
  virtual void setColor( int, int, int, int );
  virtual void setDirection( const Vector3d& );
	virtual void setImage( QImage );
  virtual void setLifeRange( int, int );
  virtual void setNumberOfParticules( int );
  virtual void setPosition( const Point3d& );
  virtual void setRadius( double );
  virtual void setRate( int = -1 );
  virtual void setSize( double );
  virtual void setType( type );
  virtual void setVelocityRange( double, double );

protected:
	struct Particule
  {
    Particule();
  	Particule( const Vector3d&, const Point3d&, int );
    
    Vector3d v;
    Point3d p;
    int t;
  };
  struct Attractor
  {
  	Attractor( attractorType, const Point3d&, const Vector3d&, double );
    attractorType t;
  	Point3d p;
    Vector3d f;
    double m;
  };
  
  virtual void drawParticules() const;
  virtual void iterate() const;
  
  mutable QTime mTime;
  type mType;
  std::pair< double, double > mVelocityRange;
  std::pair< int, int > mLifeRange;
  mutable std::vector< Particule > mParticules;
  double* mpParticulesPosition;
  Texture mTexture;
  std::vector< Attractor > mAttractors;
  Point3d mPosition;
  QColor mColor;
  double mSize;
  Vector3d mDirection;
  double mRadius;
  bool mIsDecayEnabled;
  int mRate;
};

}//treeD
}//realisim

#endif