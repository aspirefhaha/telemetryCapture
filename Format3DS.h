#ifndef FORMAT3DS_H
#define FORMAT3DS_H

#include <QString>
#include <QVector>
#include <QColor>
#include <QVector3D>
#include <QMatrix4x4>
#include <QIODevice>
/*---------------------------------------------------------------------------*/
class Chunk// 块信息
{
public:
    Chunk( void )
    {
        id = 0, length = 0, byteRead = 0;
    }
    bool EndOfChunk( void )
    {
        return length == byteRead;
    }
    quint16            id;
    quint32            length;
    quint32            byteRead;
};
/*---------------------------------------------------------------------------*/
class Material// 材质
{
public:
    QString            name;
    QString            filePath;
    QColor             ambient, diffuse, specular;
};
/*---------------------------------------------------------------------------*/
class Vertex// 点
{
public:
    QVector3D          vertex;// 顶点
    QVector3D          normal;// 法向量
};
/*---------------------------------------------------------------------------*/
class Face// 面
{
public:
    QVector3D          normal;// 法向量
    quint16            vertexIndices[3];
    quint16            info;
};
/*---------------------------------------------------------------------------*/
class Object3D// 3D对象
{
public:
    QString                    name;
    QString                    materialName;
    QVector<Vertex>            vertices;
    QVector<Face>              faces;
    QVector<QPointF>           uvCoordinates;
    QVector<quint16>           facesConcernedIndex;
    QMatrix4x4                 matrix;// x86下成员为double，ARM下成员为float
    quint32                    materialID;
    bool                       hasTexture;
};
/*---------------------------------------------------------------------------*/
class Format3DS// 3DS格式类
{
public:
    Format3DS( void );
    ~Format3DS( void );
    bool Load( const QString& fileName );
    bool Load( QIODevice* pDevice );
    void Release( void );
#ifdef _3DS_OPENGL_
    void InitGL( void );
    void RenderGL( void );
    void ReleaseGL( void );
#endif
private:
    void ReadString( QString& str );
    void ReadChunk( Chunk* pChunk );
    void ReadPrimaryChunk( Chunk* pParent );
    void ReadObjectInfoChunk( Chunk* pParent );
    void ReadObjectChunk( Chunk* pParent );
    void ReadMeshChunk( Chunk* pParent );
    void ReadMaterialChunk( Chunk* pParent );
    void CalculateNormals( void );
    void SetMaterials( void );

    QVector<Material>          m_Materials;
    QVector<Object3D>          m_Objects;
    QString                    m_Dir;
    QIODevice*                 m_pDevice;
#ifdef _3DS_OPENGL_
    void CreateTexturesGL( void );
    void RenderListGL( void );
    quint32*                   m_pTextures;
#endif
};
#endif // FORMAT3DS_H
