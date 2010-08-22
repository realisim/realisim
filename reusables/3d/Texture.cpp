
#include "Texture.h"

using namespace realisim;
using namespace treeD;

Texture::Guts::Guts() : mTextureId(0), mSize(), mRefCount(1)
{}

//---
Texture::Texture() : mpGuts(0)
{ makeGuts(); }

Texture::Texture(QImage i) : mpGuts(0)
{
  makeGuts();
  load(i);
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
bool Texture::isValid() const
{ return (bool)glIsTexture(getTextureId()); }

//----------------------------------------------------------------------------
void Texture::load(QImage i)
{ load(i, QRect(QPoint(0.0, 0.0), i.size())); }

//----------------------------------------------------------------------------
void Texture::load(QImage i, QRect r)
{
  copyGuts();
  
  if(r.size() != i.size())
    i = i.copy(r); 
  mpGuts->mSize = i.size();
  i = QGLWidget::convertToGLFormat(i);
  assert(getTextureId() == 0);
  glGenTextures(1, &mpGuts->mTextureId);
  glBindTexture(GL_TEXTURE_2D, getTextureId());
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i.width(), i.height(),
    0, GL_RGBA, GL_UNSIGNED_BYTE, i.bits());
//  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, i.width(), i.height(),
//                    GL_RGBA, GL_UNSIGNED_BYTE, i.bits());
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

