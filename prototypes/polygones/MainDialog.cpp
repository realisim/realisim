
#include "3d/Camera.h"
#include "3d/Utilities.h"
#include "MainDialog.h"
#include "math/MathUtils.h"
#include "math/PlatonicSolid.h"
#include "QColorDialog"
#include "QComboBox"
#include "QFrame"
#include "QLabel"
#include "QLayout"
#include "QLineEdit"
#include "QListWidget"
#include "QMouseEvent"
#include "QPushButton"
#include "QSlider"

using namespace realisim;
  using namespace math;
  using namespace treeD;
  
static const int kIterationInterval = 30; //ms

MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
	createUi();
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  resize(1200, 600);
  
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainFrame);
  pLyt->setMargin(2);
  pLyt->setSpacing(5);
  
  //--- on bricole le panneau de gauche
  QFrame* pLeftPanel = new QFrame(pMainFrame);
  QVBoxLayout* pLeftPanelLyt = new QVBoxLayout( pLeftPanel );  
  pLeftPanelLyt->setMargin( 2 );
  pLeftPanelLyt->setSpacing( 5 );
  {
  }
  
  //ajoute le left panel au layout principale
  pLyt->addWidget(pLeftPanel, 1);
  
  mpViewer = new Viewer(pMainFrame);
  mpViewer->setCameraSpeed( 0.2 );
  Camera c = mpViewer->getCamera();  
  c.set( Point3d( 0.0, 0.0, 50.0 ),
  	Point3d(0.0),
    Vector3d( 0, 1, 0) );
  mpViewer->setCamera( c, false );
  mpViewer->setControlType( Widget3d::ctFree );
  pLyt->addWidget(mpViewer, 4);
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	mpViewer->update();
}
//------------------------------------------------------------------------------
//--- Viewer
//------------------------------------------------------------------------------
Viewer::Viewer( QWidget* ipW ) :
	Widget3d( ipW ),
//  mTimerId(0),
  mData(),
  mMode( mCamera ),
  mSelection( -1 )
{
	setMouseTracking( true );
//  mTimerId = startTimer(kIterationInterval);  
}

Viewer::~Viewer()
{}
//------------------------------------------------------------------------------
void Viewer::drawPolygon( const Polygon& iP ) const
{
	glPointSize( 4 );
  //dessine les points du polygones
  glDisable( GL_LIGHTING );
  glBegin( GL_POINTS );
  for( int j = 0; j < iP.getNumberOfVertices(); ++j )
  {
  	iP.isCoplanar() ? glColor3ub( 5, 230, 12 ) : glColor3ub( 200, 5, 12 );
    glVertex3dv( iP.getVertex( j ).getPtr() );
  }
  glEnd(); 
  
  //dessine le polygon
  glEnable( GL_LIGHTING );
  glColor3ub( 100, 100, 100 );
  if( !iP.isCoplanar() ) glColor3ub( 200, 5, 12 );
  if( !iP.isConvex() ) glColor3ub( 5, 5, 200 );
  if( !iP.isCoplanar() && !iP.isConvex() ) glColor3ub( 200, 5, 200 );
  glNormal3dv( iP.getNormal().getPtr() );
  glBegin( GL_POLYGON );
  for( int j = 0; j < iP.getNumberOfVertices(); ++j )
  {
    glVertex3dv( iP.getVertex(j).getPtr() );
  }
  glEnd();
  
  //dessine la normal
  glDisable( GL_LIGHTING );
  glColor3ub( 200, 200, 200 );
  Vector3d v = toVector( iP.getCentroid() ) + iP.getNormal();
  glBegin( GL_LINES );
    glVertex3dv( iP.getCentroid().getPtr() );
    glVertex3dv( v.getPtr() );
  glEnd();
  
//  if( iP.getNumberOfVertices() )
//  { //debug
//      Vector3d z = iP.getNormal();
//      Vector3d x = iP.getVertex( 0 ) - iP.getCentroid();
//      x.normalise();
//      Vector3d y = z ^ x;
//      
//      Point3d centroid = iP.getCentroid();
//      glBegin( GL_LINES );
//      glColor3ub( 255, 0, 0 );
//      glVertex3dv( centroid.getPtr() );
//      glVertex3dv( (centroid + x).getPtr() );
//      
//      glColor3ub( 0, 255, 0 );
//      glVertex3dv( centroid.getPtr() );
//      glVertex3dv( (centroid + y).getPtr() );
//      
//      glColor3ub( 0, 0, 255 );
//      glVertex3dv( centroid.getPtr() );
//      glVertex3dv( (centroid + z).getPtr() );
//      glEnd();
//  }
}
//------------------------------------------------------------------------------
void Viewer::drawSceneForPicking() const
{  
  glPointSize( 4 );
  for( int i = 0; i < (int)mData.mPolygons.size(); ++i )
  {
  	glDisable( GL_LIGHTING );
  	glBegin( GL_POINTS );
  	const Polygon& pol = mData.mPolygons[i];
  	for( int j = 0; j < pol.getNumberOfVertices(); ++j )
    {
    	QColor c = idToColor( j * mData.mPolygons.size() + i );
	  	glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );
  		const Point3d& p = pol.getVertex(j);
  		glVertex3d( p.x(), p.y(), p.z() );
    }
    glEnd();     
  }
  glEnable( GL_LIGHTING );
}
//------------------------------------------------------------------------------
Polygon& Viewer::getLastPolygon()
{
	if( mData.mPolygons.empty() )
  	mData.mPolygons.push_back( Polygon() );
  return mData.mPolygons[ mData.mPolygons.size() - 1 ];
}

//------------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
}
//------------------------------------------------------------------------------
void Viewer::keyPressEvent( QKeyEvent* e )
{
  if( mMode == mCamera ) { Widget3d::keyPressEvent(e); }
  update();
}
//------------------------------------------------------------------------------
void Viewer::keyReleaseEvent( QKeyEvent* e )
{
	if( mMode == mCamera ) { Widget3d::keyReleaseEvent(e); }
  
  switch ( e->key() )
  {
    case Qt::Key_C: mMode = mCamera; break;
    case Qt::Key_X:
    	mMode = mAddPoint;
      if( !mData.mPoints.empty() )
      {
        mData.mPolygons.push_back( Polygon( mData.mPoints ) );
        mData.mPoints.clear();
      }
      break;
    case Qt::Key_T:
    {
      vector< Point3d > points;
      for( int i = 0; i < 360; i+=10 )
      {
      	double a = 3*sin( i * PI / 180.0 ), b = 3 * cos( i * PI / 180.0 );
      	points.push_back( Point3d( 0.0, a, b ) );       
      }
       mData.mPolygons.push_back( Polygon( points ) );
    }break;
    default: break;
  }
  update();
}
//------------------------------------------------------------------------------
void Viewer::mouseMoveEvent( QMouseEvent* e )
{ 
  Widget3d::mouseMoveEvent( e );
	switch( mMode )
  {
  	case mAddPoint:
    {
      vector<unsigned int> selected = pick( e->x(), e->y(), 3, 3 );
      if( !selected.empty() ) printf( "%d\n", selected[0] );
    }break;
    default: break;
  }
}
//------------------------------------------------------------------------------
void Viewer::mousePressEvent( QMouseEvent* e )
{
	switch (mMode) 
  {
    case mCamera: Widget3d::mousePressEvent( e ); break;
    case mAddPoint:
    {
    	Point3d point;
      const Camera& c = getCamera();
      point = c.screenToWorld( Point2d( e->x(), e->y() ), c.getLook() );
      mData.mPoints.push_back( point );
      update();
    } break;
    default: break;
  }
  update();
}
//------------------------------------------------------------------------------
void Viewer::mouseReleaseEvent( QMouseEvent* e )
{
	Widget3d::mouseReleaseEvent( e );
  mSelection = -1;
  update();
}
static PlatonicSolid sIsocahedron( PlatonicSolid::tIsocahedron );
//------------------------------------------------------------------------------
void Viewer::paintGL()
{
	Widget3d::paintGL();
  
  glPushMatrix();
  glTranslated(5, 5, 5);
  glEnable( GL_LIGHTING );
  glColor3ub( 128, 128, 128 );
  treeD::draw( sIsocahedron );
  glPopMatrix();
  
  drawPolygon( Polygon( mData.mPoints ) );  
  for( int i = 0; i < (int)mData.mPolygons.size(); ++i )
  {
  	drawPolygon( mData.mPolygons[i] );
    
//    { //debug - montre le polygon remapper a zéro comme le fait la fonction
//      //Polygon::checkIfConvex() const
//      Polygon g = mData.mPolygons[i];
//      Vector3d z = g.getNormal();
//      Vector3d x = g.getVertex( 0 ) - g.getCentroid();
//      x.normalise();
//      Vector3d y = z ^ x;
//			y.normalise();
//
//			glLineWidth(3);
//			glBegin( GL_LINES );
//      glColor3ub( 255, 0, 0 );
//      glVertex3dv( Point3d().getPtr() );
//      glVertex3dv( (Point3d() + x).getPtr() );
//      
//      glColor3ub( 0, 255, 0 );
//      glVertex3dv( Point3d().getPtr() );
//      glVertex3dv( (Point3d() + y).getPtr() );
//      
//      glColor3ub( 0, 0, 255 );
//      glVertex3dv( Point3d().getPtr() );
//      glVertex3dv( (Point3d() + z).getPtr() );
//      glEnd();
//      glLineWidth(1);
//
//      myMatrix4 m( x, y, z );
//      printf("produit scalaire x et y: %.4f\n", x*y );
//      printf("produit scalaire y et z: %.4f\n", y*z );
//      printf("produit scalaire z et x: %.4f\n", z*x );
//      printf(" vx: %.4f, %.4f, %.4f, norm %f\n", x.x(), x.y(), x.z(), x.norm() );
//      printf(" vy: %.4f, %.4f, %.4f, norm %f\n", y.x(), y.y(), y.z(), y.norm() );
//      printf(" vz: %.4f, %.4f, %.4f, norm %f\n", z.x(), z.y(), z.z(), z.norm() );
//      printf("debug matrice:\n %s", m.toString().toStdString().c_str() );
//      m.setTranslation( toVector(g.getCentroid()) );
//      m.invert();
//      vector<Point3d> verts;
//      for( int i = 0; i < g.getNumberOfVertices(); ++i )
//      {
//        Point3d p =  m * g.getVertex(i) ;
//        verts.push_back(p);
//				printf(" debug p%d %.4f, %.4f, %.4f\n", i, p.x(), p.y(), p.z() );
//      }
//      Polygon g2( verts );
//      drawPolygon( g2 );
//      
//			Vector3d n1 = g.getNormal();
//			Vector3d n2 = g2.getNormal();
//      printf("debug normal orig:%.4f, %.4f, %.4f\n", n1.x(), n1.y(), n1.z() );
//      printf("debug normal transf:%.4f, %.4f, %.4f\n", n2.x(), n2.y(), n2.z() );
//    }
  }
  
  //on affihce le point selectionné
  int polyIndex = -1, pointIndex = -1;
  if( mSelection != -1 )
  {
  	/*on trouve l'index du polygone et l'index du point selectionné*/
    polyIndex = mSelection % mData.mPolygons.size();
    pointIndex = mSelection / mData.mPolygons.size();  
    
    glDisable(GL_DEPTH_TEST);
    glPointSize( 5 );
    glColor3ub( 200, 200, 0 );
    glPolygonOffset(1, 1);
    glBegin( GL_POINTS );
    glDisable( GL_LIGHTING );
    glVertex3dv( mData.mPolygons[polyIndex].getVertex( pointIndex ).getPtr() );
    glEnd();
    glEnable(GL_DEPTH_TEST);
  }
  
  //on affiche les axes globaux
  {
    glBegin( GL_LINES );
    glColor3ub( 255, 0, 0 );
    glVertex3d( -1000, 0, 0 );
    glVertex3d( 1000, 0, 0 );
    
    glColor3ub( 0, 255, 0 );
    glVertex3d( 0, -1000, 0 );
    glVertex3d( 0, 1000, 0 );
    
    glColor3ub( 0, 0, 255 );
    glVertex3d( 0, 0, -1000 );
    glVertex3d( 0, 0, 1000 );
    glEnd();
  }
}
//------------------------------------------------------------------------------
//void Viewer::timerEvent(QTimerEvent* ipEvent)
//{
//	Widget3d::timerEvent( ipEvent );
//	update();
//}
