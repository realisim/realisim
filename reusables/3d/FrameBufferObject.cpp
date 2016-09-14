
#include "FrameBufferObject.h"
#include "math/Vect.h"
#include "OpenGLInfo.h"
#include <QDebug>

using namespace realisim;
using namespace math;
using namespace treeD;


FrameBufferObject::Guts::Guts() : mFrameBufferId(0),
mStencilRenderBufferId(0),
mIsValid(false),
mWidth(1),
mHeight(1),
mColorAttachments(),
mDepthTexture(),
mMaxColorAttachment(0),
mPreviousFrameBuffers(),
mRefCount(1)
{}

//---
FrameBufferObject::FrameBufferObject() : mpGuts(0)
{
    makeGuts();
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
        glDeleteRenderbuffers(1, &mpGuts->mStencilRenderBufferId);
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
            glDeleteFramebuffers(1, &mpGuts->mFrameBufferId);
        delete mpGuts;
        mpGuts = 0;
    }
}

//----------------------------------------------------------------------------
void FrameBufferObject::addColorAttachment()
{
    addColorAttachment(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
}

//----------------------------------------------------------------------------
void FrameBufferObject::addColorAttachment(GLenum internalFormat, GLenum format, GLenum dataType)
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
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

    int index = (int)mpGuts->mColorAttachments.size();
    GLenum e = GL_COLOR_ATTACHMENT0 + index;

    Texture t;
    vector<int> s(2, 0); s[0] = getWidth(); s[1] = getHeight();
    t.set(0, s, internalFormat, format, dataType);
    t.setWrapMode(GL_CLAMP);
    mpGuts->mColorAttachments.push_back(t);
    glFramebufferTexture2D(GL_FRAMEBUFFER, e, GL_TEXTURE_2D, t.getId(), 0);


    validate();
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::addDepthAttachment()
{
    init();

    //early out
    if (!getFrameBufferId())
        return;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebufferEXT(GL_FRAMEBUFFER, getFrameBufferId());

    Texture t;
    vector<int> s(2, 0); s[0] = getWidth(); s[1] = getHeight();
    t.set(0, s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    t.setFilter(GL_LINEAR, GL_LINEAR);
    mpGuts->mDepthTexture = t;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, getDepthAttachment().getId(), 0);

    validate();
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::addStencilAttachment()
{
    init();

    //early out
    if (!getFrameBufferId())
        return;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());
    if (getStencilRenderBufferId() == 0)
        glGenRenderbuffers(1, &mpGuts->mStencilRenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, getStencilRenderBufferId());
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX,
        getWidth(), getHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //on attache le depth
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, getStencilRenderBufferId());

    validate();
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void FrameBufferObject::begin()
{
    GLint previousFboId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFboId);
    mpGuts->mPreviousFrameBuffers.push_back(previousFboId);
    if (isValid()) glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());
}

//----------------------------------------------------------------------------
void FrameBufferObject::drawTo(int iIndex)
{
    GLenum e = GL_COLOR_ATTACHMENT0 + iIndex;
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
    glBindFramebuffer(GL_FRAMEBUFFER, previousFboId);
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
        glGenFramebuffers(1, &mpGuts->mFrameBufferId);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mpGuts->mMaxColorAttachment);
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
void FrameBufferObject::glClear(GLenum iClearFlags)
{
	GLint previousFb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
	glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

	if(iClearFlags & GL_DEPTH_BUFFER_BIT)
	{ ::glClear(GL_DEPTH_BUFFER_BIT); }

	if(iClearFlags & GL_STENCIL_BUFFER_BIT)
	{ ::glClear(GL_STENCIL_BUFFER_BIT); }

	if (iClearFlags & GL_COLOR_BUFFER_BIT)
	{
		for (int i = 0; i < getNumColorAttachment(); ++i)
		{
			drawTo(i);
			::glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
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
void FrameBufferObject::makeGuts()
{
    mpGuts = new Guts();
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
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

    for (int i = 0; i < getNumColorAttachment(); ++i)
    {
        GLenum e = GL_COLOR_ATTACHMENT0 + i;        
        Texture t = getColorAttachment(i);
        t.resize(getWidth(), getHeight());
        glFramebufferTexture2D(GL_FRAMEBUFFER, e, GL_TEXTURE_2D, t.getId(), 0);
    }

    if (isDepthAttachmentUsed())
    {
        mpGuts->mDepthTexture.resize(getWidth(), getHeight());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, getDepthAttachment().getId(), 0);
    }

    if (isStencilAttachmentUsed())
    {
        glBindRenderbuffer(GL_RENDERBUFFER, getStencilRenderBufferId());
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX,
            getWidth(), getHeight());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, getStencilRenderBufferId());
    }

    validate();
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
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
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        s += "complete.";
        mpGuts->mIsValid = true;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        s += "incomplete: Attachment is NOT complete."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        s += "incomplete: No image is attached to FBO."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        s += "incomplete: Attached images have different dimensions."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        s += "incomplete: Color attached images have different internal formats.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        s += "incomplete: Draw buffer."; break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        s += "incomplete: Read buffer."; break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        s += "Unsupported by FBO implementation."; break;
    default: s += "Unknow error."; break;
    }
    if (!isValid()) qDebug() << s;
}

