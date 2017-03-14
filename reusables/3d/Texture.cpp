
#include "Texture.h"

using namespace std;
using namespace realisim;
  using namespace math;
  using namespace treeD;

Texture::Guts::Guts() : mTextureId(0),
	mSize(2, 1), //init size at 1 by 1
	mType(tInvalid),
	mFormat(GL_RGBA),
	mInternalFormat(GL_RGBA8),
	mDataType(GL_UNSIGNED_BYTE),
	mMinificationFilter(GL_NEAREST),
	mMagnificationFilter(GL_NEAREST),
	mWrapSMode(GL_REPEAT),
	mWrapTMode(GL_REPEAT),
	mWrapRMode(GL_REPEAT),
    mFenceSync(0),
	mHasMipMaps(false),
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
QByteArray Texture::asBuffer( GLenum iInternalFormat, GLenum iFormat, GLenum iDt ) const
{
  char* p = 0;
  int componentPerPixel = 3, numberOfComponent = 0;
  switch( iInternalFormat )
  {
  case GL_RED: componentPerPixel = 1; break;
  case GL_RG: componentPerPixel = 2; break;
  case GL_RGB8: componentPerPixel = 3; break;
  case GL_RGBA8: componentPerPixel = 4; break;
  case GL_LUMINANCE: componentPerPixel = 1; break;
  case GL_DEPTH_COMPONENT: componentPerPixel = 4; break;
  default: assert(false && "unhandled internal format for Texture::asBuffer."); break;
  }

  switch( getType() )
  {
  case t2d: numberOfComponent = width() * height() * componentPerPixel; break;
  case t3d: numberOfComponent = width() * height() * depth() * componentPerPixel; break;
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

    GLint previousId = 0;
  switch( getType() )
  {
  case t2d:
  {
      glEnable( GL_TEXTURE_2D );
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);    
    glBindTexture( GL_TEXTURE_2D, getId() );
    glGetTexImage( GL_TEXTURE_2D, 0, iFormat, iDt, p );
    glBindTexture( GL_TEXTURE_2D, previousId );
    glDisable( GL_TEXTURE_2D );
  }
  break;
  case t3d:
  {
      glEnable( GL_TEXTURE_3D );
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);    
    glBindTexture( GL_TEXTURE_3D, getId() );
    glGetTexImage( GL_TEXTURE_3D, 0, iFormat, iDt, p );
    glBindTexture( GL_TEXTURE_3D, previousId );
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
QImage Texture::asQImage() const
{
    //GL_BGRA pour Qt
    QByteArray b = asBuffer( GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE );
  QImage r( width(), height(), QImage::Format_ARGB32 );
  memcpy( r.bits(), b.constData(), b.size() );
  //opengl et Qt on l'axe y inversé
  return r.mirrored( false, true );
}

//----------------------------------------------------------------------------
Texture Texture::copy()
{
  Texture t;
  t.mpGuts->mSize = size();
  t.mpGuts->mType = getType();
  t.mpGuts->mFormat = getFormat();
  t.mpGuts->mInternalFormat = getInternalFormat();
  t.mpGuts->mDataType = getDataType();
  t.mpGuts->mMinificationFilter = getMinificationFilter();
  t.mpGuts->mMagnificationFilter = getMagnificationFilter();
  t.mpGuts->mWrapSMode = getWrapSMode();
  t.mpGuts->mWrapTMode = getWrapTMode();
  t.mpGuts->mWrapRMode = getWrapRMode();
  
  t.set( (void*)asBuffer( getInternalFormat(), getFormat(), getDataType() ).constData(), size(),
      getInternalFormat(), getFormat(), getDataType() );

   return t;
}

//----------------------------------------------------------------------------
void Texture::deleteFenceSync()
{
    if (hasFenceSync())
    {
        glDeleteSync(getFenceSync());
        mpGuts->mFenceSync = 0;
    }
}

//----------------------------------------------------------------------------
void Texture::deleteGuts()
{
  if(mpGuts && --mpGuts->mRefCount == 0 && mpGuts->mTextureId != 0)
  {
    glDeleteTextures(1, &mpGuts->mTextureId);
    delete mpGuts;
    mpGuts = 0;
  }
}

//----------------------------------------------------------------------------
void Texture::generateMipmap(bool iUseMipMap)
{ 
	GLint previousId = 0;
	mpGuts->mHasMipMaps = iUseMipMap;
	switch (getType())
	{
	case t2d:
		glEnable(GL_TEXTURE_2D);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
		if (hasMipMaps())
		{
			glBindTexture(GL_TEXTURE_2D, getId());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			//destroy the texture and realocate it. Seems it is the only way
			//to get rid of mimaps
			QByteArray data = asBuffer(getInternalFormat(), getFormat(), getDataType());
			glDeleteTextures(1, &mpGuts->mTextureId);
			mpGuts->mTextureId = 0;

			Vector2i s(width(),height());
			set( (void*)data.constData(), s, getInternalFormat(), getFormat(), getDataType() );
		}
		glBindTexture(GL_TEXTURE_2D, previousId);
		break;
	case t3d:
		glEnable(GL_TEXTURE_3D);
		glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
		if (hasMipMaps())
		{
			glBindTexture(GL_TEXTURE_3D, getId());
			glGenerateMipmapEXT(GL_TEXTURE_3D);
		}
		else
		{
			//destroy the texture and realocate it. Seems it is the only way
			//to get rid of mimaps
			QByteArray data = asBuffer(getInternalFormat(), getFormat(), getDataType());
			glDeleteTextures(1, &mpGuts->mTextureId);
			mpGuts->mTextureId = 0;

			Vector3i s(width(), height(), depth());
			set( (void*)data.constData(), s, getInternalFormat(), getFormat(), getDataType() );
		}
		glBindTexture(GL_TEXTURE_3D, previousId);
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
const vector<int>& Texture::size() const
{return mpGuts->mSize;}

//----------------------------------------------------------------------------
int Texture::width() const
{ return getType() != tInvalid ? mpGuts->mSize[0] : 0; }

//----------------------------------------------------------------------------
int Texture::height() const
{ return getType() != tInvalid ? mpGuts->mSize[1] : 0; }

//----------------------------------------------------------------------------
int Texture::depth() const
{ return getType() == t3d ? mpGuts->mSize[2] : 0; }

//----------------------------------------------------------------------------
GLenum Texture::getWrapSMode() const
{ return mpGuts->mWrapSMode; }

//----------------------------------------------------------------------------
GLenum Texture::getWrapTMode() const
{ return mpGuts->mWrapTMode; }

//----------------------------------------------------------------------------
GLenum Texture::getWrapRMode() const
{ return mpGuts->mWrapRMode; }

//----------------------------------------------------------------------------
bool Texture::isFenceSignaled() const
{
    bool r = true;
    if (hasFenceSync())
    {
        GLint result = GL_UNSIGNALED;
        glGetSynciv(getFenceSync(), GL_SYNC_STATUS, sizeof(GLint), NULL, &result);
        r = result == GL_SIGNALED;
    }
    return r;
}

//----------------------------------------------------------------------------
bool Texture::isValid() const
{ return (bool)glIsTexture(getId()); }

//----------------------------------------------------------------------------
/*Permet de redimensionner une texture. Le contenu de la texture sera détruit.
  A priori, cette méthode peu sembler inutile puisque le contenu de la texture
  est détruit. Par contre, les framebuffer qui utilisent une texture comme
  attachement utilise cette méthode lorsque la taille du fbo change. Ainsi lors
  du prochain redessin (updateGL) le contenu de la texture est regénérée et
  la texture résultante possède la bonne taille.*/
void Texture::resize( const vector<int>& iS)
{
    GLint previousId = 0;
    switch (getType()) 
  {
    case t2d:
        assert( iS.size() == 2 );
      mpGuts->mSize = iS;
      glPushAttrib(GL_ENABLE_BIT);
      glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(), width(), height(),
        0, getFormat(), getDataType(), 0);
      glBindTexture(GL_TEXTURE_2D, previousId);
      glPopAttrib();
      break;
    case t3d:
        assert( iS.size() == 3 );
      mpGuts->mSize = iS;
      glPushAttrib(GL_ENABLE_BIT);
      glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glTexImage3D(GL_TEXTURE_3D, 0, getInternalFormat(), width(), height(),
        depth(), 0, getFormat(), getDataType(), 0);
      glBindTexture(GL_TEXTURE_3D, previousId);
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
void Texture::setFenceSync(GLsync iFs)
{
    if (hasFenceSync())
    {
        deleteFenceSync();
    }
    mpGuts->mFenceSync = iFs;
}


//----------------------------------------------------------------------------
void Texture::set(QImage i, GLenum iInternalFormat,
    GLenum iFormat,
    GLenum iDataType)
{
    if(!i.isNull())
  { i = QGLWidget::convertToGLFormat(i); }

  vector<int> s;
  s.push_back( i.width() ); s.push_back( i.height() ); 
  set( i.bits(), s, iInternalFormat, iFormat, iDataType );
}

//----------------------------------------------------------------------------
void Texture::set(void* iPtr, const Vector2i& iS,
    GLenum iInternalFormat,
    GLenum iFormat,
    GLenum iDataType)
{
    vector<int> s(2, 0); s[0] = iS.x(); s[1] = iS.y();
  set( iPtr, s, iInternalFormat, iFormat, iDataType );
}

//----------------------------------------------------------------------------
void Texture::set(void* iPtr, const Vector3i& iS,
    GLenum iInternalFormat,
    GLenum iFormat,
    GLenum iDataType)
{
    vector<int> s(3, 0); s[0] = iS.x(); s[1] = iS.y(); s[2] = iS.z();
  set( iPtr, s, iInternalFormat, iFormat, iDataType );
}

//----------------------------------------------------------------------------
void Texture::set(void* iPtr, const vector<int>& iS,
    GLenum iInternalFormat,
    GLenum iFormat,
    GLenum iDataType)
{
    GLint previousId = 0;
    switch ( iS.size() ) 
  {
    case 2:
      if(!isValid()) glGenTextures(1, &mpGuts->mTextureId);
      
      setType(t2d);
      setDataType(iDataType);
      setInternalFormat(iInternalFormat);
      setFormat(iFormat);      
      resize( iS );
        
      glPushAttrib(GL_ENABLE_BIT);
      glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
      glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinificationFilter() );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagnificationFilter() );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapTMode() );
            
      glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(), width(), height(),
        0, getFormat(), getDataType(), iPtr );
      glBindTexture(GL_TEXTURE_2D, previousId);
      glPopClientAttrib();
      glPopAttrib();
      break;
    case 3:
      if( !isValid() ) glGenTextures(1, &mpGuts->mTextureId);
        
      setType(t3d);
      setDataType(iDataType);
      setInternalFormat(iInternalFormat);
      setFormat(iFormat);
      resize( iS );
      
      glPushAttrib(GL_ENABLE_BIT);
      glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
      glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, getMinificationFilter() );
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, getMagnificationFilter() );
      glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, getWrapTMode() );
      glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, getWrapRMode() );
            
      glTexImage3D(GL_TEXTURE_3D, 0, getInternalFormat(), width(), height(),
        depth(), 0, getFormat(), getDataType(), iPtr);

      glBindTexture(GL_TEXTURE_3D, previousId);
      glPopClientAttrib();
      glPopAttrib();
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setFilter( GLenum iF )
{ setFilter( iF, iF );    }

//----------------------------------------------------------------------------
void Texture::setFilter( GLenum iMin, GLenum iMag )
{
    setMinificationFilter( iMin );
  setMagnificationFilter( iMag );
}
//----------------------------------------------------------------------------
void Texture::setMinificationFilter( GLenum iI )
{
    GLint previousId = 0;
    mpGuts->mMinificationFilter = iI;
  switch (getType()) 
  {
    case t2d:
            glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
          getMinificationFilter() );
      glBindTexture(GL_TEXTURE_2D, previousId);
      break;
    case t3d:
            glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,
          getMinificationFilter() );
      glBindTexture(GL_TEXTURE_3D, previousId);
      break;
    default:
      break;
  }
}
//----------------------------------------------------------------------------
void Texture::setMagnificationFilter( GLenum iI )
{
    GLint previousId = 0;
    mpGuts->mMagnificationFilter = iI;
  switch (getType()) 
  {
    case t2d:
            glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
          getMagnificationFilter() );
      glBindTexture(GL_TEXTURE_2D, previousId);
      break;
    case t3d:
            glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,
          getMagnificationFilter() );
      glBindTexture(GL_TEXTURE_3D, previousId);
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
    GLint previousId = 0;
    mpGuts->mWrapSMode = iS;
  switch (getType()) 
  {
    case t2d:
            glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glBindTexture(GL_TEXTURE_2D, previousId);
      break;
    case t3d:
            glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, getWrapSMode() );
      glBindTexture(GL_TEXTURE_3D, previousId);
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setWrapTMode( GLenum iT )
{
    GLint previousId = 0;
    mpGuts->mWrapTMode = iT;
  switch (getType()) 
  {
    case t2d:
            glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapTMode() );
      glBindTexture(GL_TEXTURE_2D, previousId);
      break;
    case t3d:
            glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, getWrapTMode() );
      glBindTexture(GL_TEXTURE_3D, previousId);
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void Texture::setWrapRMode( GLenum iR )
{
    GLint previousId = 0;
    mpGuts->mWrapRMode = iR;
  switch (getType()) 
  {
    case t2d:
            glEnable(GL_TEXTURE_2D);
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);
      glBindTexture(GL_TEXTURE_2D, getId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, getWrapRMode() );
      glBindTexture(GL_TEXTURE_2D, previousId);
      break;
    case t3d:
            glEnable(GL_TEXTURE_3D);
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousId);
      glBindTexture(GL_TEXTURE_3D, getId());
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, getWrapRMode() );
      glBindTexture(GL_TEXTURE_3D, previousId);
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

