
#include "Texture.h"

using namespace std;
using namespace realisim;
  using namespace math;
  using namespace treeD;

Texture::Guts::Guts() : mTextureId(0),
  mSize(),
  mType(tInvalid),
  mFormat(GL_RGBA),
  mDataType(GL_UNSIGNED_BYTE),
  mMinificationFilter(GL_NEAREST),
  mMagnificationFilter(GL_NEAREST),
  mWrapSMode( GL_REPEAT ),
  mWrapTMode( GL_REPEAT ),
  mWrapRMode( GL_REPEAT ),
  mRefCount(1)
{}

//---
Texture::Texture() : mpGuts(0)
{ makeGuts(); }

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
QByteArray Texture::asBuffer( GLenum iF, GLenum iDt ) const
{
  char* p = 0;
  int componentPerPixel = 3, numberOfComponent = 0;
  switch( iF )
  {
  case GL_BGR: componentPerPixel = 3; break;
  case GL_BGRA: componentPerPixel = 4; break;
  case GL_RGB: componentPerPixel = 3; break;
  case GL_RGBA: componentPerPixel = 4; break;
  case GL_LUMINANCE: componentPerPixel = 1; break;
  default: break;
  }

  switch( getType() )
  {
  case t2d: numberOfComponent = getSizeX() * getSizeY() * componentPerPixel; break;
  case t3d: numberOfComponent = getSizeX() * getSizeY() * getSizeZ() * componentPerPixel; break;
  default: break;
  }   

  switch( iDt )
  {
  case GL_BYTE: numberOfComponent *= sizeof(char); break;
  case GL_UNSIGNED_BYTE: numberOfComponent *= sizeof(unsigned char); break;
  case GL_SHORT: numberOfComponent *= sizeof(short); break;
  case GL_UNSIGNED_SHORT: numberOfComponent *= sizeof(unsigned short); break;
  case GL_INT: numberOfComponent *= sizeof(int); break;
  case GL_UNSIGNED_INT: numberOfComponent *= sizeof(unsigned int); break;
  case GL_FLOAT: numberOfComponent *= sizeof(float); break;
  case GL_2_BYTES: break;
  case GL_3_BYTES: break;
  case GL_4_BYTES: break;
  case GL_DOUBLE: numberOfComponent *= sizeof(double); break;
  default: break;
  }
  p = new char[ numberOfComponent ];

  switch( getType() )
  {
  case t2d:
  {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, getTextureId() );
    glGetTexImage( GL_TEXTURE_2D, 0, getFormat(), getDataType(), p );
    glBindTexture( GL_TEXTURE_2D, 0 );
    glDisable( GL_TEXTURE_2D );
  }
  break;
  case t3d:
  {
    glEnable( GL_TEXTURE_3D );
    glBindTexture( GL_TEXTURE_3D, getTextureId() );
    glGetTexImage( GL_TEXTURE_3D, 0, getFormat(), getDataType(), p );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glDisable( GL_TEXTURE_3D );
  }
  break;
  default: break;
  }

	QByteArray r( p, numberOfComponent );
  if( p ) { delete[] p; p = 0; };  
  return r;
}

//----------------------------------------------------------------------------
QImage Texture::asImage() const
{
	//GL_BGRA pour Qt
	QByteArray b = asBuffer( GL_BGRA, GL_UNSIGNED_BYTE );
  QImage r( getSizeX(), getSizeY(), QImage::Format_ARGB32 );
  memcpy( r.bits(), b.data(), b.size() );
  //opengl et Qt on l'axe y inversé
  return r.mirrored( false, true );
}

//----------------------------------------------------------------------------
Texture Texture::copy()
{
  Texture t;
  t.mpGuts->mSize = getSize();
  t.mpGuts->mType = getType();
  t.mpGuts->mFormat = getFormat();
  t.mpGuts->mDataType = getDataType();
  t.mpGuts->mMinificationFilter = getMinificationFilter();
  t.mpGuts->mMagnificationFilter = getMagnificationFilter();
  t.mpGuts->mWrapSMode = getWrapSMode();
  t.mpGuts->mWrapTMode = getWrapTMode();
  t.mpGuts->mWrapRMode = getWrapRMode();
  t.set( asBuffer( getFormat(), getDataType() ).data(), getSize(),
  	getFormat(), getDataType() );
      
   //void* p = 0;
//   int componentPerPixel = 3, numberOfComponent = 0;
//   switch( t.getFormat() )
//   {
//   case GL_BGR: componentPerPixel = 3; break;
//   case GL_BGRA: componentPerPixel = 4; break;
//   case GL_RGB: componentPerPixel = 3; break;
//   case GL_RGBA: componentPerPixel = 4; break;
//   case GL_LUMINANCE: componentPerPixel = 1; break;
//   default: break;
//   }
//   
//   switch( t.getType() )
//   {
//   case t2d: numberOfComponent = getSizeX() * getSizeY() * componentPerPixel; break;
//   case t3d: numberOfComponent = getSizeX() * getSizeY() * getSizeZ() * componentPerPixel; break;
//   default: break;
//   }   
//   
//   switch( getDataType() )
//   {
//   case GL_BYTE: p = new char[ numberOfComponent ]; break;
//   case GL_UNSIGNED_BYTE: p = new unsigned char[ numberOfComponent ]; break;
//   case GL_SHORT: p = new short[ numberOfComponent ]; break;
//   case GL_UNSIGNED_SHORT: p = new unsigned short[ numberOfComponent ]; break;
//   case GL_INT: p = new int[ numberOfComponent ]; break;
//   case GL_UNSIGNED_INT: p = new unsigned int[ numberOfComponent ]; break;
//   case GL_FLOAT: p = new float[ numberOfComponent ]; break;
//   case GL_2_BYTES: break;
//   case GL_3_BYTES: break;
//   case GL_4_BYTES: break;
//   case GL_DOUBLE: p = new double[ numberOfComponent ]; break;
//   default: break;
//   }     
//   
//   switch( t.getType() )
//   {
//   case t2d:
//   {
//      glEnable( GL_TEXTURE_2D );
//      glBindTexture( GL_TEXTURE_2D, getTextureId() );
//      glGetTexImage( GL_TEXTURE_2D, 0, getFormat(), getDataType(), p );
//      t.set( p, t.getSize(), t.getFormat(), t.getDataType() );
//      glBindTexture( GL_TEXTURE_2D, 0 );
//      glDisable( GL_TEXTURE_2D );
//   }
//   break;
//   case t3d:
//   {
//      glEnable( GL_TEXTURE_3D );
//      glBindTexture( GL_TEXTURE_3D, getTextureId() );
//      glGetTexImage( GL_TEXTURE_3D, 0, getFormat(), getDataType(), p );
//      t.set( p, t.getSize(), t.getFormat(), t.getDataType() );
//      glBindTexture( GL_TEXTURE_3D, 0 );
//      glDisable( GL_TEXTURE_3D );
//   }
//   break;
//   default: break;
//   }
//   
//   if( p ) { delete[] p; p = 0; };
   return t;
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
void Texture::generateMipmap()
{ 
	switch( getType() )
  {
    case t2d:
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glGenerateMipmap(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
    break;
    case t3d:
      glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glGenerateMipmapEXT(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, 0);
    break;
  	default: break;
  }
}

//----------------------------------------------------------------------------
GLenum Texture::getMagnificationFilter() const
{ return mpGuts->mMagnificationFilter; }

//----------------------------------------------------------------------------
GLenum Texture::getMinificationFilter() const
{ return mpGuts->mMinificationFilter; }

//----------------------------------------------------------------------------
const vector<int>& Texture::getSize() const
{return mpGuts->mSize;}

//----------------------------------------------------------------------------
int Texture::getSizeX() const
{ return getType() != tInvalid ? mpGuts->mSize[0] : 0; }

//----------------------------------------------------------------------------
int Texture::getSizeY() const
{ return getType() != tInvalid ? mpGuts->mSize[1] : 0; }

//----------------------------------------------------------------------------
int Texture::getSizeZ() const
{ return getType() == t3d ? mpGuts->mSize[2] : 0; }

//----------------------------------------------------------------------------
GLenum Texture::getWrapSMode() const
{ return mpGuts->mWrapSMode; }

//----------------------------------------------------------------------------
GLenum Texture::getWrapTMode() const
{ return mpGuts->mWrapSMode; }

//----------------------------------------------------------------------------
GLenum Texture::getWrapRMode() const
{ return mpGuts->mWrapSMode; }

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
void Texture::resize( const vector<int>& iS)
{
	switch (getType()) 
  {
    case t2d:
    	assert( iS.size() == 2 );
      mpGuts->mSize = iS;
      glPushAttrib(GL_ENABLE_BIT);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glTexImage2D(GL_TEXTURE_2D, 0, getFormat(), getSizeX(), getSizeY(),
        0, getFormat(), getDataType(), 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glPopAttrib();
      break;
    case t3d:
	    assert( iS.size() == 3 );
      mpGuts->mSize = iS;
      glPushAttrib(GL_ENABLE_BIT);
      glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glTexImage3D(GL_TEXTURE_3D, 0, getFormat(), getSizeX(), getSizeY(),
        getSizeZ(), 0, getFormat(), getDataType(), 0);
      glBindTexture(GL_TEXTURE_3D, 0);
      glPopAttrib();
    	break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
//voir resize( std::vector<int> )
void Texture::resize(int iW, int iH )
{
	vector<int> s;
  s.push_back( iW ); s.push_back( iH ); resize( s );
}

//----------------------------------------------------------------------------
//voir resize( std::vector<int> )
void Texture::resize(int iW, int iH, int iD )
{
	vector<int> s;
  s.push_back( iW ); s.push_back( iH ); s.push_back( iD ); resize( s );
}

//----------------------------------------------------------------------------
void Texture::set(QImage i, GLenum iF/*= GL_RGBA*/ )
{
	if(!i.isNull())
  { i = QGLWidget::convertToGLFormat(i); }
  vector<int> s;
  s.push_back( i.width() ); s.push_back( i.height() ); 
  set( i.bits(), s, iF, GL_UNSIGNED_BYTE );    
}

//----------------------------------------------------------------------------
void Texture::set(void* iPtr, const vector<int>& iS, GLenum iF/*= GL_RGBA*/,
  GLenum iDt /*= GL_UNSIGNED_BYTE*/ )
{
	switch ( iS.size() ) 
  {
    case 2:
      if(!isValid()) glGenTextures(1, &mpGuts->mTextureId);
      
      setType(t2d);
      setDataType(iDt);
      setFormat(iF);      
      resize( iS );
        
      glPushAttrib(GL_ENABLE_BIT);
      glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinificationFilter() );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagnificationFilter() );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapTMode() );
            
      glTexImage2D(GL_TEXTURE_2D, 0, getFormat(), getSizeX(), getSizeY(),
        0, getFormat(), getDataType(), iPtr );
      glBindTexture(GL_TEXTURE_2D, 0);
      glPopClientAttrib();
      glPopAttrib();
      break;
    case 3:
      if( !isValid() ) glGenTextures(1, &mpGuts->mTextureId);
        
      setType(t3d);
      setDataType(iDt);
      setFormat(iF);      
      resize( iS );
      
      glPushAttrib(GL_ENABLE_BIT);
      glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
      glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, getMinificationFilter() );
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, getMagnificationFilter() );
      glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, getWrapTMode() );
      glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, getWrapRMode() );
            
      glTexImage3D(GL_TEXTURE_3D, 0, getFormat(), getSizeX(), getSizeY(),
        getSizeZ(), 0, getFormat(), getDataType(), iPtr);

      glBindTexture(GL_TEXTURE_3D, 0);
      glPopClientAttrib();
      glPopAttrib();
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setFilter( GLenum iF )
{ setFilter( iF, iF );	}

//----------------------------------------------------------------------------
void Texture::setFilter( GLenum iMin, GLenum iMag )
{
	setMinificationFilter( iMin );
  setMagnificationFilter( iMag );
}
//----------------------------------------------------------------------------
void Texture::setMinificationFilter( GLenum iI )
{
	mpGuts->mMinificationFilter = iI;
  switch (getType()) 
  {
    case t2d:
			glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
      	getMinificationFilter() );
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case t3d:
			glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,
      	getMinificationFilter() );
      glBindTexture(GL_TEXTURE_3D, 0);
      break;
    default:
      break;
  }
}
//----------------------------------------------------------------------------
void Texture::setMagnificationFilter( GLenum iI )
{
	mpGuts->mMagnificationFilter = iI;
  switch (getType()) 
  {
    case t2d:
			glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
      	getMagnificationFilter() );
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case t3d:
			glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,
      	getMagnificationFilter() );
      glBindTexture(GL_TEXTURE_3D, 0);
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setWrapMode( GLenum iW )
{ setWrapMode( iW, iW, iW ); }

//----------------------------------------------------------------------------
void Texture::setWrapMode( GLenum iS, GLenum iT )
{ setWrapSMode( iS ); setWrapTMode( iT ); }

//----------------------------------------------------------------------------
void Texture::setWrapMode( GLenum iS, GLenum iT, GLenum iR )
{ setWrapSMode( iS ); setWrapTMode( iT ); setWrapRMode( iR ); }

//----------------------------------------------------------------------------
void Texture::setWrapSMode( GLenum iS )
{
	mpGuts->mWrapSMode = iS;
  switch (getType()) 
  {
    case t2d:
			glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case t3d:
			glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glBindTexture(GL_TEXTURE_3D, 0);
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setWrapTMode( GLenum iT )
{
	mpGuts->mWrapTMode = iT;
  switch (getType()) 
  {
    case t2d:
			glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapTMode() );
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case t3d:
			glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, getWrapTMode() );
      glBindTexture(GL_TEXTURE_3D, 0);
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setWrapRMode( GLenum iR )
{
	mpGuts->mWrapRMode = iR;
  switch (getType()) 
  {
    case t2d:
			glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getTextureId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, getWrapRMode() );
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case t3d:
			glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, getTextureId());
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, getWrapRMode() );
      glBindTexture(GL_TEXTURE_3D, 0);
      break;
    default:
      break;
  }
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

