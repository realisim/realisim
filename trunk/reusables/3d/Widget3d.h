/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: Widget3d
* Description: Opengl Widget  to display 3d
*
*
******************************************************************************/

#ifndef Realisim_Widget3d_hh
#define Realisim_Widget3d_hh

namespace Realisim{ class Widget3d; }

#include <QGLWidget>

class Realisim::Widget3d : public QGLWidget
{
    Q_OBJECT
public:
    Widget3d( QWidget* ipParent = 0,
              const QGLWidget* shareWidget = 0,
              Qt::WindowFlags iFlags = 0 );

    virtual ~Widget3d();

signals:
    void clicked();

protected:
    void rotateBy(int xAngle, int yAngle, int zAngle);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void initializeGL();
    void paintGL();
    void resizeGL(int iWidth, int iHeight);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    QColor clearColor;
    QPoint lastPos;
    int xRot;
    int yRot;
    int zRot;
};

#endif //Realisim_Widget3d_hh