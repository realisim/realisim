
#include "Camera.h"
#include "FrameBufferObject.h"
#include "math/MathUtils.h"
#include "Text.h"
#include <QPainter>
#include "Shader.h"

using namespace std;
using namespace realisim;
  using namespace treeD;
  
namespace 
{
//application d'un filter 2d
	const QString kBlur =
"#version 120												\n\
#extension GL_EXT_gpu_shader4 : enable //pour textureSize2D() \n\
uniform sampler2D texture;						\n\
uniform sampler2D filter;							\n\
uniform float scale;									\n\
void main()																\n\
{																					\n\
  vec2 step = fwidth(gl_TexCoord[0].xy);	\n\
	vec4 result = vec4(0.0);								\n\
  vec2 offset;															\n\
  vec4 fValue;															\n\
  ivec2 filterSize = textureSize2D( filter, 0 );  \n\
                                                   \n\
    int fi, fj; \n\
    int i, j;  \n\
    for( j = -filterSize.y / 2, fj = 0; fj < filterSize.y; ++fj, ++j)	\n\
    for( i = -filterSize.x / 2, fi = 0; fi < filterSize.x; ++fi, ++i)	\n\
    {    																										\n\
      offset = vec2( step.x * i, step.y *j );								\n\
      fValue = texture2D(filter, vec2( fi / float(filterSize.x), fj / float(filterSize.y) ) ); 							\n\
      result += texture2D(texture, gl_TexCoord[0].xy + offset) * (fValue);	\n\
    }			\n\
  																												\n\
  gl_FragColor = vec4( 0.0, 0.0, 0.0, result.r * scale );		\n\
  //gl_FragColor = result;		\n\
}";


}

Text::Text( QString iT /* = "" */ ) : mText( iT ),
  mTexture(),
  mFont( QFont( "futura", 20 ) ),
  mFrontColor( "white" ),
  mBackgroundColor( 0, 0, 0, 0 ),
  mHasDropShadow( false )
{}

Text::Text(const Text& iT) : mText( iT.getText() ),
	mTexture( iT.getTexture() ),
  mFont( iT.getFont() ),
  mFrontColor( iT.getFrontColor() ),
  mBackgroundColor( iT.getBackgroundColor() ),
  mHasDropShadow( iT.hasDropShadow() )
{}

Text::~Text()
{ }

Text& Text::operator=(const Text& iT)
{
	mText = iT.getText();
  mTexture = iT.getTexture();
  mFont = iT.getFont();
  mFrontColor = iT.getFrontColor();
  mBackgroundColor = iT.getBackgroundColor();
  mHasDropShadow = iT.hasDropShadow();
	return *this;
}

//------------------------------------------------------------------------------
void Text::addDropShadow( bool iD )
{
	mHasDropShadow = iD;
  render();
}

//------------------------------------------------------------------------------
void Text::draw() const
{
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDisable( GL_LIGHTING );
    
  glColor4ub( 255, 255, 255, 255 );
  glBindTexture( GL_TEXTURE_2D, getTexture().getId() );
  glBegin( GL_QUADS );
  	glTexCoord2d( 0.0, 0.0 );
    glVertex2i( 0, 0 );
    glTexCoord2d( 0.0, 1.0 );
    glVertex2i( 0, getTexture().height() );
    glTexCoord2d( 1.0, 1.0 );
    glVertex2i( getTexture().width(), getTexture().height() );
    glTexCoord2d( 1.0, 0.0 );
    glVertex2i( getTexture().width(), 0 );
  glEnd();
  glEnable( GL_LIGHTING );
  glDisable( GL_BLEND );
  glDisable( GL_TEXTURE_2D );
}

//------------------------------------------------------------------------------
const QColor& Text::getBackgroundColor() const
{ return mBackgroundColor; }

//------------------------------------------------------------------------------
const QColor& Text::getFrontColor() const
{ return mFrontColor; }

//------------------------------------------------------------------------------
const QFont& Text::getFont() const
{ return mFont; }

//------------------------------------------------------------------------------
const QString& Text::getText() const
{ return mText; }

//------------------------------------------------------------------------------
Texture Text::getTexture() const
{ return mTexture;	}

//------------------------------------------------------------------------------
bool Text::hasDropShadow() const
{ return mHasDropShadow; }

//------------------------------------------------------------------------------
int Text::height() const
{ return getTexture().height(); }

//------------------------------------------------------------------------------
void Text::render() const
{
	QFontMetrics fm( getFont() );
  QRect r = fm.boundingRect( QRect(), Qt::TextExpandTabs, getText(), 0, 0 );
  QImage im( r.size(), QImage::Format_ARGB32 );
  im.fill( getBackgroundColor().rgba() );
  QPainter p( &im );
  p.setFont( getFont() );
  p.setPen( getFrontColor() );
  p.setBrush( getFrontColor() );
	p.drawText( r, Qt::TextExpandTabs, getText() );
  p.end();
  
  if( hasDropShadow() )
  {
  	QImage im2( r.size(), QImage::Format_ARGB32 );
    im2.fill( QColor( "black" ).rgba() );
    QPainter p( &im2 );
    p.setFont( getFont() );
    p.setPen( "white" );
    p.drawText( r, Qt::TextExpandTabs, getText() );
    p.end();
    
    //on met le texte en noire dans la texture
    mTexture.set( im2 );
    mTexture.setWrapMode( GL_CLAMP );
    
    FrameBufferObject fbo;
    fbo.resize( r.width(), r.height() );
    fbo.addColorAttachment( true );    
    fbo.begin();
    fbo.drawTo( 0 );
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Camera c;
    c.set( Point3d(0.0, 0.0, 5.0), 
	    Point3d(0.0, 0.0, 0.0),
      Vector3d( 0.0, 1.0, 0.0 ) );
    c.setWindowSize( r.width(), r.height() );
    c.setProjection( 0, r.width(), 
      0, r.height(), 0.5, 100.0,
      Camera::Projection::tOrthogonal );
    c.pushAndApplyMatrices();
    
    int kernelSize = 3;
    vector< float > f = meanKernel2D<float>( kernelSize );
    vector<int> fs(2, 0); fs[0] = kernelSize; fs[1] = kernelSize;
    Texture filter;
    filter.set( &f[0], fs, GL_LUMINANCE, GL_FLOAT );
    filter.setWrapMode( GL_CLAMP );
    
    Shader s;
    s.addFragmentSource( kBlur );
//    s.link();
    
    //on dessine le drop shadow
    s.begin();
    glEnable( GL_TEXTURE_2D );
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, filter.getId() );
    s.setUniform("texture", 0);
    s.setUniform("filter", 1);
    s.setUniform("scale", 8.0);
    glActiveTexture(GL_TEXTURE0);
    draw();
    glDisable( GL_TEXTURE_2D );
    s.end();
    
    //on dessine le texte.
    mTexture.set( im );
    mTexture.setFilter( GL_LINEAR );
    mTexture.setWrapMode( GL_CLAMP ); 
		draw();
    
    //on recupere le resultat
    mTexture = fbo.getTexture( 0 ).copy();
    mTexture.setFilter( GL_LINEAR );
    mTexture.setWrapMode( GL_CLAMP );
    
    c.popMatrices();
    
    glPopAttrib();
    fbo.end();
  }
  else 
  {  	
    mTexture.set( im );
    mTexture.setFilter( GL_LINEAR );
    mTexture.setWrapMode( GL_CLAMP ); 
  }
}

//------------------------------------------------------------------------------
void Text::setBackgroundColor( QColor iC )
{ 
	if(mBackgroundColor != iC)
	{mBackgroundColor = iC; render(); }
}

//------------------------------------------------------------------------------
void Text::setText( QString iT )
{
	if( mText != iT ) 
  {mText = iT; render();}
}

//------------------------------------------------------------------------------
void Text::setFont( QFont iF )
{ 
	if(mFont != iF)
	{mFont = iF; render(); }
}

//------------------------------------------------------------------------------
void Text::setFrontColor( QColor iC )
{ if(mFrontColor != iC) {mFrontColor = iC; render(); } }

//------------------------------------------------------------------------------
int Text::width() const
{ return getTexture().width(); }