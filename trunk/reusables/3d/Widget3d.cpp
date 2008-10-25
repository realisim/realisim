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
    clearColor = Qt::black;
    xRot = 0;
    yRot = 0;
    zRot = 0;
}

Widget3d::~Widget3d()
{
}

void Widget3d::rotateBy(int xAngle, int yAngle, int zAngle)
{
    xRot += xAngle;
    yRot += yAngle;
    zRot += zAngle;
    updateGL();
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
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambiant);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

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
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

void
Widget3d::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -10.0);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

    const int coords[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
    };
    
    for (int i = 0; i < 6; ++i) {
        glBegin(GL_QUADS);
        for (int j = 0; j < 4; ++j) {
            glVertex3d(0.2 * coords[i][j][0], 0.2 * coords[i][j][1],
                       0.2 * coords[i][j][2]);
        }
        glEnd();
    }
}

void
Widget3d::resizeGL(int iWidth, int iHeight)
{
    int side = qMin(iWidth, iHeight);
    glViewport((iWidth - side) / 2, (iHeight - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void Widget3d::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void Widget3d::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        rotateBy(8 * dy, 8 * dx, 0);
    } else if (event->buttons() & Qt::RightButton) {
        rotateBy(8 * dy, 0, 8 * dx);
    }
    lastPos = event->pos();
}

void Widget3d::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}