/* Sprite.h */

#ifndef Realisim_TreeD_Sprite_hh
#define Realisim_TreeD_Sprite_hh

namespace realisim{namespace treeD{class Camera;}}
#include <QImage>
#include <qgl.h>
#include <QRect>
#include <QString>
#include <QTime>
#include "3d/Texture.h"
#include "math/Primitives.h"

/*Sprite permet d'afficher/animer une texture à l'écran. La méthode draw affiche
  le sprite dans le plan x,y et la taille du sprite est identique à celle de 
  l'image (ou texture) utilisée pour créer le sprite. Par exemple, si on utilise
  une image de 640x480 pixel, le sprite aura une dimention de 640x480.
  
  La position du Sprite peut être ajustée avec la méthode setAnchor. Par
  exemple, si on fait: setAnchor(aCenter);
  L'origine du sprite sera au centre.
  Si on fait: setAnchor(aTopLeft);
  L'origine sera dans le coin supérieur gauche

 membre:
  Texture mTexture; 
  mutable state mState; État du Sprite. ie: en animation(sAnimating),
    animation terminé (sAnimatingDone) ou pas d'animation (sStatic)
  anchor mAnchor; Détermine le point d'ancrage du Sprite. 
  bool isLooping;
  */

namespace realisim
{
namespace treeD
{

class Sprite
{
public:
  enum anchor{aBottomLeft, aBottomCenter, aBottomRight,
    aCenterLeft, aCenter, aCenterRight,
    aTopLeft, aTopCenter, aTopRight};
  
  Sprite();
  Sprite(const Sprite&);
  Sprite& operator=(const Sprite&);
  virtual ~Sprite();
  
  virtual QImage asQImage() const;
  virtual void draw() const;
  virtual anchor getAnchorPoint() const {return mAnchor;}
//virtual void fromBinary() const; voir spriteCatalog
  virtual int getAnimationDuration() const {return mAnimationDuration;}
  virtual const math::Vector2i& getFrameGrid() const {return mFrameGrid;}
  virtual int getNumberOfFrames() const {return mNumberOfFrames;}
  virtual QRect getRect() const {return mRect;}
  virtual const math::Vector2i& getFrameSize() const {return mFrameSize;}
  virtual Texture getTexture() const {return mTexture;}
  virtual math::Vector2d getTranslation() const;
  virtual bool isLooping() const {return mIsLooping;}
  virtual void setAnchorPoint(anchor a) { mAnchor = a; }
  virtual void setAnimationDuration(double d) {mAnimationDuration = d;}
  virtual void setAsLooping(bool l) {mIsLooping = l;}
  virtual void set(const Texture& t);
  virtual void set(const Texture& t, QRect);
  virtual void set(QImage);
  virtual void setFrameGrid( int, int ); 
  virtual void setRect( const QRect&);
  virtual void setNumberOfFrames( int iF ) {mNumberOfFrames = iF;}
  virtual void startAnimation();
//virtual void toBinary() const; voir spriteCatalog

protected:
	virtual int getCurrentFrameIndex() const;
  virtual math::Rectangle getFrameTextureCoordinate() const;
virtual int getFrameHeight() const {return mFrameSize.y();}
virtual int getFrameWidth() const {return mFrameSize.x();}
	virtual QTime getTimer() const {return mTimer;}

  Texture mTexture;
  anchor mAnchor;
  QTime mTimer;
  int mAnimationDuration;
  bool mIsLooping;
  math::Vector2i mFrameSize;
  QRect mRect;
  int mNumberOfFrames;
  math::Vector2i mFrameGrid;
  mutable int mCurrentFrameIndex;
};

}//treeD
}//realisim

#endif