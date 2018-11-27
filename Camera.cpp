#include <gl.h>
#include <glu.h>
#include <QMatrix4x4>
#include "Camera.h"
/*---------------------------------------------------------------------------*/
#ifdef _3DS_OPENGL_ARM_
#define glTranslater    glTranslatef
#define glMultMatrixr   glMultMatrixf
#else
#define glTranslater    glTranslated
#define glMultMatrixr   glMultMatrixd
#endif
/*---------------------------------------------------------------------------*/
Camera::Camera( void )
{
    m_Pos = QVector3D( 0.0, 0.0, 0.0 );         // 我默认在原点
    m_Forward = QVector3D( 0.0, 0.0, -1.0 );    // 我向Z轴负半轴看
    m_Upward = QVector3D( 0.0, 1.0, 0.0 );      // Y轴正半轴是上
}
/*---------------------------------------------------------------------------*/
void Camera::SetPos( const QVector3D& pos )
{
    m_Pos = pos;
}
/*---------------------------------------------------------------------------*/
void Camera::SetForward(const QVector3D& forward )
{
    m_Forward = forward;
}
/*---------------------------------------------------------------------------*/
void Camera::SetUpward( const QVector3D& upward )
{
    m_Upward = upward;
}
/*---------------------------------------------------------------------------*/
void Camera::MoveForward( const qreal step )
{
    m_Pos += m_Forward * step;
}
/*---------------------------------------------------------------------------*/
void Camera::Apply( void )
{
    // 使用glu类的方法
    //gluLookAt( m_Pos.x( ), m_Pos.y( ), m_Pos.z( ),
    //           m_Forward.x( ), m_Forward.y( ), m_Forward.z( ),
    //           m_Upward.x( ), m_Upward.y( ), m_Upward.z( ) );

    // 自定义的方法
    qreal matrix[16];

    // 首先获取照相机观察和向上向量生成的矩阵
    QVector3D x = QVector3D::crossProduct( m_Upward, -m_Forward );
    QVector3D& y = m_Upward;
    QVector3D z = -m_Forward;
#define MATRIX( row, col ) matrix[ row * 4 + col ]// 临时定义的宏
    MATRIX( 0, 0 ) = x.x( );
    MATRIX( 1, 0 ) = x.y( );
    MATRIX( 2, 0 ) = x.z( );
    MATRIX( 3, 0 ) = 0.0f;
    MATRIX( 0, 1 ) = y.x( );
    MATRIX( 1, 1 ) = y.y( );
    MATRIX( 2, 1 ) = y.z( );
    MATRIX( 3, 1 ) = 0.0f;
    MATRIX( 0, 2 ) = z.x( );
    MATRIX( 1, 2 ) = z.y( );
    MATRIX( 2, 2 ) = z.z( );
    MATRIX( 3, 2 ) = 0.0f;
    MATRIX( 0, 3 ) = 0.0f;
    MATRIX( 1, 3 ) = 0.0f;
    MATRIX( 2, 3 ) = 0.0f;
    MATRIX( 3, 3 ) = 1.0f;
#undef MATRIX

    // 设置矩阵
    glMultMatrixr( matrix );

    // Qt提供方法
    /*QMatrix4x4 matrix;
    matrix.lookAt( m_Pos, m_Forward, m_Upward );
    glMultMatrixr( matrix.data( ) );*/
    //qDebug() << m_Pos.x() << m_Pos.x() << m_Pos.z();
    // 移动模型视图矩阵
    glTranslater( -m_Pos.x( ), -m_Pos.y( ), -m_Pos.z( ) );
}
/*---------------------------------------------------------------------------*/
void Camera::RotateX( qreal angleInDegree )
{
    // 计算旋转矩阵
    QMatrix4x4 rm;
    rm.rotate( angleInDegree, QVector3D::crossProduct( m_Upward, m_Forward ) );

    // 更新向前的向量和向上的向量
    m_Forward = QVector3D(
                rm( 0, 0 ) * m_Forward.x( ) + rm( 1, 0 ) * m_Forward.y( ) + rm( 2, 0 ) * m_Forward.z( ),
                rm( 0, 1 ) * m_Forward.x( ) + rm( 1, 1 ) * m_Forward.y( ) + rm( 2, 1 ) * m_Forward.z( ),
                rm( 0, 2 ) * m_Forward.x( ) + rm( 1, 2 ) * m_Forward.y( ) + rm( 2, 2 ) * m_Forward.z( ) );
    m_Upward = QVector3D(
                rm( 0, 0 ) * m_Upward.x( ) + rm( 1, 0 ) * m_Upward.y( ) + rm( 2, 0 ) * m_Upward.z( ),
                rm( 0, 1 ) * m_Upward.x( ) + rm( 1, 1 ) * m_Upward.y( ) + rm( 2, 1 ) * m_Upward.z( ),
                rm( 0, 2 ) * m_Upward.x( ) + rm( 1, 2 ) * m_Upward.y( ) + rm( 2, 2 ) * m_Upward.z( ) );
}
/*---------------------------------------------------------------------------*/
void Camera::RotateY( qreal angleInDegree )
{
    // 计算旋转矩阵
    QMatrix4x4 rm;
    rm.rotate( angleInDegree, m_Upward );

    // 更新向前的向量即可
    m_Forward = QVector3D(
                rm( 0, 0 ) * m_Forward.x( ) + rm( 1, 0 ) * m_Forward.y( ) + rm( 2, 0 ) * m_Forward.z( ),
                rm( 0, 1 ) * m_Forward.x( ) + rm( 1, 1 ) * m_Forward.y( ) + rm( 2, 1 ) * m_Forward.z( ),
                rm( 0, 2 ) * m_Forward.x( ) + rm( 1, 2 ) * m_Forward.y( ) + rm( 2, 2 ) * m_Forward.z( ) );
}
/*---------------------------------------------------------------------------*/
void Camera::RotateZ( qreal angleInDegree )
{
    // 计算旋转矩阵
    QMatrix4x4 rm;
    rm.rotate( angleInDegree, m_Forward );

    // 更新向上的向量即可
    m_Upward = QVector3D(
                rm( 0, 0 ) * m_Upward.x( ) + rm( 1, 0 ) * m_Upward.y( ) + rm( 2, 0 ) * m_Upward.z( ),
                rm( 0, 1 ) * m_Upward.x( ) + rm( 1, 1 ) * m_Upward.y( ) + rm( 2, 1 ) * m_Upward.z( ),
                rm( 0, 2 ) * m_Upward.x( ) + rm( 1, 2 ) * m_Upward.y( ) + rm( 2, 2 ) * m_Upward.z( ) );
}
/*---------------------------------------------------------------------------*/

