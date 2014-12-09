/* */

#include "3d/Camera.h"
#include "3d/Sprite.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/BoundingBox.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainWindow.h"
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>
#include "3d/SpriteCatalog.h"

using namespace realisim;
	using namespace platform;
  using namespace math;
  using namespace treeD;
  using namespace utils;

namespace 
{
	const int kSpriteTableWidth = 8;
  bool kDebugCollisions = true;
  bool kDebugVisualEffects = true;
  
  const QString kShadowLightVertex = "#version 120\n"
  "uniform mat4 MVPMatrix;\n"
  "uniform mat4 MCToShadowMap;\n"
  "varying vec4 shadowCoord;\n"
  "void main()\n"
  "{\n"
  "  vec4 texCoord = MCToShadowMap * gl_Vertex;"
  "  shadowCoord = texCoord;\n"
  "  gl_Position = MVPMatrix * gl_Vertex; \n"
  "}\n";
  
  const QString kShadowLightFrag  = "#version 120\n"
  "uniform sampler2DShadow shadowMap;\n"
  "varying vec4 shadowCoord;"
  "void main()\n"
  "{\n"
  "	 vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n"  
  "  vec4 shadowCoord2 = vec4(shadowCoord.xyz / shadowCoord.w, shadowCoord.w);"
  "  bvec2 gtz = greaterThan(shadowCoord2.xy, vec2(0.0, 0.0) );\n"
  "  bvec2 lto = lessThan(shadowCoord2.xy, vec2(1.0, 1.0) );\n"
  "  if( gtz.x && lto.x && gtz.y && lto.y  ) "
  "  {\n"
  		 "if( shadowCoord2.z < shadow2D( shadowMap, shadowCoord2.xyz ).z + 0.001 )"
  "      color.rgb = vec3(1.0);\n"
  "    else { discard; } \n"
  "  }\n"
  "  else { discard; } \n"
  "  gl_FragColor = color;\n"
  "}\n";
}

Viewer::Viewer(Engine& iE, QWidget* ipParent /*=0*/) : 
	Widget3d(ipParent),
	Client(),
  mEngine( iE )
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking( true );
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::draw()
{
	switch ( mEngine.getState() ) 
  {
    case Engine::sMainMenu: drawMenu(); break;
    case Engine::sConfigureMenu: drawMenu(); break;
    case Engine::sEditing:
    	drawGame();
      break;
    case Engine::sPlaying:
    {
//      Texture lightMask = renderLights();
      drawGame();
//      const Camera::WindowInfo& wi = mEngine.getGameCamera().getViewport();
//      ScreenSpaceProjection ssp( wi.getSize() );
//      {
//        glEnable(GL_BLEND);
//        glColor4ub(15, 255, 255, 80);
//        drawRectangle( lightMask, 
//          Point2d( 0.0 ), wi.getSize() );
//        glDisable(GL_BLEND);
//      }
        
      
//      if( kDebugVisualEffects )
//      {
//        const Camera::WindowInfo& wi = mEngine.getGameCamera().getViewport();
//        ScreenSpaceProjection ssp( wi.getSize() );
//        {
//          glEnable(GL_BLEND);
//          glColor4ub(255, 255, 255, 200);
//          drawRectangle( lightMask, 
//            Point2d( 5, 5 ), Vector2d(lightMask.width(), lightMask.height()) );
//          glDisable(GL_BLEND);
//        }
//      }      
      break;
    }
    case Engine::sPaused: drawMenu(); break; 
    default: break;
  }
  
  if( kDebugVisualEffects )
  {
  	showFps();
  }
}

//-----------------------------------------------------------------------------
void Viewer::draw( const GameEntity& iA )
{
  //utils::SpriteCatalog& sc = mEngine.getSpriteCatalog();
  
  const Sprite& s = iA.getSprite();

  glDisable( GL_BLEND );
  glPushMatrix();
  glColor4ub( 255, 255, 255, 255 );
  glTranslated( iA.getPosition().x(), iA.getPosition().y(), 0.0 );
  s.draw();
  glPopMatrix();
  
  if( kDebugCollisions )
  {
  	
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //dessine le searchGrid
    {
    vector<int> cells = mEngine.getStage().getCellIndices( iA.getPosition(),
	  	iA.getCollisionSearchGrid() );
    Rectangle r;
    glColor3ub( 255, 255, 255 );
    for( int i = 0; i < (int)cells.size(); ++i )
    {
      Point2d p = toPoint(mEngine.getStage().getCellPixelCoordinate( cells[i] ));
      r.set( p, mEngine.getStage().getCellSize() );
    	drawRectangle(r);
    }
    }
    //le bounding box
    glColor3ub( 255, 255, 0 );
    drawRectangle( iA.getBoundingBox() );
    //le bounding cricle
    drawCircle( iA.getBoundingCircle().getCenter(), iA.getBoundingCircle().getRadius() );
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    //dessine les intersections entre le joueur et le terrain
    glColor3ub( 255, 0, 0);
    glPointSize(2.0);
    //const vector<Intersection2d>& intersections = iA.getIntersections();
    for( int i = 0; i < iA.getNumberOfIntersections(); ++i )
    {
    	Intersection2d x = iA.getIntersection(i);
      for( int j = 0; j < x.getNumberOfContacts(); ++j )
      {
      	drawPoint( x.getContact(j), 4 );
        
        //normal
        Point2d n = x.getContact(j) + x.getNormal(j) * 10;
        drawLine( x.getContact(j), n );
        
//        //la penetration
//        drawLine( Point2d( iA.getBoundingBox().bottomLeft().x(), 
//          iA.getBoundingBox().bottomLeft().y() - 5 ), 
//          Point2d( iA.getBoundingBox().bottomLeft().x() + iA.getIntersections().getPenetration().x(), 
//          iA.getBoundingBox().bottomLeft().y() - 5 ) );
//        drawLine( Point2d( iA.getBoundingBox().bottomLeft().x() - 5, 
//          iA.getBoundingBox().bottomLeft().y() ), 
//          Point2d( iA.getBoundingBox().bottomLeft().x() - 5, 
//          iA.getBoundingBox().bottomLeft().y() + iA.getIntersections().getPenetration().y() ) );
      }
    }    
    
    //les vecteurs forces
    glColor3ub( 0, 0, 255 );
    for( int i = 0; i < iA.getNumberOfForces(); ++i )
    { drawLine( iA.getPosition(), iA.getPosition() + iA.getForce(i) ); }
    
    //le vecteur acceleration
    glColor3ub( 0, 255, 0 );
    drawLine( iA.getPosition(), iA.getPosition() + iA.getAcceleration() );
  
    //le vecteur vitesse
    glColor3ub( 255, 0, 0 );
    drawLine( iA.getPosition(), iA.getPosition() + iA.getVelocity() );
  }
}

//-----------------------------------------------------------------------------
void Viewer::draw( const Animation& iA )
{
  glDisable( GL_BLEND );
  glPushMatrix();
  glColor4ub( 255, 255, 255, 255 );
  glTranslated( iA.getPosition().x(), iA.getPosition().y(), 0.0 );
  iA.getSprite().draw();
  glPopMatrix();
}
//-----------------------------------------------------------------------------
void Viewer::drawDataMap()
{
	const Stage& stage = mEngine.getStage();
  vector<int> visibleCells = mEngine.getVisibleCells();

  bool draw = false;
  for( size_t i = 0; i < visibleCells.size(); ++i )
  {  	
    switch ( stage.value( visibleCells[i] ) )
    {
      case Stage::ctGround: draw = true; 
        glColor3ub( 255, 255, 255 ); break;
      case Stage::ctDestructibleGround: draw = true; 
        glColor3ub( 200, 200, 200 ); break;
      case Stage::ctStart: draw = true;
        glColor3ub( 12, 12, 255 ); break;
      default: draw = false; break;
    }
    if( draw )
    {
      Vector2i t = stage.getCellPixelCoordinate( visibleCells[i] );
      drawRectangle(toPoint(t), stage.getCellSize() );
    }
  }
}

//-----------------------------------------------------------------------------
void Viewer::drawGame()
{
	glDisable( GL_DEPTH_TEST );
  treeD::SpriteCatalog& sc = mEngine.getSpriteCatalog();
  Stage& stage = mEngine.getStage();
  
	//projection ortho
  const Camera& cam = mEngine.getGameCamera();
  cam.pushAndApplyMatrices();
  
  //dessine le background
  {
  glColor4ub( 255, 255, 255, 255 );	
  Texture background = sc.getTexture( stage.getBackgroundToken() );
	if( background.isValid() )
  {
    double nbRepeatX = 1.0, nbRepeatY = 1.0;
    Vector2i origin = stage.getCellPixelCoordinate( 0 );
    Vector2i end = stage.getCellPixelCoordinate( stage.getTerrainWidth(),
      stage.getTerrainHeight() );
    Vector2i size = end - origin;
    nbRepeatX = size.x() / (double)background.width();
    nbRepeatY = size.y() / (double)background.height();
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, background.getId() );
    glBegin(GL_QUADS);
      glTexCoord2d( 0.0, 0.0 );
      glVertex2d( origin.x(), origin.y() );
      glTexCoord2d( 0.0, nbRepeatY );
      glVertex2d( origin.x(), origin.y() + size.y() );
      glTexCoord2d( nbRepeatX, nbRepeatY );
      glVertex2d( origin.x() + size.x(), origin.y() + size.y() );
      glTexCoord2d( nbRepeatX, 0.0 );
      glVertex2d( origin.x() + size.x(), origin.y() );
    glEnd();
    glDisable( GL_TEXTURE_2D );
    }
  }
  
  //dessine le data map
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glEnable( GL_BLEND );

  if( mEngine.getState() == Engine::sEditing )
  { drawDataMap(); }

  vector<int> visibleCells = mEngine.getVisibleCells();
//printf("visible cells %d\n", visibleCells.size() );  
  //dessine les layers, de la derniere a la premiere
  for( int i = stage.getNumberOfLayers() - 1; i >= 0 ; --i )
  {
  	if( !stage.isLayerVisible( i ) ){ continue; }
    
  	for( size_t j = 0; j < visibleCells.size(); ++j )
	  {
    	QString st = stage.getToken( i, visibleCells[j] );
      if( !st.isNull() )
      {
        Vector2i t = stage.getCellPixelCoordinate( visibleCells[j] );
        glPushMatrix();
        glTranslated(t.x(), t.y(), 0.0);
        sc.getSprite( st ).draw();
        glPopMatrix(); 	
      }
    }
  }
  
  //les grilles d'édition et le data map
  glColor4ub( 255, 255, 255, 120 );
  if( mEngine.getState() == Engine::sEditing )
  {
  	for( size_t i = 0; i < visibleCells.size(); ++i )
  	{
      Vector2i t = stage.getCellPixelCoordinate( visibleCells[i] );
    	glPushMatrix();
      glTranslated(t.x(), t.y(), 0.0);
      sc.getSprite("editionGrid").draw();
      glPopMatrix();
    }
  }
  
  //dessine le joueur
  draw( mEngine.getPlayer() );
  if( kDebugCollisions )
  {
  	glColor3ub( 255, 255, 255 );
    //affiche letat du jouer
    QString playerState;
    switch ( mEngine.getPlayer().getState() ) 
    {
      case Player::sIdle : playerState = "idle"; break;
      case Player::sRunningLeft : playerState = "running left"; break;
      case Player::sRunningRight : playerState = "running right"; break;
      case Player::sFalling : playerState = "falling"; break;
      case Player::sJumping : playerState = "jumping"; break;
      case Player::sHit : playerState = "hit"; break;
      default: break;
    }
    renderText(10, 30, playerState );
  }
  
  //dessine les autres acteurs
  for( int i = 0; i < stage.getNumberOfActors(); ++i )
  { 
  	const Actor& a = stage.getActor(i);
    if( mEngine.isVisible( a ) )
    { draw( a ); }
  }

	//dessine les projectiles
  for( int i = 0; i < mEngine.getNumberOfProjectiles(); ++i )
  {
  	draw( mEngine.getProjectile(i) );
  }
  
  //dessine les armes (items)
  for( int i = 0; i < stage.getNumberOfWeapons(); ++i )
  { draw( stage.getWeapon(i) ); }
	
  //les animations diverses
  for( int i = 0; i < mEngine.getNumberOfAnimations(); ++i )
  {
  	draw( mEngine.getAnimation(i) );
  }
  
	cam.popMatrices();
}


#include "Math/Primitives.h"
#include "Math/intersection.h"
//-----------------------------------------------------------------------------
void Viewer::drawMenu()
{
	//projection ortho
  Camera c = getCamera();
  c.set( Point3d( 0.0, 0.0, 5.0 ),
  	Point3d( 0.0, 0.0, 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c.setViewportSize(c.getViewport().getWidth(), c.getViewport().getHeight());
  c.setProjection( 0, c.getViewport().getWidth(),
  	0, c.getViewport().getHeight(), 0.0, 6.0, Camera::Projection::tOrthogonal );
	
	c.pushAndApplyMatrices();
  
  vector<QString> menuItems; 
  int currentMenuItem;
  switch ( mEngine.getState() ) 
  {
    case Engine::sMainMenu: 
    	menuItems = mEngine.getMainMenuItems();
      currentMenuItem = mEngine.getCurrentMainMenuItem();
      break;
    case Engine::sPaused:
    	menuItems = mEngine.getPauseMenuItems();
      currentMenuItem = mEngine.getCurrentPauseMenuItem();
      break;
    case Engine::sConfigureMenu:
    	menuItems = mEngine.getConfigureMenuItems();
      currentMenuItem = mEngine.getCurrentConfigureMenuItem();
      break;
    default: break;
  }
  
  Text t( "TEXTE CON POUR AVOIR LA HAUTEUR PAR QFONTMETRIC" );
  t.setFont( QFont("Futura", 48 ) );
  int itemsHeight = t.height() * menuItems.size();
  int y = c.getViewport().getHeight() / 2 + itemsHeight / 2;
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
  for(size_t i = 0; i < menuItems.size(); ++i)
  {
  	t.setText( menuItems[i] );
    if( (uint)currentMenuItem == i )
    	t.setBackgroundColor( QColor("yellow") );
    else t.setBackgroundColor( QColor( 0, 0, 0, 0 ) );

    glPushMatrix();
    glTranslated( c.getViewport().getWidth() / 2, y, 0 );
    Sprite s;
    s.set( t.getTexture() );
    s.draw();
    y -= t.height() + 10;
    glPopMatrix();
  }
  
  
//--------
glDisable( GL_DEPTH_TEST );
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

static int _rx = 30;
_rx += 2;
_rx %= 200;
math::Circle _c( Point2d( 100, 100 ), 60 );
math::Rectangle _r( Point2d( _rx, 80 ), Vector2d( 200, 200 ) );
treeD::drawCircle( _c.getCenter(), _c.getRadius() );
treeD::drawRectangle(_r.bottomLeft(), _r.size());
Intersection2d _i = intersect( _c, _r );

glPointSize(2.0);
glColor3ub( 255, 0, 0);

for( int i = 0; i < _i.getNumberOfContacts(); ++i )
{
	drawPoint(_i.getContact(i), 4.0);  
  //normal
  Point2d n = _i.getContact(i) + _i.getNormal(i) * 10;
  glBegin( GL_LINES );
  glVertex2dv( _i.getContact(i).getPtr() );
  glVertex2dv( n.getPtr() );
  glEnd();
}


//on dessine le minkowski
static int _r1x = 180;
_r1x += 2;
_r1x %= 260;
Rectangle _r1( Point2d( _r1x, 220 ), Vector2d(40, 60) );
Rectangle _r2( Point2d( 200, 200 ), Vector2d(40, 40) );
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
treeD::drawRectangle(_r1.bottomLeft(), _r1.size());
treeD::drawRectangle(_r2.bottomLeft(), _r2.size());
BoundingBox2d bb;
glColor3ub(255, 0, 0 );
for( int i = 0; i < 4; i++ )
{
	for( int j = 0; j < 4; j++ )
  {
  	bb.add( toPoint(_r1.points()[i] - _r2.points()[j] ) );
  }
}
treeD::drawRectangle(bb.bottomLeft(), bb.size());

glColor3ub( 255, 255, 0 );
glBegin( GL_POINTS );
for( int i = 0; i < 4; i++ )
{
	for( int j = 0; j < 4; j++ )
  {
  	glVertex2dv( (_r1.points()[i] - _r2.points()[j] ).getPtr() );
  }
}
glEnd();
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


//intersection rectangle a rectangle avec lineSegment
{
	glColor3ub( 255, 255, 0);
	Vector2d rs( 30, 60 );
  Vector2d rs2( 32, 32 );
	Point2d mp = mEngine.getMouse().getPosition();
  mp.setY( c.getViewport().getHeight() - mEngine.getMouse().getPosition().y() );
	Rectangle r( mp - rs/2.0, rs );
  Rectangle r2( Point2d( 600, 450 ) - rs2/2.0, rs2 );

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
  drawRectangle( r );
  drawRectangle( r2 );
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  Intersection2d x = intersect( r2, r );
  if( x.hasContacts() )
  {
  	glColor3ub( 255, 0, 0 );
    for( int i = 0; i < x.getNumberOfContacts(); ++i)
    {
    	drawPoint( x.getContact(i), 4 );
      drawLine( x.getContact(i), x.getContact(i) + 10*x.getNormal(i) );
    }
  }
}
  
//intersection avec penetration (axis overlap)
{
	glColor3ub( 255, 255, 0);
	Vector2d rs( 30, 60 );
  Vector2d rs2( 32, 32 );
	Point2d mp = mEngine.getMouse().getPosition();
  mp.setY( c.getViewport().getHeight() - mEngine.getMouse().getPosition().y() );
	Rectangle r( mp - rs/2.0, rs );
  Rectangle r2( Point2d( 600, 250 ) - rs2/2.0, rs2 );
  Rectangle r3;
  
  Intersection2d intersection = intersect( r, r2 );
  if( intersection.hasContacts() )
  {
  	glColor3ub( 255, 0, 0 );
  	double px1, px2;
  	double penx = axisOverLap(r.bottomLeft().x(), r.bottomRight().x(), 
	    r2.bottomLeft().x(), r2.bottomRight().x(), &px1, &px2 );
    
    glLineWidth( 3.0 );
    glBegin( GL_LINES );
    glVertex2d( px1, r2.bottomLeft().y() - 5 );
    glVertex2d( px2, r2.bottomLeft().y() - 5 );
    glEnd();
    glLineWidth( 1.0 );
    
    double peny = axisOverLap(r.bottomLeft().y(), r.topLeft().y(), 
	    r2.bottomLeft().y(), r2.topLeft().y(), &px1, &px2 );
  
    glLineWidth( 3.0 );
    glBegin( GL_LINES );
    glVertex2d( r2.bottomLeft().x() - 5, px1 );
    glVertex2d( r2.bottomLeft().x() - 5, px2 );
    glEnd();
    glLineWidth( 1.0 );
    
    Vector2d penetration( penx, peny );
		if( abs(penetration.x()) <= abs(penetration.y()) )
    	r3.set( Point2d( 
      	r.bottomLeft().x() - penetration.x(), r.bottomLeft().y() ),
        r.size() );
    else
      r3.set( Point2d( 
      	r.bottomLeft().x(), r.bottomLeft().y() - penetration.y() ),
        r.size() );
  }
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
  drawRectangle( r );
  drawRectangle( r2 );
  glColor3ub( 200, 200, 200 );
  drawRectangle( r3 );
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}  

//intersection line2d/line2d et line2d/segment2d
{
  Point2d mp = mEngine.getMouse().getPosition();
  mp.setY( c.getViewport().getHeight() - mp.y() );
	Line2d l2( Point2d( 0, 10 ), Vector2d( 120, 27 ) );
  Point2d m(c.getViewport().getWidth() / 2, 
  	c.getViewport().getHeight() / 2);
  Line2d l1( m, mp - m); 
  Rectangle rect( Point2d(100, 100), Vector2d( 200, 400 ) );
  
  glColor3ub( 20, 200, 153 );
  glLineWidth( 1.0 );
  drawLine( l1.getPoint() - 100.0 * l1.getDirection(),
  	l1.getPoint() + 100.0 * l1.getDirection() );
  drawLine( l2.getPoint() - 100.0 * l2.getDirection(),
  	l2.getPoint() + 100.0 * l2.getDirection() );
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  drawRectangle( rect );
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  Intersection2d interLine = intersect( l1, l2 );
  if( interLine.hasContacts() )
  {
  	glColor3ub( 255, 0, 0 );
    drawPoint(interLine.getContact(0), 4.0 );
		drawLine( interLine.getContact(0),
    	interLine.getContact(0) + 10 * interLine.getNormal(0) );
  }
  
  Intersection2d interRect = intersect( l1, rect );
  if( interRect.hasContacts() )
  {
  	glColor3ub( 255, 0, 0 );
  	for( int i = 0; i < interRect.getNumberOfContacts(); ++i )
    {
      drawPoint(interRect.getContact(i), 4.0 );
  		drawLine( interRect.getContact(i),
      	interRect.getContact(i) + 10 * interRect.getNormal(i) );
    }  	
  }
}

//intersection segment 2d
{
	static int t=0;
  t++;
	Point2d mp = mEngine.getMouse().getPosition();
  mp.setY( c.getViewport().getHeight() - mp.y() );
  Point3d a(200, 400, 0), b(200,500,0), c( 200, 450,0);
  Point3d ra = rotatePoint( t%360 * kDegreeToRadian, a, Vector3d(0.0, 0.0, 1.0),
  	c );
  Point3d rb = rotatePoint( t%360 * kDegreeToRadian, b, Vector3d(0.0, 0.0, 1.0),
  	c );
	LineSegment2d ls1( Point2d(ra.getX(), ra.getY()), Point2d(rb.getX(), rb.getY()) );
  Rectangle r( Point2d(50, 400), Vector2d(100, 200) );
  LineSegment2d lsUser1( mp, toPoint(mp + Point2d( 0, 100 )) );
  LineSegment2d lsUser2( mp, toPoint(mp + Point2d( 100, 0 )) );
  
  
  glColor3ub(10, 255, 10);
  drawLine( ls1.a(), ls1.b() );
  drawLine( lsUser1.a(), lsUser1.b() );
  drawLine( lsUser2.a(), lsUser2.b() );
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  drawRectangle( r );
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  Intersection2d x = intersect( lsUser1, ls1 );
  x.add( intersect( lsUser2, ls1 ) );
  if( x.hasContacts() )
  {
  	glColor3ub( 255, 0, 0 );
    drawPoint( x.getContact(0), 4 );
    drawLine( x.getContact(0), x.getContact(0) + 10*x.getNormal(0) );
  }
  
  x = intersect(lsUser1, r);
  x.add( intersect( lsUser2, r ) );
  if( x.hasContacts() )
  {
  	glColor3ub( 255, 0, 0 );
    for( int i = 0; i < x.getNumberOfContacts(); ++i)
    {
    	drawPoint( x.getContact(i), 4 );
      drawLine( x.getContact(i), x.getContact(i) + 10*x.getNormal(i) );
    }
  }
}

glColor3ub( 255, 255, 255);
//--------  
	c.popMatrices();
}

//-----------------------------------------------------------------------------
void Viewer::gotEvent( Engine::event iE )
{
	switch (iE) 
  {
  	case Engine::eStateChanged:
    	switch (mEngine.getState()) 
      {
        case Engine::sEditing: break;
        default: break;
      }
    break;
    case Engine::eStageLoaded: update(); break;
    case Engine::eFrameDone: updateGL(); break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0, 0.0, 0.0, 0.0);

	mFboLightDepth.addColorAttachment(true);  
  mFboLightDepth.addDepthAttachment(true);
  mFboLightDepth.getDepthTexture().setFilter( GL_LINEAR );
  
  mFboLightMask.addColorAttachment(true);
  
  //mShadowMapShader.addFragmentSource( kLightFade );
  //mShadowMapShader.addFragmentSource( kLightOfSight );
  mShadowMapShader.addVertexSource( kShadowLightVertex );
  mShadowMapShader.addFragmentSource( kShadowLightFrag );
  
  mEngine.graphicsAreReady();
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent( QKeyEvent* ipE )
{ if( !ipE->isAutoRepeat() ) mEngine.keyPressed( ipE->key() ); }

//-----------------------------------------------------------------------------
void Viewer::keyReleaseEvent( QKeyEvent* ipE )
{ mEngine.keyReleased( ipE->key() ); }

//-----------------------------------------------------------------------------
/*0 - est 
  1 - nord
  2 - ouest
  3 - sud*/
Camera Viewer::makeLightCamera(int i, const Point2d& iPos,
	const Vector2d& iViewport, double iSightDepth) const
{
	Camera r;
  Point2d look;
  Vector2d up;
  switch (i) 
  {
    case 0: look.set( iPos.x() + 1, iPos.y() ); up.set(0.0, 1.0); break;
    case 1: look.set( iPos.x(), iPos.y() + 1 ); up.set(1.0, 0.0); break;
    case 2: look.set( iPos.x() - 1, iPos.y() ); up.set(0.0, 1.0); break;
    case 3: look.set( iPos.x(), iPos.y() - 1 ); up.set(1.0, 0.0); break;
    default: break;
  }
  
  r.setViewportSize( iViewport );
  r.set(
    Point3d( iPos.x(), iPos.y(), 0.0),
    Point3d( look.x(), look.y(), 0.0),
    Vector3d( up.x(), up.y(), 0.0 ) );
  r.setPerspectiveProjection(92, iViewport.y() / iViewport.x(), 10, 
  	iSightDepth, false);
  return r;
}

//-----------------------------------------------------------------------------
void Viewer::mouseMoveEvent( QMouseEvent* ipE )
{ mEngine.mouseMoved( Point2i( ipE->x(), ipE->y() ) ); }

//-----------------------------------------------------------------------------
void Viewer::mousePressEvent( QMouseEvent* ipE )
{ mEngine.mousePressed( ipE->button() ); }

//-----------------------------------------------------------------------------
void Viewer::mouseReleaseEvent( QMouseEvent* ipE )
{ mEngine.mouseReleased( ipE->button() ); }

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  draw();
}

//-----------------------------------------------------------------------------
Texture Viewer::renderLights()
{
  const Camera& gc = mEngine.getGameCamera();
  const Player& p = mEngine.getPlayer();
  Texture shadowMap;
  Matrix4d lightCamView, lightCamProjection, MCToShadowMap;
  Matrix4d camView, camProjection;

  double sightDepth = 400;
  Vector2i fboSize = mFboLightDepth.getSize();
  
  pushFrameBuffer( mFboLightMask );
  mFboLightMask.drawTo(0);
  glClear( GL_COLOR_BUFFER_BIT );
  popFrameBuffer();
    
  for( int i = 0; i < 4; ++i )
  {
    pushFrameBuffer( mFboLightDepth );
    mFboLightDepth.drawTo(0);
    glEnable(GL_DEPTH_TEST);
    glClear( GL_DEPTH_BUFFER_BIT );
    Camera c = makeLightCamera( i, p.getPosition(), fboSize, sightDepth );

    c.pushAndApplyMatrices();
    lightCamView = c.getViewMatrix();
    lightCamProjection = c.getProjectionMatrix();
    MCToShadowMap = lightCamView * lightCamProjection;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(5.0);
    drawDataMap();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0);
    c.popMatrices();
    glDisable( GL_DEPTH_TEST );
    shadowMap = mFboLightDepth.getDepthTexture();
    popFrameBuffer();
    
    c = gc;
    c.setViewportSize( mFboLightMask.getSize() );
    c.pushAndApplyMatrices();
    c.popMatrices();

    Matrix4d clipToWindow;      
    clipToWindow.setScaling( Vector3d(0.5) );
    clipToWindow.setTranslation( Point3d(0.5) );
    camView = c.getViewMatrix();
    camProjection = c.getProjectionMatrix();

    pushFrameBuffer( mFboLightMask );
    pushShader( mShadowMapShader );
    mFboLightMask.drawTo(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMap.getId() );
    mShadowMapShader.setUniform( "MVPMatrix", camView * camProjection );
    mShadowMapShader.setUniform( "MCToShadowMap", MCToShadowMap * clipToWindow );
    mShadowMapShader.setUniform( "shadowMap", 0 );
    drawRectangle( 
      toPoint( toVector(p.getPosition()) - c.getProjection().getSize() ), 
      2 * c.getProjection().getSize() );
    popShader();
  	popFrameBuffer();
  }
    
  return mFboLightMask.getTexture(0);//.copy();
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
  Widget3d::resizeGL( iW, iH );
  
  const Camera& gc = mEngine.getGameCamera();
  Vector2d offSize( 1, gc.getViewport().getHeight() );
  mFboLightDepth.resize( offSize );
  
  mFboLightMask.resize( gc.getViewport().getSize() );
  //mFboLightDepth.resize(iW, iH);
  //mFboLightMask.resize(iW, iH);
  update();
}

//-----------------------------------------------------------------------------
void Viewer::wheelEvent( QWheelEvent* ipE )
{ mEngine.mouseWheelMoved( ipE->delta() / 8.0 ); }

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainWindow::MainWindow() : QMainWindow(),
	Client(),
  mEngine(),
  mpViewer(0),
  mEditionPath( QDir::homePath() )
{
  resize(800, 600);
  
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(0);
  pLyt->setMargin( 0 );    
  mpViewer = new Viewer( mEngine, pMainWidget);
  pLyt->addWidget( mpViewer );
  
  //--- paneau pour l'édition
  mpEditionPanel = new QWidget( this );
  mpEditionPanel->setWindowFlags( mpEditionPanel->windowFlags() | Qt::Tool |
    Qt::WindowStaysOnTopHint );
	mpEditionPanel->setMinimumSize(200, 300);
  mpEditionPanel->hide();
  
  QVBoxLayout* pEditionLyt = new QVBoxLayout( mpEditionPanel );
  pEditionLyt->setMargin(0);
  pEditionLyt->setMargin( 0 );
  QTabWidget* pTabs = new QTabWidget( mpEditionPanel );
  pEditionLyt->addWidget( pTabs );

  //--- Edition de la map
  {
    QWidget* pPage = new QWidget();
    QVBoxLayout* pLyt = new QVBoxLayout(pPage);
    pLyt->setSpacing( 2 );
    pLyt->setMargin( 2 );
    
    //ouvrir et nouveau
    QHBoxLayout* pNewAndOpen = new QHBoxLayout();
    {
      QPushButton* pNew = new QPushButton( "Nouveau...", pPage );
      connect( pNew, SIGNAL( clicked() ), this, SLOT( newMapClicked() ) );
      QPushButton* pOpen = new QPushButton( "Ouvrir...", pPage );
      connect( pOpen, SIGNAL( clicked() ), this, SLOT( openMapClicked() ) );
      
      pNewAndOpen->addWidget( pNew );
      pNewAndOpen->addWidget( pOpen );
      pNewAndOpen->addStretch( 1 );
    }
    
    //Layers
    QVBoxLayout* pLayersLyt = new QVBoxLayout();
    {
      mpLayers = new QListWidget( pPage );
      mpLayers->setAlternatingRowColors(true);
      connect( mpLayers, SIGNAL( currentRowChanged( int ) ),
        this, SLOT( layerSelectionChanged( int ) ) );
      connect( mpLayers, SIGNAL( itemChanged( QListWidgetItem* ) ),
        this, SLOT( layerVisibilityChanged( QListWidgetItem* ) ) );
      mpLayers->setFixedHeight( 80 );
      
      QHBoxLayout* pL1 = new QHBoxLayout();
      {
        QPushButton* pAdd = new QPushButton( "ajouter", pPage );
        connect( pAdd, SIGNAL( clicked() ), this, SLOT( addLayerClicked() ) );
        QPushButton* pRemove = new QPushButton( "enlever", pPage );
        connect( pRemove, SIGNAL( clicked() ), this, SLOT( removeLayerClicked() ) );
        
        pL1->addStretch(1);
        pL1->addWidget( pAdd );
        pL1->addWidget( pRemove );
      }
      
      pLayersLyt->addWidget(mpLayers);
      pLayersLyt->addLayout(pL1);
    }
    
    //sprites
    QVBoxLayout* pL1 = new QVBoxLayout();
    {
      mpSprites = new QTableWidget( pPage );
      connect( mpSprites, SIGNAL( cellClicked(int, int) ),
        this, SLOT( spriteSelectionChanged(int, int) ) );
          
      QHBoxLayout* pAddRemoveLyt = new QHBoxLayout();
      {
        QPushButton* pAdd = new QPushButton( "ajouter", pPage );
        connect( pAdd, SIGNAL( clicked() ), 
          this, SLOT( addSpriteToLayerClicked() ) );
        QPushButton* pRemove = new QPushButton( "enlever", pPage );
        connect( pRemove, SIGNAL( clicked() ), 
          this, SLOT( removeSpriteFromLayerClicked() ) );
        
        pAddRemoveLyt->addStretch(1);
        pAddRemoveLyt->addWidget(pAdd);
        pAddRemoveLyt->addWidget(pRemove);
      }
      
      pL1->addWidget( mpSprites );
      pL1->addLayout( pAddRemoveLyt );
    }
    
    //type de cellule
    mpCellTypeFrame = new QWidget(pPage);
    QHBoxLayout* pL2 = new QHBoxLayout(mpCellTypeFrame);
    {
      QLabel* pL = new QLabel("Cellule", mpCellTypeFrame);
      mpCellType = new QComboBox( mpCellTypeFrame );
      connect( mpCellType, SIGNAL(activated(int)), 
        this, SLOT(cellTypeChanged( int ) ) );
      
      for( int i = 0; i < Stage::ctNumberOfCellType; ++i )
        mpCellType->insertItem( i, mEngine.toString( 
          (Stage::cellType)i ) );
        
      pL2->addWidget(pL);
      pL2->addStretch(1);
      pL2->addWidget(mpCellType);
    }
            
    pLyt->addLayout(pNewAndOpen);
    pLyt->addLayout(pLayersLyt);
    pLyt->addLayout(pL1, 2);
    pLyt->addWidget(mpCellTypeFrame);    
    pLyt->addStretch(1);
    
    pTabs->insertTab( tMap, pPage, "Map" );
  }
  
  //--- edition background
  {
  	QWidget* pPage = new QWidget();
    QVBoxLayout* pLyt = new QVBoxLayout(pPage);
    
    //background
    QHBoxLayout* pL1 = new QHBoxLayout();
    {
      QLabel* pl = new QLabel( "Backg.", pPage );
      mpBackground = new QComboBox( pPage );
      connect( mpBackground, SIGNAL( activated(int) ),
        this, SLOT( backgroundChanged(int) ) );
        
      pL1->addWidget(pl);
      pL1->addStretch(1);
      pL1->addWidget(mpBackground);      
    }

    pLyt->addLayout(pL1);
    pLyt->addStretch(1);
    pTabs->insertTab( tBackground, pPage, "Back" );
  }
  
  //--- edition actor
  {
  	QWidget* pPage = new QWidget();
    QVBoxLayout* pLyt = new QVBoxLayout(pPage);
    
    pTabs->insertTab( tActor, pPage, "Actor" );
  }
  
  Camera c = mpViewer->getCamera();
  c.set( Point3d(10, 0, 0),
  	Point3d(0),
    Vector3d( 0, 1, 0 ) );
	c.setOrthoProjection( 200, 0.5, 2000 );
  mpViewer->setCamera(c, false);
  
  mEngine.registerClient( this );
  mEngine.registerClient( mpViewer );
}

//-----------------------------------------------------------------------------
void MainWindow::addLayerClicked()
{
	mEngine.addLayer();
  mpLayers->blockSignals(true);
  mpLayers->clear();
  mpLayers->blockSignals(false);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::addSpriteToLayerClicked()
{
  SpriteCatalog& sc = mEngine.getSpriteCatalog();
  int layer = mpLayers->currentRow();
  for( int i = 0; i < (int)sc.getNumberOfSprites(); ++i )
  	mEngine.getStage().addToken( layer, sc.getSpriteToken(i) );
    
  mpSprites->clear(); mpSprites->setRowCount(0);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::backgroundChanged(int iIndex)
{
	QString token = mpBackground->currentText();
  mEngine.getStage().setBackgroundToken( token );
  updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::cellTypeChanged( int iCt )
{ mEngine.setEditingTool( (Stage::cellType)iCt ); }

//-----------------------------------------------------------------------------
void MainWindow::gotEvent( Engine::event iE )
{
	switch (iE) 
  {
  	case Engine::eStageLoaded:
    mpLayers->clear();
    mpSprites->clear(); mpSprites->setRowCount(0);
    mpBackground->clear();
    updateUi();
    break;
  	case Engine::eStateChanged: updateUi(); break;
    case Engine::eQuit: qApp->quit(); break;
    case Engine::eEditorUiChanged:
      mpLayers->blockSignals(true);
      mpLayers->clear();
      mpLayers->blockSignals(false);
      updateUi();
    break;

    default: break;
  }
}

//-----------------------------------------------------------------------------
void MainWindow::layerSelectionChanged( int iRow )
{
	mEngine.setCurrentLayer( iRow );
	//clear les sprites... Ils seront repeuplés par updateUi
  mpSprites->clear(); mpSprites->setRowCount(0);
	updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::layerVisibilityChanged( QListWidgetItem* ipItem )
{
	int i = mpLayers->row( ipItem );
  mEngine.getStage().setLayerAsVisible( i, ipItem->checkState() == Qt::Checked );
  updateUi();  
}

//-----------------------------------------------------------------------------
void MainWindow::newMapClicked()
{
	QDialog d;
  QVBoxLayout* l = new QVBoxLayout( &d );
  l->setMargin(5); l->setSpacing(2);
  
  QLineEdit* pName;
  QHBoxLayout* pLine1 = new QHBoxLayout();
  {
  	QLabel* pl = new QLabel( "nom:", &d );
    pName = new QLineEdit( "stage", &d );
    
    pLine1->addWidget(pl);
    pLine1->addStretch(1);
    pLine1->addWidget(pName);
  }
  
  QSpinBox *pSizeX, *pSizeY;
  QHBoxLayout* pLine2 = new QHBoxLayout();
  {
  	QLabel* pl = new QLabel( "taille:", &d );
    pSizeX = new QSpinBox( &d );
    pSizeX->setMinimum( 1 );
    pSizeX->setMaximum( 10000 );
    pSizeX->setValue( 200 );
    pSizeY = new QSpinBox( &d );
    pSizeY->setMinimum( 1 );
    pSizeY->setMaximum( 10000 );
    pSizeY->setValue( 100 );
    
    pLine2->addWidget(pl);
    pLine2->addStretch(1);
    pLine2->addWidget(pSizeX);
    pLine2->addWidget(pSizeY);
  }
  
  QHBoxLayout* pOkCancel = new QHBoxLayout();
  {
  	QPushButton* pOk = new QPushButton( "Ok", &d );
    connect( pOk, SIGNAL( clicked() ), &d, SLOT( accept() ) );
    QPushButton* pCancel = new QPushButton( "Cancel", &d );
    connect( pCancel, SIGNAL( clicked() ), &d, SLOT( reject() ) );
    
  	pOkCancel->addStretch(1);
    pOkCancel->addWidget(pOk);
    pOkCancel->addWidget(pCancel);
  }
  
  l->addLayout(pLine1);
  l->addLayout(pLine2);
  l->addLayout(pOkCancel);
  
  pName->setFocus();
  if( d.exec() == QDialog::Accepted )
  {
  	QString name = pName->text().isNull() ? "noName" : pName->text();
  	mEngine.newStage( name, pSizeX->value(), pSizeY->value() );
  }
  
}

//-----------------------------------------------------------------------------
void MainWindow::openMapClicked()
{
	QString f = QFileDialog::getOpenFileName( this, tr("Open Stage"),
		mEditionPath,
    tr("Stage (*.bin)") );
  if( !f.isNull() )
  {
  	mEditionPath = QDir( f ).dirName();
  	mEngine.loadStage( f );
  }
}

//-----------------------------------------------------------------------------
void MainWindow::populateBackground()
{
	treeD::SpriteCatalog& sp = mEngine.getSpriteCatalog();
	if( mpBackground->count() == 0 )
  {
  	mpBackground->addItem( "aucun" );
    for( int i = 0; i < sp.getNumberOfTextures(); ++i )
    {
      mpBackground->addItem( sp.getTextureToken(i) );
    }
  }
}

//-----------------------------------------------------------------------------
void MainWindow::populateLayers()
{
	mpLayers->blockSignals(true);
	const Stage& s = mEngine.getStage();

	if( mpLayers->count() == 0 )
  {
  	QString name;
    for( int i = 0; i < s.getNumberOfLayers(); ++i )
    {
    	QListWidgetItem* pItem = new QListWidgetItem( s.getLayerName(i), mpLayers );
      pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
      pItem->setCheckState( Qt::Checked );
    	mpLayers->addItem( pItem );
    }
    
    mpLayers->setCurrentRow( mEngine.getCurrentLayer() );
  }	
  mpLayers->blockSignals(false);
}

//-----------------------------------------------------------------------------
void MainWindow::populateSprites()
{  
  if( mpSprites->rowCount() == 0 )
  {
  	int layerIndex = mpLayers->currentRow();
    vector<QString> st = mEngine.getStage().getTokens( layerIndex );
  	mpSprites->setRowCount( ceil(st.size() / (double)kSpriteTableWidth) );
    mpSprites->setColumnCount( kSpriteTableWidth );
		for( int i = 0; i < mpSprites->columnCount(); ++i )
      mpSprites->horizontalHeader()->resizeSection( i, 32 );
    for( int i = 0; i < mpSprites->rowCount(); ++i )
	    mpSprites->verticalHeader()->resizeSection( i, 32 );

    for( int i = 0; i < (int)st.size(); ++i )
    {
      QLabel* l = new QLabel();
      QPixmap pm; 
      pm.convertFromImage( mEngine.getSpriteCatalog().getSprite( st[i] ).
      	asQImage() );
      l->setPixmap( pm );
      mpSprites->setCellWidget( i / kSpriteTableWidth, 
      	i % kSpriteTableWidth, l );
    }
  }
}

//-----------------------------------------------------------------------------
void MainWindow::removeLayerClicked()
{
	if( mpLayers->count() > 0 && mpLayers->currentItem()->isSelected() )
  {
  	mEngine.removeLayer( mpLayers->currentRow() );
    mpLayers->blockSignals(true);
    mpLayers->clear();
    mpLayers->blockSignals(false);
    updateUi();
  }
}

//-----------------------------------------------------------------------------
void MainWindow::removeSpriteFromLayerClicked()
{
}

//-----------------------------------------------------------------------------
void MainWindow::spriteSelectionChanged(int i, int j)
{ 
	int currentLayer = mpLayers->currentRow();
	vector<QString> tokens = mEngine.getStage().getTokens( currentLayer );
  int index = i * kSpriteTableWidth + j;
  QString t;
  if( index >= 0 && index < (int)tokens.size() ) { t = tokens[index]; }
	mEngine.setEditingSpriteToken( t );
}

//-----------------------------------------------------------------------------
void MainWindow::updateUi()
{
	switch( mEngine.getState() )
  {
  case Engine::sEditing: 
  	mpEditionPanel->show();
    mpEditionPanel->move( mapToGlobal( rect().topRight() ) );
    break;
  default:
  	mpEditionPanel->hide();
    break;
  }
  
  if( mpEditionPanel->isVisible() )
  {
  	//--- layers tab
  	populateLayers();
    //ajustement de la visibility de la couche
    for(int i = 0; i < mpLayers->count(); ++i)
    {
    	Qt::CheckState cs = mEngine.getStage().isLayerVisible(i) ? Qt::Checked :
        Qt::Unchecked;
    	mpLayers->item( i )->setCheckState( cs );
    }
    
    mpCellTypeFrame->setShown( mpLayers->item( mEngine.getCurrentLayer() )->text() == "data" );
    mpCellType->setCurrentIndex( mEngine.getEditingTool() );
    
    //sprites
    populateSprites();

		//--- Actor tab
    
    //--- background tab
  	populateBackground();
    
    //selection du backgound
    int index = mpBackground->findText( mEngine.getStage().getBackgroundToken() );
    mpBackground->setCurrentIndex( index );
  }
}
