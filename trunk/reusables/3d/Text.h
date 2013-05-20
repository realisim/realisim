
#ifndef Realisim_TreeD_Text_hh
#define Realisim_TreeD_Text_hh

#include <cassert>
#include <qgl.h>
#include <QColor>
#include <QFont>
#include <QString>
#include "Texture.h"

/*
*/

namespace realisim
{
namespace treeD
{

class Text
{
public:
  Text( QString = "" );
  Text( const Text& );
  virtual ~Text();
  virtual Text& operator=(const Text&);
  
  virtual void addDropShadow( bool );
  virtual void draw() const;
  virtual const QColor& getBackgroundColor() const;
  virtual const QColor& getFrontColor() const;
  virtual const QFont& getFont() const;
  virtual Texture getTexture() const;
  virtual const QString& getText() const;
  virtual bool hasDropShadow() const;
  virtual void setBackgroundColor( QColor );
  virtual void setText( QString );
  virtual void setFont( QFont );
  virtual void setFrontColor( QColor );
  
protected:
	virtual void render() const;
  virtual bool isRenderValid() const {return mIsRenderValid;}

  QString mText;
  mutable Texture mTexture;
  QFont mFont;
  QColor mFrontColor;
  QColor mBackgroundColor;
  bool mHasDropShadow;
	mutable bool mIsRenderValid;
};

}//treeD
}//realisim

#endif