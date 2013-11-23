
#ifndef Realisim_TreeD_FrameBufferObject_hh
#define Realisim_TreeD_FrameBufferObject_hh

#include <cassert>
#include <QImage>
#include <qgl.h>
#include "Texture.h"
#include <vector>

/*Cette classe sert à créer et interagir avec un FramebufferObject d'OpenGL.
  Toutes les ressources nécessaires au frame buffer (renderBuffers et textures)
  sont prises en charge par cette classe. Quand on dit prisent en charge, on
  veut dire création et destruction. De plus, afin de gérer efficacement les
  ressources openGL, la classe FrameBufferObject utilise le partage explicite
  (voir méthode copy pour comprendre pourquoi on ne peut pas utiliser le partage
   implicite). La copie d'un Shader doit être faite par un appel à la méthode
  copy(). Le constructeur copie et l'opérateur= ne font qu'incrémenter le compte
  de référence sur les guts (les ressources openGL).
  
  example:
    FrameBufferObject fbo;
    fbo.addColorAttachement(true);
    fbo.addDepthAttachement(false);
    
    pushFrameBuffer(fbo);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);
    glViewport(0, 0, 200, 150);
    fbo.resize(200, 150);
    fbo.drawTo(0);
    glClearColor(0.0, 0.0, 0.0, 0.5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(255, 0, 0);
    drawSomething();
    glPopAttrib();
    popFrameBuffer();
    
    ...
    
    Texture t = fbo.getTexture(0);
    Sprite s;
    s.setTexture(t);
    s.setAnchorPoint(Sprite::aTopLeft);
    s.set2dPositioningOn(true);
    s.set2dPosition(10, 10, 0);
    s.draw(getCamera());
  
  membres:
    mpGuts: pointer ur les membres, Il n'est jamais null.

    mFrameBufferId: id du frameBuffer. Il vaut 0 (zero) quand il n'est pas 
      alloué;
    mDepthRenderBufferId: id du render buffer utiliser pour le 'depth'. Il vaut
      0 (zero) quand il n'est pas alloué;
		mStencilRenderBufferId: id du stencil buffer utiliser pour le 'depth'. Il vaut
      0 (zero) quand il n'est pas alloué;
		mIsValid; indique si le frame buffer est valide. Lorsqu'il est valide, il est
      prêt à être utilisé.
		mWidth; Largeur du frame buffer (ainsi que toutes les ressources qu'il
      contient). Quand on modifie la taille du frame buffer via la méthode
      resize(int, int), on change aussi la taille de toutes les ressources
      contenu par le framebuffer.
		mHeight; Hauteur du frame buffer... similaire a mWidth.
		mColorAttachments; Le vecteur de id de render/texture pour les attachements
      de couleur. Le boolean sert a indiqué si le id correspond à une texture
      (vrai) ou à un render buffer (faux). Voir la méthode
      addColorAttachments(bool iUseTexture).
    mTextures; une map des Textures. L'index est l'identificateur du color
      attachemnt. Cette map sert principalement à la méthode getTexture(int).
    mDepthTexture; Représente la texture associée à l'attachement de depth 
      lorsque la méthode addDepthAttachment(bool) est appelée avec le 
      paramètre iUseTexture a vrai.
    mMaxColorAttachment; Détient le nombre maximal de color attachment supporté
      par la carte graphique.
    mRefCount; Le compte de référence sur les guts.
  
  Notes:
  Widget3d possède les méthodes pushFrameBuffer(FrameBufferObject()) et
  popFrameBuffer qui permettent d'empiler/dépiler correctement les frame
  buffers.
  */

namespace realisim
{
namespace treeD
{
using namespace std;

/**/
class FrameBufferObject
{
public:  
  explicit FrameBufferObject(int = 0, int = 0);
  FrameBufferObject(const FrameBufferObject&);
  virtual ~FrameBufferObject();
  virtual FrameBufferObject& operator=(const FrameBufferObject&);

  virtual void addDepthAttachment(bool);
	virtual void addStencilAttachment();
  virtual void addColorAttachment(bool);
  virtual void begin();
//virtual void addColorAttachment(Texture);
  virtual void clear();
  virtual FrameBufferObject copy(); //detach voir Shader::copy  
  virtual void drawTo(int);
  virtual Texture getDepthTexture() const {return mpGuts->mDepthTexture;}
  virtual GLuint getFrameBufferId() const {return mpGuts->mFrameBufferId;}
	virtual int getHeight() const {return mpGuts->mHeight;}
  virtual int getNumColorAttachment() const {return mpGuts->mColorAttachments.size();}
  virtual QImage getImageFrom(int) const;
  virtual math::Vector2i getSize() const;
  virtual Texture getTexture(int) const;
	virtual int getWidth() const {return mpGuts->mWidth;}
  virtual void end();
  virtual bool isValid() const;
//virtual void removeColorAttachment(int);
//virtual void removeDepthAttachment();
//virtual void removeStencilAttachment();
  virtual void resize(int, int);
  virtual void resize( math::Vector2i );
  
protected:
  virtual bool isDepthAttachmentUsingTexture() const;
  virtual void init();
  virtual bool isColorAttachmentUsed() const {return !mpGuts->mColorAttachments.empty();}
	virtual bool isDepthAttachmentUsed() const;
	virtual bool isStencilAttachmentUsed() const {return mpGuts->mStencilRenderBufferId != 0 ;}
	virtual GLuint getDepthRenderBufferId() const {return mpGuts->mDepthRenderBufferId;}
	virtual GLuint getStencilRenderBufferId() const {return mpGuts->mStencilRenderBufferId;}
  virtual int getMaxColorAttachment() const {return mpGuts->mMaxColorAttachment;}
  virtual void validate();

  struct Guts
  {
		explicit Guts(int = 0, int = 0);
    
    GLuint mFrameBufferId;
    GLuint mDepthRenderBufferId;
		GLuint mStencilRenderBufferId;
		bool mIsValid;
		int mWidth;
		int mHeight;
		vector<pair<GLuint, bool> > mColorAttachments;
    map<int, Texture> mTextures;
    Texture mDepthTexture;
    int mMaxColorAttachment;
    vector<GLuint> mPreviousFrameBuffers;
    unsigned int mRefCount;
  };
  
  virtual void deleteGuts();
	virtual void makeGuts(int = 0, int = 0);
  virtual void shareGuts(Guts*);
  
  Guts* mpGuts;
};

}//treeD
}//realisim

#endif