#define _3DS_OPENGL_
#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#elif __APPLE__
#include <gl.h>
#include <glext.h>
#endif
#include <QImage>
#include <QMessageBox>
#include "Format3DS.h"

/*---------------------------------------------------------------------------*/
#ifdef _3DS_OPENGL_ARM_
#define glMultMatrixr   glMultMatrixf
#define glTexCoord2r    glTexCoord2f
#define glColor3r       glColor3f
#define glNormal3r      glNormal3f
#define glVertex3r      glVertex3f
#else
#define glMultMatrixr   glMultMatrixd
#define glTexCoord2r    glTexCoord2d
#define glColor3r       glColor3d
#define glNormal3r      glNormal3d
#define glVertex3r      glVertex3d
#endif
/*---------------------------------------------------------------------------*/
void Format3DS::InitGL( void )
{
    if ( !m_Materials.isEmpty( ) ) CreateTexturesGL( );
}
/*---------------------------------------------------------------------------*/
void Format3DS::CreateTexturesGL( void )
{
    // 初始化纹理
    glEnable( GL_TEXTURE_2D );// 启用2D纹理
    m_pTextures = new quint32[m_Materials.size( )];
    glGenTextures( m_Materials.size( ), (GLuint*)m_pTextures );

    // 载入和设置纹理
    QImage image;
    for ( int i = 0; i < m_Materials.size( ); ++i )
    {
        glBindTexture( GL_TEXTURE_2D, m_pTextures[i] );
        if ( image.load( m_Materials[i].filePath ) )// 成功地载入图像文件
        {
            GLint internalFormat;
            GLint textureType;
            switch ( image.format( ) )
            {
            case QImage::Format_ARGB32:
                internalFormat = GL_RGBA8;
                textureType = GL_UNSIGNED_BYTE;
                break;
            case QImage::Format_RGB32:
                internalFormat = GL_RGB8;
                textureType = GL_UNSIGNED_BYTE;
                break;
            case QImage::Format_RGB16:
                internalFormat = GL_RGB16;
                textureType = GL_UNSIGNED_SHORT_5_6_5;
                break;
            default:// 其余格式不测试，统一为GL_RGBA8
                internalFormat = GL_RGBA8;
                textureType = GL_UNSIGNED_BYTE;
            }

            // 纹理被加载会经历像素包装、像素缩放、颜色表、卷积操作
            glTexImage2D( GL_TEXTURE_2D,					// 目标
                          0,								// 级别
                          internalFormat,                   // 纹理内部格式
                          image.width( ),					// 纹理的宽（最好2的次方）
                          image.height( ),              	// 纹理的高（最好2的次方）
                          0,            					// 纹理的深度（最好2的次方）
                          GL_BGRA,                       	// 纹理单元格式
                          textureType,          			// 像素的数据类型
                          image.mirrored( ).constBits( ) );	// 数据指针

            // 设置滤波方式
            glTexParameteri( GL_TEXTURE_2D,                 // 目标纹理，必须为GL_TEXTURE_2D
                             GL_TEXTURE_MIN_FILTER,         // 用来设置的变量名称
                             GL_LINEAR );                   // 变量的值
            glTexParameteri( GL_TEXTURE_2D,                 // 目标纹理，必须为GL_TEXTURE_2D
                             GL_TEXTURE_MAG_FILTER,         // 用来设置的变量名称
                             GL_LINEAR );                   // 变量的值

            // 设置纹理环绕模式（STR（相当于XYZ）中的ST方向忽略边缘纹理采样）
            glTexParameteri( GL_TEXTURE_2D,                 // 目标纹理，必须为GL_TEXTURE_2D
                             GL_TEXTURE_WRAP_S,             // 用来设置的变量名称
                             GL_REPEAT );                   // 变量的值
            glTexParameteri( GL_TEXTURE_2D,                 // 目标纹理，必须为GL_TEXTURE_2D
                             GL_TEXTURE_WRAP_T,             // 用来设置的变量名称
                             GL_REPEAT );                   // 变量的值

            // 设置颜色混合模式
            glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );// 替换模式
        }
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::RenderListGL( void )// 渲染链表
{

}
/*---------------------------------------------------------------------------*/
void Format3DS::RenderGL( void )
{
    // 遍历3DS的所有对象
    for ( int i = 0; i < m_Objects.size( ); ++i )
    {
        Object3D& object = m_Objects[i];

        // 应用模型矩阵
        glPushMatrix( );
        glMultMatrixf( object.matrix.data( ) );

        if ( object.materialID == -1 )// 没有材质
        {
            glColor3ub( 255, 255, 255 );// 默认为白色
        }
        else// 有材质
        {
            if ( object.hasTexture )// 有纹理，启用纹理
            {
                glEnable( GL_TEXTURE_2D );
                glBindTexture( GL_TEXTURE_2D, m_pTextures[object.materialID] );
            }
            else// 无纹理，禁用纹理
            {
                glDisable( GL_TEXTURE_2D );
            }
        }

        glBegin( GL_TRIANGLES );// 必要时可以修改渲染图元的方式
        {
            for ( int j = 0; j < object.faces.size( ); ++j )
            {
                Face& face = object.faces[j];
                for ( int k = 0; k < 3; ++k )
                {
                    quint16 index = face.vertexIndices[k];
                    if ( object.materialID != -1 )// 有材质
                    {
                        if ( object.hasTexture )// 有纹理
                        {
                            QPointF& uv = object.uvCoordinates[index];
                            glTexCoord2r( uv.x( ), uv.y( ) );
                        }
                        else// 没有纹理，也有颜色材质
                        {
                            QColor& diffuse = m_Materials[object.materialID].diffuse;
                            glColor3r( diffuse.redF( ),
                                       diffuse.greenF( ),
                                       diffuse.blueF( ) );
                        }
                    }
                    QVector3D& normal = object.vertices[index].normal;
                    QVector3D& vertex = object.vertices[index].vertex;
                    glNormal3r( normal.x( ), normal.y( ), normal.z( ) );
                    glVertex3r( vertex.x( ), vertex.y( ), vertex.z( ) );
                }
            }
        }
        glEnd( );
        glPopMatrix( );
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReleaseGL( void )
{
    glDeleteTextures( m_Materials.size( ), (GLuint*)m_pTextures );
    delete m_pTextures;
}
