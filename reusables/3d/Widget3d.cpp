/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: RePluginMgr
* Description: Basic QT OpenGL canvas
*
*
******************************************************************************/

#include "Widget3d.h"

#include <QMouseEvent>

using namespace Realisim;

Widget3d::Widget3d( QWidget* ipParent /*= 0*/,
                    const QGLWidget* shareWidget /*= 0*/,
                    Qt::WindowFlags  iFlags /*= 0*/ )
: QGLWidget( ipParent, shareWidget, iFlags)
{
}

Widget3d::~Widget3d()
{
}

QSize
Widget3d::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize
Widget3d::sizeHint() const
{
    return QSize(200, 200);
}

void
Widget3d::initializeGL()
{
    //useful for lights 
    GLfloat shininess[] = {80.0};
    GLfloat position[]  = {0.0, 50.0, 25.0, 0.0};
    GLfloat ambiant[]   = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[]   = {0.5, 0.5, 0.5, 1.0};
    GLfloat specular[]  = {1.0, 1.0, 1.0, 1.0};

    GLfloat mat_ambiant[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat mat_specular[]  = {0.5f, 0.5f, 0.5f, 1.0f};

    // Let OpenGL clear background to Grey
    glClearColor(125/255.0f, 125/255.0f, 125/255.0f, 0.0);

    glShadeModel(GL_SMOOTH);

    //define material props
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    //init lights
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    //Enable de la lumiere
    glEnable(GL_LIGHT0);

    //Init du ColorMaterial
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void
Widget3d::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -10.0);
}

void
Widget3d::resizeGL(int iWidth, int iHeight)
{
  bool horiz = iWidth >= iHeight;
  
  int windowShortSide = qMin(iWidth, iHeight);
  int windowLongSide = qMax(iWidth, iHeight);
  
  float projectionShortSide = 10.0;
  float projectionLongSide = windowLongSide * projectionShortSide / windowShortSide; 
  
  if( horiz )
  {
    glViewport(0, 0, windowLongSide, windowShortSide);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-projectionLongSide, projectionLongSide,
            projectionShortSide, -projectionShortSide, 
            4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
  }
  else //vertical
  {
    glViewport(0, 0, windowShortSide, windowLongSide );
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-projectionShortSide, projectionShortSide,
            projectionLongSide, -projectionLongSide, 
            4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
  }
  
  updateGL();
  
}

void Widget3d::mousePressEvent(QMouseEvent *event)
{
}

void Widget3d::mouseMoveEvent(QMouseEvent *event)
{
}

void Widget3d::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}