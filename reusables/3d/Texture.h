
#ifndef Realisim_TreeD_Texture_hh
#define Realisim_TreeD_Texture_hh

#include "openGlHeaders.h"
#include <cassert>
#include "math/Vect.h"
#include <QImage>
#include <QString>
#include <vector>

/*La classe Texture encapsule la création d'une texture (2d, 3d) d'openGL et
  facilite la gestion de la ressource ainsi que l'utilisation via le partage
  implicite.

  Texture permet de chargé une texture 2d openGL a partir d'un QImage avec
  la méthode set(QImage) ou d'une sous région d'une QImage avec la méthode
  set(QImage, QRect).
  
  Texture permet de chargé une texture 3d openGL a partir d'un pointeur sur les
  données avec la méthode set(void*). Par example:
    QFile f("../Resources/73_512_512");
    if(f.open(QIODevice::ReadOnly))
    {
      QByteArray a = f.readAll();
      mCtTexture.set(a.data(), Vector3i(gVolumeSize.x(), gVolumeSize.y(),
        gVolumeSize.z()), GL_LUMINANCE, GL_UNSIGNED_SHORT);
      f.close();
    }
    
  La classe Texture est implémenté avec le concepte de partage explicite.
  Une texture instanciée par le constructeur copie ou assigné par l'opérateur
  égale pointe sur la texture originale.
  
  Example:
    Texture t1(image1);
    Texture t2(t1); => t2 et t1 partage la même texture.
    t2.set(image2); => t2 et t1 contienne maintenant image2
    t2 = t1; => la texture image2 est détruite parce qu'elle n'est plus 
                référencée et t2 et t1 pointe sur image1.
    t1.set(image3); t1 et t2 pointe sur image3.
  
  On peut aussi faire ceci
  Texture t1(image1); 
  Texture t2(t1); //t2 et t1 pointent sur les mêmes ressources opengl
  t2 = t1.detach() //La reference de t2 a t1 est détruite et t1 pointe
    //sur ces propres ressources opengl.
  t2.set(image2); //t1 pointe encore sur image1 et t2 pointe sur image2
  
  
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
      GL_LUMINANCE
      GL_RGBA ...
    mDataType: Le type de donné (voir la doc OpenGL glTexImagexD).
       GL_UNSIGNED_BYTE
       GL_UNSIGNED_SHORT ...
    mRefCount: le compte de référence sur les guts.
*/

namespace realisim
{
namespace treeD
{

class Texture
{
public:
  enum type {t2d, t3d, tCubeMap, tInvalid};
  
  Texture();
  Texture(const Texture&);
  virtual ~Texture();
  virtual Texture& operator=(const Texture&);
  
  virtual QByteArray asBuffer( GLenum, GLenum ) const;
	virtual QImage asQImage() const;
  virtual Texture copy();
  virtual int depth() const;
//virtual void fromBinary() const; voir spriteCatalog
  virtual void generateMipmap();
  virtual GLenum getDataType() const {return mpGuts->mDataType;}
  virtual GLenum getFormat() const {return mpGuts->mFormat;}
  virtual GLenum getMagnificationFilter() const;
  virtual GLenum getMinificationFilter() const;      
  virtual GLuint getId() const {return mpGuts->mTextureId;}
  virtual type getType() const {return mpGuts->mType;}
  virtual GLenum getWrapSMode() const;
  virtual GLenum getWrapTMode() const;
  virtual GLenum getWrapRMode() const;
  virtual int height() const;
  virtual bool isValid() const;
  virtual void resize( const std::vector<int>& );
  virtual void resize( int, int );
  virtual void resize( int, int, int );
	virtual void set( QImage, GLenum = GL_RGBA );
  virtual void set( void*, const math::Vector2i&, GLenum = GL_RGBA,
    GLenum = GL_UNSIGNED_BYTE );
  virtual void set( void*, const math::Vector3i&, GLenum = GL_RGBA,
    GLenum = GL_UNSIGNED_BYTE );
  virtual void set( void*, const std::vector<int>&, GLenum = GL_RGBA,
    GLenum = GL_UNSIGNED_BYTE );
  virtual void setFilter( GLenum );
  virtual void setFilter( GLenum, GLenum );
  virtual void setMinificationFilter( GLenum );
  virtual void setMagnificationFilter( GLenum );
  virtual void setWrapMode( GLenum );
  virtual void setWrapMode( GLenum, GLenum );
  virtual void setWrapMode( GLenum, GLenum, GLenum );
  virtual void setWrapSMode( GLenum );
  virtual void setWrapTMode( GLenum );
  virtual void setWrapRMode( GLenum );
  virtual const std::vector<int>& size() const;
//virtual void toBinary() const; voir spriteCatalog
  virtual int width() const;
  
protected:
  struct Guts
  {
		explicit Guts();
    
    GLuint mTextureId;
    std::vector<int> mSize;
    type mType;
    GLenum mFormat;
    GLenum mDataType;
    GLenum mMinificationFilter;
    GLenum mMagnificationFilter;
    GLenum mWrapSMode;
    GLenum mWrapTMode;
    GLenum mWrapRMode;
    unsigned int mRefCount;
  };
  
  virtual void deleteGuts();  
  virtual void makeGuts();
  virtual void setDataType(GLenum iT) {mpGuts->mDataType = iT;}
  virtual void setFormat(GLenum iF) {mpGuts->mFormat = iF;}
  virtual void setType(type iT) {mpGuts->mType = iT;}
  virtual void shareGuts(Guts*);

  Guts* mpGuts;
};

}//treeD
}//realisim

#endif