/* SpriteCatalog.cpp */

#include <QFile>
#include "3d/SpriteCatalog.h"
#include "utils/utilities.h"

using namespace realisim;
using namespace math;
using namespace utils;
using namespace treeD;
using namespace std;

namespace
{
  const quint32 kHeader = 0xA0937452;
  const qint32 kOldestSupportedVersion = 1;
  const qint32 kVersion = 2;
}

treeD::Texture SpriteCatalog::mDummyTexture;
treeD::Sprite SpriteCatalog::mDummySprite;

SpriteCatalog::SpriteCatalog()
{}

SpriteCatalog::SpriteCatalog( const SpriteCatalog& iSc) :
	mSprites( iSc.mSprites ),
  mTextures( iSc.mTextures ),
  mErrors( iSc.mErrors )
{}

SpriteCatalog::~SpriteCatalog()
{}

//------------------------------------------------------------------------------
void SpriteCatalog::add( QString iToken, Texture iT )
{
	if( mTextures.find( iToken ) == mTextures.end() )
  { mTextures[iToken] = iT; }
  else 
  	addError("Impossible d'ajouter la texture " + iToken + " car elle "
    "est déjà présente dans le catalogue.");
}

//------------------------------------------------------------------------------
void SpriteCatalog::add( QString iToken, Sprite iS)
{
  if( mSprites.find( iToken ) == mSprites.end() )
  { mSprites[iToken] = new Sprite(iS); }
  else 
  	addError("Impossible d'ajouter le sprite " + iToken + " car il "
    "est déjà présent dans le catalogue.");
}

//------------------------------------------------------------------------------
void SpriteCatalog::addError( QString iE )
{ mErrors += mErrors.isEmpty() ? iE : "\n" + iE; }

//------------------------------------------------------------------------------
void SpriteCatalog::clear()
{
	mErrors = QString();
  mTextures.clear();
  map<QString, Sprite*>::iterator it = mSprites.begin();
  for(; it != mSprites.end(); ++it)
  	delete it->second;
  mSprites.clear();
}

//------------------------------------------------------------------------------
Sprite& SpriteCatalog::getSprite( QString iToken )
{
	Sprite* t = &mDummySprite;
	map<QString, Sprite*>::iterator it = mSprites.find(iToken);
	if( it != mSprites.end() )
  	t = it->second;
  return *t;
}

//------------------------------------------------------------------------------
Sprite& SpriteCatalog::getSprite(int iIndex)
{
	Sprite* t = &mDummySprite;
  int count = 0;
  map<QString, Sprite*>::iterator it = mSprites.begin();
  for( ; it != mSprites.end(); ++it )
  {
    if(count++ == iIndex)
    { t = it->second; break; }
  }
  return *t;
}

//------------------------------------------------------------------------------
vector<QString> SpriteCatalog::getSpritesForTexture( QString iToken )
{
	vector<QString> r;
  uint tId = getTexture( iToken ).getId();
  map<QString, Sprite*>::const_iterator it = mSprites.begin();
  for(; it != mSprites.end(); ++it)
  {
  	if( it->second->getTexture().getId() == tId )
    	r.push_back(it->first);
  }
  return r;
}

//------------------------------------------------------------------------------
QString SpriteCatalog::getSpriteToken( int iIndex ) const
{
	QString t;
  int count = 0;
  map<QString, Sprite*>::const_iterator it = mSprites.begin();
  for( ; it != mSprites.end(); ++it )
  {
    if(count++ == iIndex)
    { t = it->first; break; }
  } 
  return t;
}

//------------------------------------------------------------------------------
Texture& SpriteCatalog::getTexture( QString iToken )
{
	Texture* t = &mDummyTexture;
	map<QString, Texture>::iterator it = mTextures.find(iToken);
	if( it != mTextures.end() )
  	t = &(it->second);
  return *t;
}

//------------------------------------------------------------------------------
Texture& SpriteCatalog::getTexture(int iIndex)
{
	Texture* t = &mDummyTexture;
  int count = 0;
  map<QString, Texture>::iterator it = mTextures.begin();
  for( ; it != mTextures.end(); ++it )
  {
    if(count++ == iIndex)
    { t = &(it->second); break; }
  }
  return *t;
}

//------------------------------------------------------------------------------
QString SpriteCatalog::getTextureToken( int iIndex ) const
{
	QString t;
  int count = 0;
  map<QString, Texture>::const_iterator it = mTextures.begin();
  for( ; it != mTextures.end(); ++it )
  {
    if(count++ == iIndex)
    { t = it->first; break; }
  } 
  return t;
}

//------------------------------------------------------------------------------
QString SpriteCatalog::getTextureToken( const Sprite& ipS ) const
{
	uint tId = ipS.getTexture().getId();
  QString r;
  map<QString, Texture>::const_iterator it = mTextures.begin();
  for( ; it != mTextures.end(); ++it )
  {
    if(tId == it->second.getId())
    { r = (it->first); break; }
  }
  return r;
}
  
//------------------------------------------------------------------------------
QString SpriteCatalog::getAndClearLastErrors() const
{
	QString r = mErrors;
  mErrors = QString();
	return r;
}

//------------------------------------------------------------------------------
int SpriteCatalog::getNumberOfSprites() const
{ return mSprites.size(); }

//------------------------------------------------------------------------------
int SpriteCatalog::getNumberOfTextures() const
{ return mTextures.size(); }

//------------------------------------------------------------------------------
bool SpriteCatalog::hasError() const
{ return !mErrors.isEmpty(); }

//------------------------------------------------------------------------------
void SpriteCatalog::fromBinary( const QByteArray& iBa )
{
	clear();
  QByteArray ba = iBa;
  QDataStream in(&ba, QIODevice::ReadOnly);
  // Read and check the header
  quint32 header;
  in >> header;
  if (header != kHeader) { addError("Format de fichier invalide."); return; }
  qint32 version;
  in >> version;
  if( version < kOldestSupportedVersion ) { addError("Format de fichier trop "
    "vieux."); return; }

  quint32 nbTex;
  QString token;
  qint32 minFilter, magFilter, wrapR, wrapS, wrapT;
  QImage image;
  in >> nbTex;
  for( uint i = 0; i < nbTex; ++i )
  {
    in >> token;
    in >> image;
    in >> minFilter;
    in >> magFilter;
    in >> wrapR;
    in >> wrapS;
    in >> wrapT;
    
    Texture t;
    t.set( image );
    t.setMinificationFilter( (GLenum)minFilter );
    t.setMagnificationFilter( (GLenum)magFilter );
    t.setWrapRMode( (GLenum)wrapR );
    t.setWrapSMode( (GLenum)wrapS );
    t.setWrapTMode( (GLenum)wrapT );
    add( token, t );
  }

	if( version > 1 )
  {
  	quint32 nbSprites;
    QString token, texToken;
    qint32 anchor, animationDuration, nbFrames, frameGridW,
    	frameGridH;
    bool isLooping;
    QRect rect;
    
    in >> nbSprites;
    for( uint i = 0; i < nbSprites; ++i )
  	{
    	in >> token;
      in >> texToken;
      in >> anchor;
      in >> animationDuration;
      in >> isLooping;
      in >> rect;
      in >> nbFrames;
      in >> frameGridW; in >> frameGridH;
      
      Sprite s;
      Texture& t = getTexture( texToken );
      s.set(t);
      s.setAnchorPoint( (Sprite::anchor)anchor );
      s.setAnimationDuration(animationDuration);
      s.setAsLooping(isLooping);
      s.setRect(rect);
      s.setNumberOfFrames(nbFrames);
      s.setFrameGrid( frameGridW, frameGridH );
      
      add( token, s );
    }
  }
}

//------------------------------------------------------------------------------
void SpriteCatalog::removeSprite( QString iToken )
{
	map<QString, Sprite*>::iterator it = mSprites.find( iToken );
  delete it->second;
  mSprites.erase(it);
}

//------------------------------------------------------------------------------
void SpriteCatalog::removeTexture( QString iToken )
{
	vector<QString> sprites = getSpritesForTexture( iToken );
	map<QString, Texture>::iterator it = mTextures.find( iToken );
  mTextures.erase(it);
  
  for( size_t i = 0; i < sprites.size(); ++i )
  { removeSprite( sprites[i] ); }
}

//------------------------------------------------------------------------------
QByteArray SpriteCatalog::toBinary() const
{
  QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);

  //header
  out << (quint32)kHeader;
  //version courante
  out << (qint32)kVersion;
  out.setVersion(QDataStream::Qt_4_7);

  //le nombre de textures
  out << (quint32)getNumberOfTextures();
  //toutes les textures;
  map<QString, treeD::Texture>::const_iterator itTex = mTextures.begin();
  for(; itTex != mTextures.end(); ++itTex )
  {
    //le token
    out << itTex->first;
    //les données
    const Texture& t = itTex->second;
    out << t.asQImage();
    out << (qint32)t.getMinificationFilter();
    out << (qint32)t.getMagnificationFilter();
    out << (qint32)t.getWrapRMode();
    out << (qint32)t.getWrapSMode();
    out << (qint32)t.getWrapTMode();
  }

	//le nombre de sprite
  out << (quint32)getNumberOfSprites();
  //toutes les sprites
  map<QString, Sprite*>::const_iterator itSprite = mSprites.begin();
	for(; itSprite != mSprites.end(); ++itSprite)
  {
  	const Sprite& s = *(itSprite->second);
  	out << itSprite->first; //token
    out << getTextureToken(s); //token de la texture parent
    out << (qint32)s.getAnchorPoint();
    out << (qint32)s.getAnimationDuration();
    out << (bool)s.isLooping();
    out << s.getRect();
    out << s.getNumberOfFrames();
    out << (qint32)s.getFrameGrid().x();
    out << (qint32)s.getFrameGrid().y();
  }
  return r;
}

//------------------------------------------------------------------------------
