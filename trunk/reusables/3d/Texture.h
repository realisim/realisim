
#ifndef Realisim_TreeD_Texture_hh
#define Realisim_TreeD_Texture_hh

#include <cassert>
#include <QImage>
#include <qgl.h>
#include <QString>

/*Texture permet de chargé une texture openGL a partir d'un QImage avec
  la méthode load(QImage) ou d'une sous région d'une QImage avec la méthode
  load(QImage, QRect). 
  
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
*/

namespace realisim
{
namespace treeD
{

class Texture
{
public:
  Texture();
  Texture(QImage);
  Texture(const Texture&);
  virtual ~Texture();
  virtual Texture& operator=(const Texture&);
  
  virtual QSize getSize() const {return mpGuts->mSize;}
  virtual GLuint getTextureId() const {return mpGuts->mTextureId;}
  virtual bool isValid() const;
  virtual void load(QImage);
  virtual void load(QImage, QRect);
  
protected:
  struct Guts
  {
		explicit Guts();
    
    GLuint mTextureId;
    QSize mSize;
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