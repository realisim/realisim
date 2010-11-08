
#include "Texture.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;

Texture::Guts::Guts() : mTextureId(0),
  mSize(),
  mType(tInvalid),
  mFormat(GL_RGBA),
  mDataType(GL_UNSIGNED_BYTE),
  mInterpolation(GL_LINEAR),
  mRefCount(1)
{}

//---
Texture::Texture() : mpGuts(0)
{ makeGuts(); }

Texture::Texture(QImage i, GLenum iF/*= GL_RGBA*/,
  GLenum iDt /*= GL_UNSIGNED_BYTE*/, GLenum iInt /*GL_LINEAR*/) : mpGuts(0)
{
  makeGuts();
	set(i, iF, iDt, iInt);
}

Texture::Texture(void* iPtr, const Vector3i& iSize, GLenum iF/*= GL_RGBA*/,
  GLenum iDt /*= GL_UNSIGNED_BYTE*/, GLenum iInt /*GL_LINEAR*/)
{
  makeGuts();
  set(iPtr, iSize, iF, iDt, iInt);
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
//void Texture::copyGuts()
//{
//	/*expliquer le truc ici... g = mpGuts et ensuite deleteGuts, on pourrait
//    penser que ce code est bugger parce qu'apres le deleteGuts on utilise
//    le pointeur g et que celui ci pourrait ne plus être valide. Ca ne sera 
//    jamais le cas, puisque ce code est dans une condition qui s'assure qu'il
//    y a au moin 2 references, donc on peut en deleter un et le pointeur g
//    sera toujours valide. Dans le cas ou il n'y a qu'une seule reference, on
//    ne veut pas copier les guts. On copie les guts seulement quand un client
//    tente de modifier (par le biais d'une méthode non const) un guts qui a
//    plus d'une reference.*/
//  if(mpGuts->mRefCount > 1)
//  {
//    Guts* g = mpGuts;
//    deleteGuts();
//    makeGuts();
//    mpGuts->mTextureId = g->mTextureId;
//    mpGuts->mSize = g->mSize;
//    mpGuts->mType = g->mType;
//    mpGuts->mFormat = g->mFormat;
//    mpGuts->mDataType = g->mDataType;
//    mpGuts->mInterpolation = g->mInterpolation;
//  }
//}

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
/*Permet de redimensionner une texture. Le contenu de la texture sera détruit.
  A priori, cette méthode peu sembler inutile puisque le contenu de la texture
  est détruit. Par contre, les framebuffer qui utilisent une texture comme
  attachement utilise cette méthode lorsque la taille du fbo change. Ainsi lors
  du prochain redessin (updateGL) le contenu de la texture est regénérée et
  la texture résultante possède la bonne taille.*/
void Texture::resize(int iWidth, int iHeight)
{
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
	mpGuts->mSize.setXYZ(iWidth, iHeight, 0);
  glBindTexture(GL_TEXTURE_2D, getTextureId());
  glTexImage2D(GL_TEXTURE_2D, 0, getFormat(), iWidth, iHeight,
    0, getFormat(), getDataType(), 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopAttrib();
}

//----------------------------------------------------------------------------
void Texture::set(QImage i, GLenum iF/*= GL_RGBA*/,
  GLenum iDt /*= GL_UNSIGNED_BYTE*/, GLenum iInt /*GL_LINEAR*/)
{ set(i, QRect(QPoint(0, 0), i.size()), iF, iDt, iInt); }
		
//----------------------------------------------------------------------------
void Texture::set(QImage i, QRect r, GLenum iF/*= GL_RGBA*/,
  GLenum iDt /*= GL_UNSIGNED_BYTE*/, GLenum iInt /*GL_LINEAR*/)
{
  //copyGuts();
	//redimensionne l'image a la taille de r
  if(r.size() != i.size())
    i = i.copy(r); 
  mpGuts->mSize.setXYZ(r.width(), r.height(), 0);
  if(!i.isNull())
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
  setFormat(iF);
  setInterpolation(iInt);
    
  glPushAttrib(GL_ENABLE_BIT);
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, getTextureId());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, getInterpolation());
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, getInterpolation());
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        
  glTexImage2D(GL_TEXTURE_2D, 0, getFormat(), i.width(), i.height(),
    0, getFormat(), getDataType(), i.isNull() ? 0 : i.bits());
  //glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopClientAttrib();
  glPopAttrib();
}

//----------------------------------------------------------------------------
void Texture::set(void* iPtr, const Vector3i& iSize, GLenum iF/*= GL_RGBA*/,
  GLenum iDt /*= GL_UNSIGNED_BYTE*/, GLenum iInt /*= GL_LINEAR*/)
{
  //copyGuts();
  
  mpGuts->mSize = iSize;
  assert(getTextureId() == 0); 
  if(!glIsTexture(getTextureId()))
    glGenTextures(1, &mpGuts->mTextureId);
    
  setType(t3d);
  setDataType(iDt);
  setFormat(iF);
  setInterpolation(iInt);
  
  glPushAttrib(GL_ENABLE_BIT);
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, getTextureId());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, getInterpolation());
  glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, getInterpolation());
  glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
        
  glTexImage3D(GL_TEXTURE_3D, 0, getFormat(), iSize.getX(), iSize.getY(),
    iSize.getZ(), 0, getFormat(), getDataType(), iPtr);

  glBindTexture(GL_TEXTURE_3D, 0);
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

