
#ifndef Realisim_TreeD_FrameBufferObject_hh
#define Realisim_TreeD_FrameBufferObject_hh

#include <cassert>
#include "openGlHeaders.h"
#include <QImage>
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
    fbo.addColorAttachement();
    fbo.addDepthAttachement();

    fbo.begin();
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);
    glViewport(0, 0, 200, 150);
    fbo.resize(200, 150);
    fbo.drawTo(0);
    glClearColor(0.0, 0.0, 0.0, 0.5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(255, 0, 0);
    drawSomething();
    glPopAttrib();
    fbo.end()

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
    mStencilRenderBufferId: id du stencil buffer utiliser pour le 'depth'. Il vaut
      0 (zero) quand il n'est pas alloué;
        mIsValid; indique si le frame buffer est valide. Lorsqu'il est valide, il est
      prêt à être utilisé.
    mWidth; Largeur du frame buffer (ainsi que toutes les ressources qu'il
      contient). Quand on modifie la taille du frame buffer via la méthode
      resize(int, int), on change aussi la taille de toutes les ressources
      contenu par le framebuffer.
    mHeight; Hauteur du frame buffer... similaire a mWidth.
    mColorAttachments; Le vecteur texture pour les attachements
      de couleur.
    mDepthTexture; Représente la texture associée à l'attachement de depth
      lorsque la méthode addDepthAttachment(bool) est appelée avec le
      paramètre iUseTexture a vrai.
    mMaxColorAttachment; Détient le nombre maximal de color attachment supporté
      par la carte graphique.
    mRefCount; Le compte de référence sur les guts.
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
         FrameBufferObject();
        FrameBufferObject(const FrameBufferObject&);
        ~FrameBufferObject();
        FrameBufferObject& operator=(const FrameBufferObject&);

        void addDepthAttachment();
        void addStencilAttachment();
        void addColorAttachment();
        void addColorAttachment(GLenum internalFormat, GLenum format, GLenum dataType);
        void begin();
        void clear();
        FrameBufferObject copy(); //detach voir Shader::copy  
        void drawTo(int);
		void end();
        Texture getColorAttachment(int) const;
        Texture getDepthAttachment() const { return mpGuts->mDepthTexture; }
        GLuint getFrameBufferId() const { return mpGuts->mFrameBufferId; }
        int getHeight() const { return mpGuts->mHeight; }
        int getNumColorAttachment() const { return (int)mpGuts->mColorAttachments.size(); }
        QImage getImageFrom(int) const;
        math::Vector2i getSize() const;
        int getWidth() const { return mpGuts->mWidth; }
        void glClear(GLenum);
        bool isValid() const;
        //virtual void removeColorAttachment(int);
        //virtual void removeDepthAttachment();
        //virtual void removeStencilAttachment();
        void resize(int, int);
        void resize(math::Vector2i);

    protected:
        void init();
        bool isColorAttachmentUsed() const { return !mpGuts->mColorAttachments.empty(); }
        bool isDepthAttachmentUsed() const;
        bool isStencilAttachmentUsed() const { return mpGuts->mStencilRenderBufferId != 0; }
        GLuint getStencilRenderBufferId() const { return mpGuts->mStencilRenderBufferId; }
        int getMaxColorAttachment() const { return mpGuts->mMaxColorAttachment; }
        void validate();

        struct Guts
        {
            Guts();

            GLuint mFrameBufferId;
            GLuint mStencilRenderBufferId;
            bool mIsValid;
            int mWidth;
            int mHeight;
            vector<Texture> mColorAttachments;
            Texture mDepthTexture;
            int mMaxColorAttachment;
            vector<GLuint> mPreviousFrameBuffers;
            unsigned int mRefCount;
        };

        void deleteGuts();
        void makeGuts();
        void shareGuts(Guts*);

        Guts* mpGuts;
    };

}//treeD
}//realisim

#endif