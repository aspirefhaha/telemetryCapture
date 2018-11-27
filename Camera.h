#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>

class Camera
{
public:
    Camera( void );
    void SetPos( const QVector3D& pos );
    void SetForward( const QVector3D& forward );
    void SetUpward(const QVector3D& upward );
    void MoveForward( const qreal step );
    void Apply( void );
    void RotateX( qreal angleInDegree );
    void RotateY( qreal angleInDegree );
    void RotateZ( qreal angleInDegree );

    const QVector3D& Pos( void ){ return m_Pos; }
    const QVector3D& Forward( void ){ return m_Forward; }
    const QVector3D& Upward( void ){ return m_Upward; }
private:
    QVector3D           m_Pos;          // 我的位置
    QVector3D           m_Forward;      // 我的前方在哪儿
    QVector3D           m_Upward;       // 哪个位置是上

};

#endif // CAMERA_H
