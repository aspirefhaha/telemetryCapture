#pragma once

#define _3DS_OPENGL_
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "Camera.h"
#include "Format3DS.h"

/*---------------------------------------------------------------------------*/
class SpaceTrekGLWidget : public QOpenGLWidget, QOpenGLFunctions
{
    Q_OBJECT
public:
    SpaceTrekGLWidget( int w,int h,QWidget* parent = 0 );
    ~SpaceTrekGLWidget( void );
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

