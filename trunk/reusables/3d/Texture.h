
#ifndef Realisim_TreeD_Texture_hh
#define Realisim_TreeD_Texture_hh

#include <cassert>
#include "math/Vect.h"
#include <QImage>
#include <qgl.h>
#include <QByteArray>
#include <QString>

/*La classe Texture encapsule la création d'une texture (2d, 3d) d'openGL et
  facilite la gestion de la ressource ainsi que l'utilisation via le partage
  implicite.

  Texture permet de chargé une texture 2d openGL a partir d'un QImage avec
  la méthode set(QImage) ou d'une sous région d'une QImage avec la méthode
  set(QImage, QRect).
  
  Texture permet de chargé une texture 3d openGL a partir d'un QByteArray avec
  la méthode set(QByteArray).
    
  La classe Texture est implémenté avec le concepte de partage implicite.
  Une texture instanciée par le constructeur copie ou assigné par l'opérateur
  égale pointe sur la texture originale.
  
  Texture t1(image1);
  Texture t2(t1); => t2 et t1 partage la même texture.
  t2.load(image2); => t2 pointe sur image2 et t1 sur image1.
  t2 = t1; => la texture image2 est détruite parce qu'elle n'est plus 
              référencée et t2 et t1 pointe sur image1.
  t1.load(image3); t1 pointe sur image3 et t2 sur image1.
  
  Bref, ça fonctionne toujours comme on s'y attend et ça limite le nombre
  de ressource openGL/mémoire utilisé. De plus le constructeur copy est
  très léger, on peut donc passer la texture par valeur sans trop s'en soucier.
  
  membre:
  mpGuts: Le coeur du partage implicite. Il n'est jamais null.
  
  Guts:
    mTextureId: Le id de texture assigné par OpenGL
    mSize: La taille de la texture. C'est toujours un vecteur 3d même
      si la texture est 2d parce que je n'ai pas encore de classe vecteur2d.
      Dans le cas de la texture 2d, mSize aura toujours un 0 dans la 
      composante z du vecteur.
    mType: Le type de la texture; 2d ou 3d.
    mFormat: Le format de la texture (voir la doc OpenGL glTexImagexD).
      fLuminance: GL_LUMINANCE
      fRGBA: GL_RGBA
    mDataType: Le type de donné (voir la doc OpenGL glTexImagexD).
      dtUnsignedByte: GL_UNSIGNED_BYTE
      dtUnsignedShort: GL_UNSIGNED_SHORT
    mRefCount: le compte de référence sur les guts.
*/

namespace realisim
{
namespace treeD
{

class Texture
{
public:
  enum type {t2d, t3d, tInvalid};
  enum format {fLuminance, fRgba};
  enum dataType {dtShort, dtUnsignedByte, dtUnsignedShort};
  
  Texture();
Texture(QImage, format = fRgba, dataType = dtUnsignedByte);  //void*, const math::Vector2i?
  Texture(void*, const math::Vector3i&, format = fRgba, dataType = dtUnsignedByte);
  Texture(const Texture&);
  virtual ~Texture();
  virtual Texture& operator=(const Texture&);
  
  virtual dataType getDataType() const {return mpGuts->mDataType;}
  virtual format getFormat() const {return mpGuts->mFormat;}
  virtual const math::Vector3i& getSize() const;
  virtual GLuint getTextureId() const {return mpGuts->mTextureId;}
  virtual type getType() const {return mpGuts->mType;}
  virtual bool isValid() const;
  virtual void set(QImage, format = fRgba, dataType = dtUnsignedByte);
  virtual void set(QImage, QRect, format = fRgba, dataType = dtUnsignedByte);
  virtual void set(void*, const math::Vector3i&, format = fRgba, dataType = dtUnsignedByte);
  
protected:
  virtual void setDataType(dataType iT) {mpGuts->mDataType = iT;}
  virtual void setFormat(format iF) {mpGuts->mFormat = iF;}
  virtual void setType(type iT) {mpGuts->mType = iT;}

  struct Guts
  {
		explicit Guts();
    
    GLuint mTextureId;
    math::Vector3i mSize;
    type mType;
    format mFormat;
    dataType mDataType;
    unsigned int mRefCount;
  };
  
  virtual void copyGuts();
  virtual void deleteGuts();
	virtual void makeGuts();
  virtual void shareGuts(Guts*);
  
  Guts* mpGuts;
};

}//treeD
}//realisim

#endif