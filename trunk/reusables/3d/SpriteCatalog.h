/* SpriteCatalog.h */

#ifndef Realisim_TreeD_Sprite_Catalog_hh
#define Realisim_TreeD_Sprite_Catalog_hh

#include "3d/Sprite.h"
#include "3d/Texture.h"
#include "cassert"
/**/

namespace realisim
{
namespace utils
{

class SpriteCatalog
{
public:  
  SpriteCatalog();
  SpriteCatalog(const SpriteCatalog&);
  SpriteCatalog& operator=(const SpriteCatalog&) { assert(0); return *this; }
  virtual ~SpriteCatalog();
  
  virtual void add( QString, treeD::Texture );
  virtual void add( QString, treeD::Sprite );
  virtual void clear();
  virtual treeD::Sprite& getSprite( QString );
  virtual treeD::Sprite& getSprite( int );
virtual std::vector<QString> getSpritesForTexture( QString );
  virtual QString getSpriteToken( int ) const;
  virtual treeD::Texture& getTexture( QString );
  virtual treeD::Texture& getTexture( int );
  virtual QString getTextureToken( int ) const;
  virtual QString getAndClearLastErrors() const;
  virtual int getNumberOfSprites() const;
  virtual int getNumberOfTextures() const;
  virtual bool hasError() const;
  virtual void fromBinary( QByteArray& );
  virtual void removeSprite( QString );
  virtual void removeTexture( QString );
  virtual QByteArray toBinary() const;
  
protected:
	virtual void addError( QString );
  virtual QString getTextureToken( const treeD::Sprite& ) const;

	std::map<QString, treeD::Sprite*> mSprites;
	std::map<QString, treeD::Texture> mTextures;
  mutable QString mErrors;
  static treeD::Texture mDummyTexture;
  static treeD::Sprite mDummySprite;
};

}//utils
}//realisim

#endif