/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>

using namespace realisim;
  using namespace math;
  using namespace treeD;

namespace 
{
  QString toString( GLenum iE )
  {
    QString r;
  	switch (iE) 
    {
      case GL_LINEAR: r = "GL_LINEAR"; break;
      case GL_NEAREST: r = "GL_NEAREST"; break;
      case GL_NEAREST_MIPMAP_NEAREST: r = "GL_NEAREST_MIPMAP_NEAREST"; break;
      case GL_LINEAR_MIPMAP_NEAREST: r = "GL_LINEAR_MIPMAP_NEAREST"; break;
      case GL_NEAREST_MIPMAP_LINEAR: r = "GL_NEAREST_MIPMAP_LINEAR"; break;
      case GL_LINEAR_MIPMAP_LINEAR: r = "GL_LINEAR_MIPMAP_LINEAR"; break;
      default: r = "unknown"; break;      
    }
    return r;
  }
}

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent)
{
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
  mTex.set( QImage( ":/moto.jpg", "JPG" ) );
  mText.setFont( QFont( "Futura", 20 ) );
  mText.setFrontColor( QColor( "yellow" ) );
  mText.setText( "les pets font prout éàèî`πì¨köù" );
  mText.addDropShadow( true );
}
//-----------------------------------------------------------------------------
void Viewer::changeTextureMinificationInterpolation()
{
	switch ( mTex.getMinificationFilter() ) 
  {
    case GL_LINEAR: mTex.setMinificationFilter( GL_NEAREST ); break;
    case GL_NEAREST: mTex.setMinificationFilter( GL_NEAREST_MIPMAP_NEAREST ); break;
    case GL_NEAREST_MIPMAP_NEAREST: mTex.setMinificationFilter( GL_LINEAR_MIPMAP_NEAREST ); break;
    case GL_LINEAR_MIPMAP_NEAREST: mTex.setMinificationFilter( GL_NEAREST_MIPMAP_LINEAR ); break;
    case GL_NEAREST_MIPMAP_LINEAR: mTex.setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR ); break;
    case GL_LINEAR_MIPMAP_LINEAR: mTex.setMinificationFilter( GL_LINEAR ); break;
    default:
      break;
  }
  update();
}
//-----------------------------------------------------------------------------
void Viewer::changeTextureMagnificationInterpolation()
{
	switch ( mTex.getMagnificationFilter() ) 
  {
    case GL_LINEAR: mTex.setMagnificationFilter( GL_NEAREST ); break;
    case GL_NEAREST: mTex.setMagnificationFilter( GL_NEAREST_MIPMAP_NEAREST ); break;
    case GL_NEAREST_MIPMAP_NEAREST: mTex.setMagnificationFilter( GL_LINEAR_MIPMAP_NEAREST ); break;
    case GL_LINEAR_MIPMAP_NEAREST: mTex.setMagnificationFilter( GL_NEAREST_MIPMAP_LINEAR ); break;
    case GL_NEAREST_MIPMAP_LINEAR: mTex.setMagnificationFilter( GL_LINEAR_MIPMAP_LINEAR ); break;
    case GL_LINEAR_MIPMAP_LINEAR: mTex.setMagnificationFilter( GL_LINEAR ); break;
    default: break;
  }
  update();
}
//-----------------------------------------------------------------------------
void Viewer::generateMipmap()
{
	mTex.generateMipmap();
	update();
}
//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  glEnable( GL_TEXTURE_2D );
  glDisable( GL_LIGHTING );
  glColor3ub( 255, 255, 255);
  glBindTexture( GL_TEXTURE_2D, mTex.getId() );
  glBegin( GL_QUADS );
  	glTexCoord2d( 0.0, 0.0 );
    glVertex2i( 0, 0 );
    glTexCoord2d( 0.0, 1.0 );
    glVertex2i( 0, mTex.height() );
    glTexCoord2d( 1.0, 1.0 );
    glVertex2i( mTex.width(), mTex.height() );
    glTexCoord2d( 1.0, 0.0 );
    glVertex2i( mTex.width(), 0 );
  glEnd();
  glEnable( GL_LIGHTING );
  glDisable( GL_TEXTURE_2D );
  
  //draw text avec projection ortho 2d.
  glDisable( GL_DEPTH_TEST );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
	glLoadIdentity();
  
  glTranslated( 10, 10 ,0 );
  Camera c = getCamera();
  c.set( Point3d( 0.0, 0.0, 5.0 ),
  	Point3d( 0.0, 0.0, 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c.setProjection( 0, c.getViewport().getWidth(),
  	0, c.getViewport().getHeight(),
   	0.5, 2000, Camera::Projection::tOrthogonal );    
  c.applyProjectionTransformation();
  c.applyModelViewTransformation();
  
  glPushMatrix();
  glTranslated( 5, 3*mText.height(), 0.0);
	mText.draw();
  glPopMatrix();
    
  glEnable( GL_DEPTH_TEST );
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  
  glColor3ub( 0, 0, 0 );
  renderText(10, 20, QString("caca poil"), QFont( "helvetica", 14 ) );
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
  resize(800, 600);
  
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(5);
  pLyt->setMargin( 5 );
  
  QWidget* pPanel = new QWidget( pMainWidget );
  {
  	QVBoxLayout* pVlyt = new QVBoxLayout( pPanel );
    pVlyt->setMargin( 5 );
    pVlyt->setSpacing( 5 );
    QHBoxLayout* pLine1 = new QHBoxLayout();
    {
      mpMinInterpolation = new QPushButton( "GL_LINEAR", pPanel );
      connect( mpMinInterpolation, SIGNAL( clicked() ),
        this, SLOT( minInterpolationClicked() ) );
      
      pLine1->addWidget( new QLabel( "min.", pPanel ) );
      pLine1->addWidget( mpMinInterpolation );
    }
    
    QHBoxLayout* pLine2 = new QHBoxLayout();
    {
      mpMagInterpolation = new QPushButton( "GL_LINEAR", pPanel );
    connect( mpMagInterpolation, SIGNAL( clicked() ),
    	this, SLOT( magInterpolationClicked() ) );
      
      pLine2->addWidget( new QLabel( "mag.", pPanel ) );
      pLine2->addWidget( mpMagInterpolation );
    }
    
    QPushButton* pGenMipMap = new QPushButton( "generate mipmap", pPanel );
    connect( pGenMipMap, SIGNAL( clicked() ),
    	this, SLOT( generateMipmap() ) );
    
    pVlyt->addWidget( pGenMipMap );
    pVlyt->addLayout( pLine1 );
    pVlyt->addLayout( pLine2 );
    pVlyt->addStretch(1);
  }
  
  mpViewer = new Viewer(pMainWidget);
  
  pLyt->addWidget( pPanel, 1 );
  pLyt->addWidget(mpViewer, 5 );
  
  Camera c = mpViewer->getCamera();
  c.set( Point3d(0, 0, 100),
  	Point3d(), Vector3d(0, 1, 0) );
	c.setOrthoProjection( 200, 0.5, 2000 );
  mpViewer->setControlType( Widget3d::ctPan );
  mpViewer->setCamera(c, false);
  
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::generateMipmap()
{
	mpViewer->generateMipmap();
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::minInterpolationClicked()
{
	mpViewer->changeTextureMinificationInterpolation();
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::magInterpolationClicked()
{
	mpViewer->changeTextureMagnificationInterpolation();
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	QString minInt = toString( mpViewer->getTexMinInterpolation() );
  QString magInt = toString (mpViewer->getTexMagInterpolation() );
  
  mpMinInterpolation->setText( minInt );
  mpMagInterpolation->setText( magInt );
}