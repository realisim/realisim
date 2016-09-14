
#include "3d/MultisampleFrameBufferObject.h"
#include "math/Vect.h"
#include "OpenGLInfo.h"
#include <QDebug>

using namespace realisim;
using namespace math;
using namespace treeD;

//----------------------------------------------------------------------------
//--- MultisampleFrameBufferObject - Guts
//----------------------------------------------------------------------------
MultisampleFrameBufferObject::Guts::Guts() : mFrameBufferId(0),
mIsValid(false),
mWidth(1),
mHeight(1),
mColorAttachments(),
mColorAttachmentInternalFormat(),
mDepthTextureId(0),
mMaxColorAttachment(0),
mPreviousFrameBuffers(),
mNumberOfSamples(2),
mHasFixedSampleLocations(false),
mRefCount(1)
{}

//----------------------------------------------------------------------------
//--- MultisampleFrameBufferObject
//----------------------------------------------------------------------------
MultisampleFrameBufferObject::MultisampleFrameBufferObject() : mpGuts(0)
{
    makeGuts();
}

//----------------------------------------------------------------------------
MultisampleFrameBufferObject::MultisampleFrameBufferObject(const MultisampleFrameBufferObject& iT) : mpGuts(0)
{
    shareGuts(iT.mpGuts);
}

//----------------------------------------------------------------------------
MultisampleFrameBufferObject::~MultisampleFrameBufferObject()
{
    deleteGuts();
}

//----------------------------------------------------------------------------
MultisampleFrameBufferObject& MultisampleFrameBufferObject::operator=(const MultisampleFrameBufferObject& iT)
{
    if (mpGuts == iT.mpGuts)
        return *this;

    deleteGuts();
    shareGuts(iT.mpGuts);
    return *this;
}

//----------------------------------------------------------------------------
// vide le fbo de tous ces colorAttachment, depthAttachment
void MultisampleFrameBufferObject::clear()
{
	for (size_t i = 0; i < mpGuts->mColorAttachments.size(); ++i)
	{
		glDeleteTextures(1, & mpGuts->mColorAttachments[i]);
	}

	mpGuts->mColorAttachments.clear();
	mpGuts->mColorAttachmentInternalFormat.clear();

	glDeleteTextures(1, & mpGuts->mDepthTextureId);
}

//----------------------------------------------------------------------------
/*Cette méthode sert a copier le fbo et d'y allouer de nouvelle ressources
  opengl (renderBuffer et textures). On ne peut pas utiliser le partage
  implicite sur cette classe et créer une nouvelle instance de fbo qui aurait
  le meme id (ainsi que les memes id de frameBuffer, renderBuffer, texture)
  parce que lors de la destruction du fbo initiale, les ressources opengl serait
  detruites causant ainsi de grâve problème à la copy. C'est pourquoi la méthode
  copy ré-alloue completement un nouveau FrameBuffer!*/
MultisampleFrameBufferObject MultisampleFrameBufferObject::copy()
{
	MultisampleFrameBufferObject fbo;
	fbo.setNumberOfSamples(getNumberOfSamples());
    fbo.resize(getWidth(), getHeight());
    for (int i = 0; i < getNumColorAttachment(); ++i)
        fbo.addColorAttachment( getColorAttachmentInternalFormat(i) );

    if (isDepthAttachmentUsed())
        fbo.addDepthAttachment();

    fbo.mpGuts->mPreviousFrameBuffers = mpGuts->mPreviousFrameBuffers;
    return fbo;
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::deleteGuts()
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
void MultisampleFrameBufferObject::addColorAttachment()
{ addColorAttachment(GL_RGBA8); }

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::addColorAttachment(GLenum iInternalFormat)
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

	//grab previous framebuffer
    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

    int index = (int)mpGuts->mColorAttachments.size();
    GLenum e = GL_COLOR_ATTACHMENT0 + index;

	////--- generate multisample texture
	glEnable(GL_TEXTURE_2D);

	GLint previousTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);		
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, getNumberOfSamples(), iInternalFormat, getWidth(), getHeight(), hasFixedSampleLocations() );
	glFramebufferTexture2D(GL_FRAMEBUFFER, e, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
	mpGuts->mColorAttachments.push_back(tex);
	mpGuts->mColorAttachmentInternalFormat.push_back(iInternalFormat);

    validate();

	glBindTexture(GL_TEXTURE_2D, previousTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::addDepthAttachment()
{
    init();

    //early out
    if (!getFrameBufferId() || isDepthAttachmentUsed())
        return;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);	
	mpGuts->mDepthTextureId = tex;
	refreshDepthAttachment();

    validate();
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::begin()
{
    GLint previousFboId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFboId);
    mpGuts->mPreviousFrameBuffers.push_back(previousFboId);
    if (isValid()) glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::drawTo(int iIndex)
{
    GLenum e = GL_COLOR_ATTACHMENT0 + iIndex;
    glDrawBuffersARB(1, &e);
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::end()
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
void MultisampleFrameBufferObject::init()
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
GLuint MultisampleFrameBufferObject::getColorAttachmentId(int iIndex) const
{
	GLuint r = 0;
	if(iIndex >= 0 && iIndex < (int)mpGuts->mColorAttachments.size())
	{ r = mpGuts->mColorAttachments[iIndex]; }
	return r;
}

//----------------------------------------------------------------------------
Vector2i MultisampleFrameBufferObject::getSize() const
{
    return Vector2i(getWidth(), getHeight());
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::glClear(GLenum iClearFlags)
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
bool MultisampleFrameBufferObject::isDepthAttachmentUsed() const
{
    return mpGuts->mDepthTextureId != 0;
}

//----------------------------------------------------------------------------
bool MultisampleFrameBufferObject::isValid() const
{
    return mpGuts->mIsValid;
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::makeGuts()
{
    mpGuts = new Guts();
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::refreshColorAttachment()
{
	GLint previousTex = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTex);
	for (int i = 0; i < getNumColorAttachment(); ++i)
	{
		const GLuint texId = mpGuts->mColorAttachments[i];
		GLenum e = GL_COLOR_ATTACHMENT0 + i;        

		glEnable(GL_TEXTURE_2D);		
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texId);
		glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, getNumberOfSamples(), getColorAttachmentInternalFormat(i), getWidth(), getHeight(), hasFixedSampleLocations() );
		glFramebufferTexture2D(GL_FRAMEBUFFER, e, GL_TEXTURE_2D_MULTISAMPLE, texId, 0);
	}		
	glBindTexture(GL_TEXTURE_2D, previousTex);
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::refreshDepthAttachment()
{
	GLint previousTex = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTex);
	//--- depth
	if (isDepthAttachmentUsed())
	{
		const GLuint texId = mpGuts->mDepthTextureId;
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texId);
		glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, getNumberOfSamples(), GL_DEPTH_COMPONENT, getWidth(), getHeight(), hasFixedSampleLocations() );
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, texId, 0);
	}
	glBindTexture(GL_TEXTURE_2D, previousTex);
}
//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::resolveTo(int iFromColorAttachment, FrameBufferObject iFbo, int iToColorAttachment)
{
	GLint previousFb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);

	const int fromW = getWidth(), fromH = getHeight();	
	const int toW = iFbo.getWidth(), toH = iFbo.getHeight();	

	//read from 
	GLenum from = GL_COLOR_ATTACHMENT0 + iFromColorAttachment;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getFrameBufferId()); // Make sure your multisampled FBO is the read framebuffer
	glReadBuffer(from);

	//write to
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, iFbo.getFrameBufferId());   // Make sure no FBO is set as the draw framebuffer
	GLenum to = GL_COLOR_ATTACHMENT0 + iToColorAttachment;
	glDrawBuffer(to);                       // Set the back buffer as the draw buffer
	glBlitFramebuffer(0, 0, fromW, fromH, 0, 0, toW, toH, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
/*Note: this does not work on every machine... It seems like bliting from a multisampled FBO with RGBA8
  to the back buffer RGB8 is not always supported... Depends on the driver. an error GL_INVALID_OPERATION
  is raised. It also explains why there is no issue with method resolveTo() when the destination FBO
  is RGBA.*/
void MultisampleFrameBufferObject::resolveToBackBuffer(int iFromColorAttachment)
{
	GLint previousFb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);

	const int w = getWidth(), h = getHeight();	

	//read from 
	GLenum e = GL_COLOR_ATTACHMENT0 + iFromColorAttachment;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getFrameBufferId()); // Make sure your multisampled FBO is the read framebuffer
	glReadBuffer(e);

	//write to
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);   // Make sure no FBO is set as the draw framebuffer
	glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::resize(int iWidth, int iHeight)
{
    /*On ne fait rien si les tailles sont les mêmes.*/
    if (getWidth() == iWidth && getHeight() == iHeight)
        return;

    mpGuts->mWidth = iWidth;
    mpGuts->mHeight = iHeight;

    GLint previousFb = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
    glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

	refreshColorAttachment();
	refreshDepthAttachment();	

    validate();
    glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::resize(Vector2i iSize)
{
    resize(iSize.x(), iSize.y());
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::setHasFixedSampleLocations(bool iV)
{
	if (iV != hasFixedSampleLocations())
	{
		/*For all existing color attachment... reset fixedSampleLocations and rebind
		to fbo*/
		mpGuts->mHasFixedSampleLocations = iV;

		GLint previousFb = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
		glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

		refreshColorAttachment();
		refreshDepthAttachment();

		validate();
		glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
	}
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::setNumberOfSamples(int iN)
{
	if (iN != getNumberOfSamples())
	{
		/*For all existing color attachment... reset the number of sample and rebind
		to fbo*/
		mpGuts->mNumberOfSamples = iN;

		GLint previousFb = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFb);
		glBindFramebuffer(GL_FRAMEBUFFER, getFrameBufferId());

		refreshColorAttachment();
		refreshDepthAttachment();

		validate();
		glBindFramebuffer(GL_FRAMEBUFFER, previousFb);
	}
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::shareGuts(Guts* g)
{
    mpGuts = g;
    ++mpGuts->mRefCount;
}

//----------------------------------------------------------------------------
void MultisampleFrameBufferObject::validate()
{
    mpGuts->mIsValid = false;
    QString s("Framebuffer " + QString::number(getFrameBufferId()) + " ");
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
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
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		s += "All images must have the same number of multisample samples"; break;
    default: s += "Unknow error."; break;
    }
    if (!isValid()) qDebug() << s;
}

