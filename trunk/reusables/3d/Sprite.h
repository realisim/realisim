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
#include "math/Vect.h"

/*Sprite permet d'afficher/animer une texture à l'écran. Il y a deux façon
  de positionner un Sprite à l'écran. Méthode openGL classique qui permet
  de mettre le sprite dans l'espace 3d. Mapper un panneau publicitaire
  par exemple.
  
  glPushMatrix();
  glMultMatrixd(aMatrix);
  glRotated((angle, x, y, z);
  glTranslated(x, y, z);
  glScaled(x, y, z);
  mSprite.draw(camera);
  glPopMatrix();
 
  L'autre méthode consiste à positionner le Sprite en 2d. Dans ce mode, le
  Sprite aura exactement la même taille que la texture (ou la sous région
  de la texture si la méthode setTexture(Texture, QRect) est utilisée)
  utilisé. De plus le Sprite sera positionné au pixel x, y et à la 
  profondeur z. Le système de coordonné pour le positionnement en mode 2d
  est le suivant: (0, 0, z) se trouve dans le coin supérieur gauche de l'écran.
  
  
  set2dPositioningOn(true);
  set2dPosition(math::Vector3(10, 10, 0));
  
  ...
  
  glPushMatrix();
  glTranslate/scale/rotate n'auront aucun effet...
  mSprite.draw(camera);
  glPopMatrix();
  
  La position du Sprite (mode 2d ou 3d) peut être ajustée avec la méthode
  setAnchor. Par exemple, si on fait:
  
  setAnchor(aCenter);
  set2dPosition(100, 100, 0);
  
  Le centre du Sprite sera positioné a (100, 100, 0).
  
  Note:
  Le mode de positionnement 2d permet aussi de spécifier la coordonnée Z
  qui permet d'avoir plusieur plan de texute en mode 2d. Lorsque la texture
  se dessine, elle ne tient pas compte du depth buffer. Ce qui veut dire 
  qu'elle se dessinera pas dessus les élément déjà dessiné s'il sont derriére
  elle. Donc lorsqu'on affiche des Sprites, l'ordre d'affichage et très
  important. Par exemple pour un jeux:
  
  1- dessiner le backGround(Sprite)
  2- dessiner les élément du jeux
  3- dessiner le HUD(Sprite)(Ui du jeux, les icones ou le pointage).

 membre:
  Texture mTexture; 
	QRect mSubTextureRect; Rectangle en pixel qui détermine la région utilisé
    de la tecture.
  QRectF mGLTexCoord; Comme mSubTextureRect, mais en coordonné openGL.
  mutable state mState; État du Sprite. ie: en animation(sAnimating),
    animation terminé (sAnimatingDone) ou pas d'animation (sStatic)
  anchor mAnchor; Détermine le point d'ancrage du Sprite. 
  math::Vector3i m2dPosition; Position en pixel du Sprite. Seulement utilisé
    si m2dPositioningOn est vrai.
  bool m2dPositioningOn; Détermine si le Sprite est en mode 2d. La position
    du Sprite est alors déterminé par m2dPosition.
  mIsFullScreen: Écrase toutes les option de positionnements/affichage. Même
    si set2dPosition à été appelé. Le sprite s'affichera sur la totatlité de
    l'écran. Très pratique en combinaison avec le framebuffer.
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
	enum state{sAnimating, sAnimatingDone, sStatic};
  
  Sprite();
  Sprite(const Sprite&);
  virtual ~Sprite();
  
  virtual void draw(const Camera& c, bool = false) const;
  virtual math::Vector3d get2dPosition() const {return m2dPosition;}
  virtual anchor getAnchorPoint() const {return mAnchor;}
  virtual int getAnimationDuration() const {return mAnimationDuration;}
  virtual QTime getAnimationTimer() const {return mAnimationTimer;}
  virtual Texture getTexture() const {return mTexture;}
	virtual state getState() const {return mState;}
  virtual QRect getSubTextureRect() const {return mSubTextureRect;}
  virtual bool is2dPositioningOn() const {return m2dPositioningOn;}
  virtual bool isLooping() const {return mIsLooping;}
  virtual bool isFullScreen() const {return mIsFullScreen;}
  virtual void set2dPosition(const math::Vector3i& p);
  virtual void set2dPosition(const math::Vector3d& p) {m2dPosition = p;}
  virtual void set2dPosition(const int, const int);
  virtual void set2dPosition(const int, const int, double);
  virtual void set2dPositioningOn(bool b) {m2dPositioningOn = b;}
  virtual void setAnchorPoint(anchor a) { mAnchor = a; }
  virtual void setAnimationDuration(double d) {mAnimationDuration = d;}
  virtual void setAsLooping(bool l) {mIsLooping = l;}
  virtual void setFullScreen(bool f) {mIsFullScreen = f;}
  virtual void setFrameSize(QSize s);
  virtual void setTexture(const Texture& t);
  virtual void setTexture(const Texture& t, QRect);
  virtual void startAnimation();
  virtual void startAnimation(double, bool, QSize);
  virtual void stopAnimation();  

protected:
  virtual QSize getFrameSize() const {return mFrameSize;}
  virtual math::Vector3d getOriginTranslation() const;
  virtual QRectF getGLTexCoord() const {return mGLTexCoord;}
  virtual void setState(state s) const {mState = s;}

  Texture mTexture;
	QRect mSubTextureRect;
  QRectF mGLTexCoord;
  mutable state mState;
  anchor mAnchor;
  math::Vector3d m2dPosition;
  bool m2dPositioningOn;
  QTime mAnimationTimer;
  int mAnimationDuration;
  bool mIsLooping;
  QSize mFrameSize;
  bool mIsFullScreen;
};

}//treeD
}//realisim

#endif