
#include "Texture.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
  
namespace 
{
  GLenum toGLFormat(Texture::dataType iDt)
  {
    GLenum r = GL_UNSIGNED_BYTE;
    switch (iDt) 
    {
    case Texture::dtShort: r = GL_SHORT; break;
    case Texture::dtUnsignedByte: r = GL_UNSIGNED_BYTE; break;
    case Texture::dtUnsignedShort: r = GL_UNSIGNED_SHORT; break;
    default: assert(0); break;
    }
    return r;
  }
  
  GLenum toGLFormat(Texture::format iF)
  {
    GLenum r = GL_RGBA;
    switch (iF) 
    {
    case Texture::fLuminance: r = GL_LUMINANCE; break;
    case Texture::fRgba: r = GL_RGBA; break;
    default: assert(0); break;
    } 
    return r;
  }

  
}

Texture::Guts::Guts() : mTextureId(0),
  mSize(),
  mType(tInvalid),
  mFormat(fRgba),
  mDataType(dtUnsignedByte),
  mRefCount(1)
{}

//---
Texture::Texture() : mpGuts(0)
{ makeGuts(); }

Texture::Texture(QImage i, format iF/*= fRgba*/,
  dataType iDt /*= dtUnsignedByte*/) : mpGuts(0)
{
  makeGuts();
  set(i, iF, iDt);
}

Texture::Texture(void* iPtr, const Vector3i& iSize, format iF/*= fRgba*/,
  dataType iDt /*= dtUnsignedByte*/)
{
  makeGuts();
  set(iPtr, iSize, iF, iDt);
}

Texture::Texture(const Texture& iT) : mpGuts(0)
{ shareGuts(iT.mpGuts); }

Texture::~Texture()
{ deleteGuts(); }

Texture& Texture::operator=(const Texture& iT)
{
  if(mpGuts == iT.mpGuts)
    return *this;
  
  deleteGuts();
  shareGuts(iT.mpGuts);
  return *this;
}

//----------------------------------------------------------------------------
void Texture::copyGuts()
{
	/*expliquer le truc ici... g = mpGuts et ensuite deleteGuts, on pourrait
    penser que ce code est bugger parce qu'apres le deleteGuts on utilise
    le pointeur g et que celui ci pourrait ne plus être valide. Ca ne sera 
    jamais le cas, puisque ce code est dans une condition qui s'assure qu'il
    y a au moin 2 references, donc on peut en deleter un et le pointeur g
    sera toujours valide. Dans le cas ou il n'y a qu'une seule reference, on
    ne veut pas copier les guts. On copie les guts seulement quand un client
    tente de modifier (par le biais d'une méthode non const) un guts qui a
    plus d'une reference.*/
  if(mpGuts->mRefCount > 1)
  {
    Guts* g = mpGuts;
    deleteGuts();
    makeGuts();
    mpGuts->mTextureId = g->mTextureId;
    mpGuts->mSize = g->mSize;
    mpGuts->mType = g->mType;
    mpGuts->mFormat = g->mFormat;
    mpGuts->mDataType = g->mDataType;
  }
}

//----------------------------------------------------------------------------
void Texture::deleteGuts()
{
  if(mpGuts && --mpGuts->mRefCount == 0)
  {
    glDeleteTextures(1, &mpGuts->mTextureId);
    delete mpGuts;
    mpGuts = 0;
  }
}

//----------------------------------------------------------------------------
const Vector3i& Texture::getSize() const
{return mpGuts->mSize;}

//----------------------------------------------------------------------------
bool Texture::isValid() const
{ return (bool)glIsTexture(getTextureId()); }

//----------------------------------------------------------------------------
void Texture::set(QImage i, format iF/*= fRgba*/,
  dataType iDt /*= dtUnsignedByte*/)
{ set(i, QRect(QPoint(0.0, 0.0), i.size()), iF, iDt); }

//----------------------------------------------------------------------------
void Texture::set(QImage i, QRect r, format iF/*= fRgba*/,
  dataType iDt /*= dtUnsignedByte*/)
{
  copyGuts();
  
  //redimensionne l'image a la taille de r
  if(r.size() != i.size())
    i = i.copy(r); 
  mpGuts->mSize.setXYZ(i.width(), i.height(), 0);
  i = QGLWidget::convertToGLFormat(i);
  
  /*Si le assert est déclanché:
    set a été appeler plus d'une fois sur la même texture... Aucune ressource
    opengl n'est perdue, mais on peut se demander pourquoi faire set sur
    une texture plus d'une fois...*/
  assert(getTextureId() == 0); 
  if(!glIsTexture(getTextureId()))
    glGenTextures(1, &mpGuts->mTextureId);
  
  setType(t2d);
  setDataType(iDt);
  GLenum dataType = toGLFormat(iDt);
  setFormat(iF);
  GLenum format = toGLFormat(iF);
    
  glPushAttrib(GL_ENABLE_BIT);
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, getTextureId());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        
  glTexImage2D(GL_TEXTURE_2D, 0, format, i.width(), i.height(),
    0, format, dataType, i.bits());
//  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, i.width(), i.height(),
//                    GL_RGBA, GL_UNSIGNED_BYTE, i.bits());
  glPopClientAttrib();
  glPopAttrib();
}

//----------------------------------------------------------------------------
void Texture::set(void* iPtr, const Vector3i& iSize, format iF/*= fRgba*/,
  dataType iDt /*= dtUnsignedByte*/)
{
  copyGuts();
  
  mpGuts->mSize = iSize;
  assert(getTextureId() == 0); 
  if(!glIsTexture(getTextureId()))
    glGenTextures(1, &mpGuts->mTextureId);
    
  setType(t3d);
  setDataType(iDt);
  GLenum dataType = toGLFormat(iDt);
  setFormat(iF);
  GLenum format = toGLFormat(iF);
  
  glPushAttrib(GL_ENABLE_BIT);
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, getTextureId());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
        
  glTexImage3D(GL_TEXTURE_3D, 0, format, iSize.getX(), iSize.getY(),
    iSize.getZ(), 0, format, dataType, iPtr);
//  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, i.width(), i.height(),
//                    GL_RGBA, GL_UNSIGNED_BYTE, i.bits());
  glPopClientAttrib();
  glPopAttrib();
}

//----------------------------------------------------------------------------
void Texture::makeGuts()
{ mpGuts = new Guts(); }

//----------------------------------------------------------------------------
void Texture::shareGuts(Guts* g)
{
  mpGuts = g;
  ++mpGuts->mRefCount;
}

