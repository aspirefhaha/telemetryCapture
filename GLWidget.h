#ifndef GLWIDGET_H
#define GLWIDGET_H

#define _3DS_OPENGL_
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "Camera.h"
#include "Format3DS.h"

/*---------------------------------------------------------------------------*/
class GLWidget : public QOpenGLWidget, QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget( int w,int h,QWidget* parent = 0 );
    ~GLWidget( void );
    void initializeGL( void );
    void resizeGL( int width, int height );
    void paintEvent( QPaintEvent* event );
    void keyPressEvent( QKeyEvent* event );
    void wheelEvent( QWheelEvent* event );
    void mouseMoveEvent( QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    // 本类的函数
    void Draw3D( void );
    void Draw2D( void );

public slots:
    void pitchChanged(qreal angle);
private:
    qreal               m_AspectRatio;
    Format3DS           m_3DS;
    Camera              m_Camera;
    qreal               m_pitch;
    qreal               m_roll;
    qreal               m_yaw;
};

#endif // GLWIDGET_H
