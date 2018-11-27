// SpaceTrekGLWidget.cpp 包含了主窗口成员函数的实现
// Made By Jiangcaiyang

#include <QtGui>
#include <QtOpenGL>
#ifdef WIN32
#include <GL/glu.h>
#elif __APPLE__
#include <glu.h>
#endif
#include "GLWidget.h"

/*---------------------------------------------------------------------------*/
SpaceTrekGLWidget::SpaceTrekGLWidget( int w,int h,QWidget* parent ):QOpenGLWidget(parent),
    m_pitch(0.0),m_roll(0.0),m_yaw(0.0)
  //,m_LeftPressPos(0,0),m_RightPressPos(0,0)
{
    // 设置窗口
    resize( w, h );                             // 初始大小为640×360（小宽屏）
    setAutoFillBackground( false );                 // 不自动填充背景
    //setAutoBufferSwap( false );                     // 不自动交换缓存
    setWindowTitle( tr( "Qt Load3DS Example" ) );   // 设置窗口标题
    m_AspectRatio = qreal( width( ) ) / qreal( height( ) );// 初始的宽高比

    // 设置摄像机和线程
    m_Camera.SetPos( QVector3D( 0.0, 6.0, 20.0 ) );
    m_3DS.Load( ":/rocket_ok.3ds" );
}
/*---------------------------------------------------------------------------*/
SpaceTrekGLWidget::~SpaceTrekGLWidget( void )
{
    m_3DS.ReleaseGL( );
}
/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::initializeGL( void )
{
    initializeOpenGLFunctions();
    // 隐式调用了makeCurrent()函数
    glEnable( GL_DEPTH_TEST );      // 使用深度测试
    glFrontFace( GL_CW );
    glDisable( GL_CULL_FACE );
    //qglClearColor( QColor( 0, 0, 0 ) );

    m_3DS.InitGL( );
}
/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::resizeGL( int width, int height )
{
    // 改变大小时程序如何应对？
    GLdouble aspectRatio = GLdouble( width ) / GLdouble( height );
    // 设置视口
    if ( aspectRatio < m_AspectRatio )
    {
        GLint smallHeight = GLint( GLdouble( width ) / m_AspectRatio );
        GLint heightBlank = ( GLint( height ) - smallHeight ) / 2;
        glViewport( 0, heightBlank, GLint( width ), smallHeight );
    }
    else
    {
        GLint smallWidth = GLint( GLdouble( height ) * m_AspectRatio );
        GLint widthBlank = ( GLint( width ) - smallWidth ) / 2;
        glViewport( widthBlank, 0, smallWidth, GLint( height ) );
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    // 设置裁剪区域，绘制前层图形（左右下上近远）
    gluPerspective( 45.0, m_AspectRatio, 0.5, 1000.0 );

    // 为模型视图载入标准矩阵
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

}
/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::paintEvent( QPaintEvent* )
{
    Draw3D( );
    Draw2D( );
}
static
QPoint              m_LeftPressPos;
QPoint              m_RightPressPos;
void SpaceTrekGLWidget::mousePressEvent(QMouseEvent * event)
{
    // Q_UNUSED(event);
    // makeCurrent();
    if(event->buttons() & Qt::RightButton){
        m_RightPressPos = event->pos();
    }
    else if(event->buttons() & Qt::LeftButton){
        m_LeftPressPos = event->pos();
    }
}

void SpaceTrekGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    makeCurrent();
    update();
}

void SpaceTrekGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    //Q_UNUSED(event);
    makeCurrent();
    if(event->buttons() & Qt::RightButton)
    {
        //qDebug() << "right Button" ;
        //qDebug() << m_PressPos - event->pos();
        m_pitch = (m_RightPressPos- event->pos()).x();
        m_roll = (m_RightPressPos - event->pos()).y();
        update();
    }
    else if(event->buttons() & Qt::LeftButton)
    {
        m_yaw = (m_LeftPressPos- event->pos()).x();
        m_roll = (m_LeftPressPos - event->pos()).y();
        update();
    }
}
/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::Draw2D( void )
{
    QRect rect( width( ) * 1 / 20, height( ) * 1 / 10, width( ) * 10 / 24, height( ) * 11 / 40 );
    //QString text( tr( "Qt Load3DS example\n[Operations]\nUse [Enter] key to open 3DS file, [Up][Down][Left][Right] key and mouse wheel to move the camera." ) );
    QString text( tr( "yaw %1\npitch %2\nroll %3" ).arg(m_yaw).arg(m_pitch).arg(m_roll) );
    QColor rectColor( 33, 33, 33, 125 ), textColor( 250, 250, 250 );
    QPainter windowPainter( this );
    windowPainter.setPen( QPen( rectColor ) );
    windowPainter.setBrush( QBrush( rectColor ) );
    QFont oriFont = windowPainter.font();
    oriFont.setPointSize(height( )/15);
    windowPainter.setFont(oriFont);
    windowPainter.drawRect( rect );
    windowPainter.setPen( QPen( textColor ) );
    windowPainter.drawText( rect, Qt::AlignCenter | Qt::TextWordWrap, text );
}
/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::Draw3D( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);
    glPushMatrix( );
    m_Camera.Apply( );
        glPushMatrix();
        glRotatef(m_yaw,0,0,1);
        glRotatef(m_pitch,1,0,0);
        glRotatef(m_roll,0,1,0);
        m_3DS.RenderGL( );
        glPopMatrix();
    glPopMatrix( );
    glDisable(GL_DEPTH_TEST);
}

void SpaceTrekGLWidget::pitchChanged(qreal angle)
{
    makeCurrent();
    m_pitch=angle;
    update();
}

/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::keyPressEvent( QKeyEvent* event )
{
    static bool m_openLight = false;
    switch ( event->key( ) )
    {
    case Qt::Key_L:
        m_openLight = !m_openLight;
        if ( !m_openLight )
        {
          glDisable( GL_LIGHTING );
        }
        else
        {
          glEnable( GL_LIGHTING );
        }
        //updateGL();
        break;

    case Qt::Key_Up:
        m_Camera.RotateX( 1 ); break;
    case Qt::Key_Down:
        m_Camera.RotateX( -1 ); break;
    case Qt::Key_Left:
        m_Camera.RotateY( -1 ); break;
    case Qt::Key_Right:
        m_Camera.RotateY( 1 ); break;

    case Qt::Key_Y:
        m_yaw += 10.0; break;
    case Qt::Key_I:
        m_yaw -= 10.0; break;

    case Qt::Key_U:
        m_pitch += 10.0; break;
    case Qt::Key_J:
        m_pitch -= 10.0; break;

    case Qt::Key_H:
        m_roll += 10.0; break;
    case Qt::Key_K:
        m_roll -= 10.0; break;


    case Qt::Key_A:
    {
        QVector3D oldpos = m_Camera.Pos();
        oldpos += QVector3D(1,0,0);
        m_Camera.SetPos(oldpos);
        break;
    }
    case Qt::Key_D:
    {
        QVector3D oldpos = m_Camera.Pos();
        oldpos += QVector3D(-1,0,0);
        m_Camera.SetPos(oldpos);
        break;
    }

    case Qt::Key_S:
    {
        QVector3D oldpos = m_Camera.Pos();
        oldpos += QVector3D(0,1,0);
        m_Camera.SetPos(oldpos);
        break;
    }
    case Qt::Key_W:
    {
        QVector3D oldpos = m_Camera.Pos();
        oldpos += QVector3D(0,-1,0);
        m_Camera.SetPos(oldpos);
        break;
    }
    case Qt::Key_Return:
    {
        QString fileName = QFileDialog::getOpenFileName( this,
                                                         windowTitle( ),
                                                         QDir::homePath( ),
                                                         tr( "3DS model file (*.3ds)" ) );
        m_3DS.Load( fileName );
        m_3DS.InitGL( );
    }
    }
    update( );
}
/*---------------------------------------------------------------------------*/
void SpaceTrekGLWidget::wheelEvent( QWheelEvent* event )
{
    m_Camera.MoveForward( qreal( event->delta( ) ) / 100.0 );
    emit update( );
}
