
#include "FrameBufferObject.h"
#include "math/Vect.h"
#include "OpenGLInfo.h"
#include <QDebug>

using namespace realisim;
using namespace math;
using namespace treeD;


FrameBufferObject::Guts::Guts(int iW/*= 0*/, int iH /*= 0*/) : mFrameBufferId(0),
mStencilRenderBufferId(0),
mIsValid(false),
mWidth(iW),
mHeight(iH),
mColorAttachments(),
mDepthTexture(),
mMaxColorAttachment(0),
mPreviousFrameBuffers(),
mRefCount(1)
{}

//---
FrameBufferObject::FrameBufferObject(int iW/*= 1*/, int iH /*= 1*/) : mpGuts(0)
{
    makeGuts(iW, iH);
}

FrameBufferObject::FrameBufferObject(const FrameBufferObject& iT) : mpGuts(0)
{
    shareGuts(iT.mpGuts);
}

FrameBufferObject::~FrameBufferObject()
{
    deleteGuts();
}

FrameBufferObject& FrameBufferObject::operator=(const FrameBufferObject& iT)
{
    if (mpGuts == iT.mpGuts)
        return *this;

    deleteGuts();
    shareGuts(iT.mpGuts);
    return *this;
}

//----------------------------------------------------------------------------
// vide le fbo de tous ces colorAttachment, depthAttachment et
// stencilAttachement.
void FrameBufferObject::clear()
{
    /*On clear le vecteur de textures pour les color attachment, ce qui appele le destructeur des
      Textures. On ne relache pas le frameBuffer ici... cela est fait dans
      deleteGuts, lorsque l'object FrameBufferObject est détruit et qu'il
      n'a plus aucune référence.*/
    
    mpGuts->mColorAttachments.clear();
    mpGuts->mDepthTexture = Texture();
    if (getStencilRenderBufferId() != 0)
        glDeleteRenderbuffersEXT(1, &mpGuts->mStencilRenderBufferId);
}

//----------------------------------------------------------------------------
/*Cette méthode sert a copier le fbo et d'y allouer de nouvelle ressources
  opengl (renderBuffer et textures). On ne peut pas utiliser le partage
  implicite sur cette classe et créer une nouvelle instance de fbo qui aurait
  le meme id (ainsi que les memes id de frameBuffer, renderBuffer, texture)
  parce que lors de la destruction du fbo initiale, les ressources opengl serait
  detruites causant ainsi de grâve problème à la copy. C'est pourquoi la méthode
  copy ré-alloue completement un nouveau FrameBuffer!*/
FrameBufferObject FrameBufferObject::copy()
{
    FrameBufferObject fbo;
    fbo.resize(getWidth(), getHeight());
    for (int i = 0; i < getNumColorAttachment(); ++i)
        fbo.addColorAttachment();

    if (isDepthAttachmentUsed())
        fbo.addDepthAttachment();

    if (isStencilAttachmentUsed())
        fbo.addStencilAttachment();

    fbo.mpGuts->mPreviousFrameBuffers = mpGuts->mPreviousFrameBuffers;
    return fbo;
}

//----------------------------------------------------------------------------
void FrameBufferObject::deleteGuts()
{
    if (mpGuts && --mpGuts->mRefCount == 0)
    {
        /*On relache toutes les ressources openGL.*/
        clear();
        if (mpGuts->mFrameBufferId != 0)
            glDeleteFramebuffersEXT(1, &mpGuts->mFrameBufferId);
        delete mpGuts;
        mpGuts = 0;
    }
}

//----------------------------------------------------------------------------
void FrameBufferObject::addColorAttachment()
{
    init();

    if (mpGuts->mColorAttachments.size() >= (unsigned int)getMaxColorAttachment())
    {
        qDebug("The maximum number of color attachment has been reach. Cannot add \
		  any more.");
        return;
    }

    //early out
    if (!getFrameBufferId())
        return;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &previousFb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFrameBufferId());

    int index = (int)mpGuts->mColorAttachments.size();
    GLenum e = GL_COLOR_ATTACHMENT0_EXT + index;
    
    Texture t;
    vector<int> s(2, 0); s[0] = getWidth(); s[1] = getHeight();
    t.set(0, s, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    t.setWrapMode(GL_CLAMP);
    mpGuts->mColorAttachments.push_back(t);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, e, GL_TEXTURE_2D, t.getId(), 0);
    

    validate();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::addDepthAttachment()
{
    init();

    //early out
    if (!getFrameBufferId())
        return;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &previousFb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFrameBufferId());

    Texture t;
    vector<int> s(2, 0); s[0] = getWidth(); s[1] = getHeight();
    t.set(0, s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    t.setFilter(GL_LINEAR, GL_LINEAR);
    mpGuts->mDepthTexture = t;
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
        GL_TEXTURE_2D, getDepthAttachment().getId(), 0);

    validate();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::addStencilAttachment()
{
    init();

    //early out
    if (!getFrameBufferId())
        return;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &previousFb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFrameBufferId());
    if (getStencilRenderBufferId() == 0)
        glGenRenderbuffersEXT(1, &mpGuts->mStencilRenderBufferId);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, getStencilRenderBufferId());
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX,
        getWidth(), getHeight());
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    //on attache le depth
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
        GL_RENDERBUFFER_EXT, getStencilRenderBufferId());

    validate();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::begin()
{
    GLint previousFboId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &previousFboId);
    mpGuts->mPreviousFrameBuffers.push_back(previousFboId);
    if (isValid()) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFrameBufferId());
}

//----------------------------------------------------------------------------
void FrameBufferObject::drawTo(int iIndex)
{
    GLenum e = GL_COLOR_ATTACHMENT0_EXT + iIndex;
    glDrawBuffersARB(1, &e);
}

//----------------------------------------------------------------------------
void FrameBufferObject::end()
{
    GLuint previousFboId = 0;
    if (mpGuts->mPreviousFrameBuffers.size() > 0)
    {
        previousFboId = mpGuts->mPreviousFrameBuffers.back();
        mpGuts->mPreviousFrameBuffers.pop_back();
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, previousFboId);
}

//----------------------------------------------------------------------------
void FrameBufferObject::init()
{
    OpenGLInfo i;
    if (!i.isExtensionSupported("GL_EXT_framebuffer_object"))
    {
        qDebug("Frame buffer extension is not supported. OpenGL 2.1 is required.");
        return;
    }

    if (getFrameBufferId() == 0)
    {
        glGenFramebuffersEXT(1, &mpGuts->mFrameBufferId);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &mpGuts->mMaxColorAttachment);
    }
        
}

//----------------------------------------------------------------------------
/*Retourne la texture associé au colorAttachement*/
Texture FrameBufferObject::getColorAttachment(int iIndex) const
{
    Texture t;
    if (iIndex >= 0 && iIndex < (int)mpGuts->mColorAttachments.size())
    {
        t = mpGuts->mColorAttachments[iIndex];
    }
    return t;
}

//----------------------------------------------------------------------------
/*iIndex refert a l'index de l'attachement de couleur... Donc si on a attaché
  3 textures via la méthode addColorAttachement(). On peut
  aller chercher un QImage par getImageFrom(iIndex) avec iIndex = 0, 1, ou 2.*/
QImage FrameBufferObject::getImageFrom(int iIndex) const
{
    QImage r(getWidth(), getHeight(), QImage::Format_ARGB32);

    //early out
    if (iIndex >= getNumColorAttachment())
        return r;

    Texture t = getColorAttachment(iIndex);
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t.getId());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, t.getDataType(), r.bits());
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();
    

    return r.mirrored(true, false);
}

//----------------------------------------------------------------------------
Vector2i FrameBufferObject::getSize() const
{
    return Vector2i(getWidth(), getHeight());
}

//----------------------------------------------------------------------------
bool FrameBufferObject::isDepthAttachmentUsed() const
{
    return mpGuts->mDepthTexture.getId() != 0;
}

//----------------------------------------------------------------------------
bool FrameBufferObject::isValid() const
{
    return mpGuts->mIsValid;
}

//----------------------------------------------------------------------------
void FrameBufferObject::makeGuts(int iW/*= 0*/, int iH /*= 0*/)
{
    mpGuts = new Guts(iW, iH);
}

//----------------------------------------------------------------------------
void FrameBufferObject::resize(int iWidth, int iHeight)
{
    /*On ne fait rien si les tailles sont les mêmes.*/
    if (getWidth() == iWidth && getHeight() == iHeight)
        return;

    mpGuts->mWidth = iWidth;
    mpGuts->mHeight = iHeight;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &previousFb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFrameBufferId());

    for (int i = 0; i < getNumColorAttachment(); ++i)
    {
        GLenum e = GL_COLOR_ATTACHMENT0_EXT + i;        
        Texture t = getColorAttachment(i);
        t.resize(getWidth(), getHeight());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, e, GL_TEXTURE_2D, t.getId(), 0);
    }

    if (isDepthAttachmentUsed())
    {
        mpGuts->mDepthTexture.resize(getWidth(), getHeight());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            GL_TEXTURE_2D, getDepthAttachment().getId(), 0);
    }

    if (isStencilAttachmentUsed())
    {
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, getStencilRenderBufferId());
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX,
            getWidth(), getHeight());
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT, getStencilRenderBufferId());
    }

    validate();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::resize(Vector2i iSize)
{
    resize(iSize.x(), iSize.y());
}

//----------------------------------------------------------------------------
void FrameBufferObject::shareGuts(Guts* g)
{
    mpGuts = g;
    ++mpGuts->mRefCount;
}

//----------------------------------------------------------------------------
void FrameBufferObject::validate()
{
    mpGuts->mIsValid = false;
    QString s("Framebuffer " + QString::number(getFrameBufferId()) + " ");
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        s += "complete.";
        mpGuts->mIsValid = true;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        s += "incomplete: Attachment is NOT complete."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        s += "incomplete: No image is attached to FBO."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        s += "incomplete: Attached images have different dimensions."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        s += "incomplete: Color attached images have different internal formats.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        s += "incomplete: Draw buffer."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        s += "incomplete: Read buffer."; break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        s += "Unsupported by FBO implementation."; break;
    default: s += "Unknow error."; break;
    }
    if (!isValid()) qDebug() << s;
}

