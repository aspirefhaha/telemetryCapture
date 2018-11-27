//#include <iostream>
#include <QtCore>
#include "Format3DS.h"
#include <QDebug>
/*---------------------------------------------------------------------------*/
// 有关3DS各个块名称的宏
#define PRIMARY                                 0x4D4D
#define     OBJECTINFO                          0x3D3D
#define         MATERIAL                        0xAFFF
#define             MAT_NAME                    0xA000
#define             MAT_AMBIENT                 0xA010
#define             MAT_DIFFUSE                 0xA020
#define             MAT_SPECULAR                0xA030
#define             MAT_SHINESS                 0xA040
#define             MAT_TRANSPARENCY            0xA050
#define             MAT_SELF_ILLUM              0xA080
#define             MAT_WIRE                    0xA085
#define             MAT_TEXTURE_MAP             0xA200
#define             MAT_SPECULAR_MAP            0xA204// This is a header for a new specular map
#define             MAT_OPACITY_MAP             0xA210// This is a header for a new opacity map
#define             MAT_REFLECTION_MAP          0xA220// This is a header for a new reflection map
#define             MAT_BUMP_MAP                0xA230// This is a header for a new bump map
#define             MAT_MAP_FILEPATH            0xA300// This holds the file name of the texture

#define             MAT_MAP_TILING              0xA351// 2nd bit (from LSB) is mirror UV flag
#define             MAT_MAP_USCALE              0xA354// U axis scaling
#define             MAT_MAP_VSCALE              0xA356// V axis scaling
#define             MAT_MAP_UOFFSET             0xA358// U axis offset
#define             MAT_MAP_VOFFSET             0xA35A// V axis offset
#define             MAT_MAP_ANG                 0xA35C// UV rotation around the z-axis in rad

#define             MAT_FLOAT_COLOR             0x0010// color defined as 3 floats
#define             MAT_24BIT_COLOR             0x0011// color defined as 3 bytes
#define         OBJECT                          0x4000
#define             OBJECT_MESH                 0x4100// This lets us know that we are reading a new object
#define                 OBJECT_VERTICES         0x4110// The objects vertices
#define                 OBJECT_FACES            0x4120// The objects faces
#define                 OBJECT_MATERIAL         0x4130// This is found if the object has a material, either texture map or color
#define                 OBJECT_UV               0x4140// The UV texture coordinates
#define                 OBJECT_TRANS_MATRIX     0x4160// The Object Matrix
#define             OBJECT_LAMP                 0x4600// This lets un know we are reading a light object
#define             OBJECT_LAMP_SPOT            0x4610// The light is a spotlight.
#define             OBJECT_LAMP_OFF             0x4620// The light off.
#define             OBJECT_LAMP_ATTENUATE       0x4625
#define             OBJECT_LAMP_RAYSHADE        0x4627
#define             OBJECT_LAMP_SHADOWED        0x4630
#define             OBJECT_LAMP_LOCAL_SHADOW    0x4640
#define             OBJECT_LAMP_LOCAL_SHADOW2   0x4641
#define             OBJECT_LAMP_SEE_CONE        0x4650
#define             OBJECT_LAMP_SPOT_RECTANGULAR    0x4651
#define             OBJECT_LAMP_SPOT_OVERSHOOT  0x4652
#define             OBJECT_LAMP_SPOT_PROJECTOR  0x4653
#define             OBJECT_LAMP_EXCLUDE         0x4654
#define             OBJECT_LAMP_RANGE           0x4655
#define             OBJECT_LAMP_ROLL            0x4656
#define             OBJECT_LAMP_SPOT_ASPECT     0x4657
#define             OBJECT_LAMP_RAY_BIAS        0x4658
#define             OBJECT_LAMP_INNER_RANGE     0x4659
#define             OBJECT_LAMP_OUTER_RANGE     0x465A
#define             OBJECT_LAMP_MULTIPLIER      0x465B
#define             OBJECT_LAMP_AMBIENT_LIGHT   0x4680
#define             OBJECT_CAMERA               0x4700// This lets un know we are reading a camera object
#define                 OBJECT_CAM_RANGES       0x4720// The camera range values
#define     VERSION                             0x0002
#define     EDITKEYFRAME                        0xB000
#define         ED_KEY_AMBIENT_NODE             0xB001
#define         ED_KEY_OBJECT_NODE              0xB002
#define             EK_OB_KEYFRAME_SEG          0xB008
#define             EK_OB_KEYFRAME_CURTIME      0xB009
#define             EK_OB_KEYFRAME_HEADER       0xB00A
#define             EK_OB_NODE_HEADER           0xB010
#define             EK_OB_INSTANCE_NAME         0xB011
#define             EK_OB_PRESCALE              0xB012
#define             EK_OB_PIVOT                 0xB013
#define             EK_OB_BOUNDBOX              0xB014
#define             EK_OB_MORPH_SMOOTH          0xB015
#define             EK_OB_POSITION_TRACK        0xB020
#define             EK_OB_ROTATION_TRACK        0xB021
#define             EK_OB_SCALE_TRACK           0xB022
#define             EK_OB_CAMERA_FOV_TRACK      0xB023
#define             EK_OB_CAMERA_ROLL_TRACK     0xB024
#define             EK_OB_COLOR_TRACK           0xB025
#define             EK_OB_MORPH_TRACK           0xB026
#define             EK_OB_HOTSPOT_TRACK         0xB027
#define             EK_OB_FALLOF_TRACK          0xB028
#define             EK_OB_HIDE_TRACK            0xB029
#define             EK_OB_NODE_ID               0xB030
#define         ED_KEY_CAMERA_NODE              0xB003
#define         ED_KEY_TARGET_NODE              0xB004
#define         ED_KEY_LIGHT_NODE               0xB005
#define         ED_KEY_L_TARGET_NODE            0xB006
#define         ED_KEY_SPOTLIGHT_NODE           0xB007
/*---------------------------------------------------------------------------*/
Format3DS::Format3DS( void )
{

}
/*---------------------------------------------------------------------------*/
Format3DS::~Format3DS( void )
{
    Release( );
}
/*---------------------------------------------------------------------------*/
void Format3DS::Release( void )
{
}
/*---------------------------------------------------------------------------*/
bool Format3DS::Load( const QString& fileName )
{
    // 获取文件的父文件夹的绝对路径
    QFileInfo fileInfo( fileName );
    m_Dir = fileInfo.dir( ).absolutePath( ) + '/';

    // 读取文件
    qDebug() << "Open File:" << fileName;
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
        return false;
    else return Load( &file );
}
/*---------------------------------------------------------------------------*/
bool Format3DS::Load( QIODevice* pDevice )
{
    m_pDevice = pDevice;
    m_Materials.clear( );
    m_Objects.clear( );
    Chunk chunk;

    // 读取基本块
    ReadChunk( &chunk );
    if ( chunk.id != PRIMARY )
    {
        //std::cerr << "[x]This file is not 3ds file.\n";
        return false;
    }

    // 读取主块
    ReadPrimaryChunk( &chunk );
    CalculateNormals( );
    SetMaterials( );

    return true;
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadChunk( Chunk* pChunk )
{
    //assert( pChunk != 0 );
    m_pDevice->read( (char*)&pChunk->id, 2 );
    m_pDevice->read( (char*)&pChunk->length, 4 );
    pChunk->byteRead += 6;
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadString( QString& str )// 读取字符串
{
    char ch;
    do
    {
        m_pDevice->read( &ch, 1 );
        str += ch;
    }
    while ( ch != 0 );
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadPrimaryChunk( Chunk* pParent )
{
    //assert( pParent != 0 );
    while ( !pParent->EndOfChunk( ) )
    {
        Chunk chunk;
        ReadChunk( &chunk );

        switch ( chunk.id )
        {
        case EDITKEYFRAME:// 关键帧
            //std::cerr << "[i]Reading key frame chunk.\n";
            break;
        case OBJECTINFO:// 对象的信息
            //std::cerr << "[i]Reading object info chunk.\n";
            ReadObjectInfoChunk( &chunk );
            break;
        case VERSION:// 版本
        {
            //std::cerr << "[i]Reading version chunk.\n";
            quint32 version;
            m_pDevice->read( (char*)&version, 4 );
            //std::cerr << "[i]The version of 3DS file is: " << version << '\n';
        }
            break;
        default:
            //std::cerr << "[x]No matching chunk id(0x" << std::hex << chunk.id << std::dec <<
            //             ") in Format3DS::ReadPrimaryChunk(). Skipping...\n";
            m_pDevice->seek( m_pDevice->pos( ) + chunk.length - 6 );// 跳过读取chunk.length - 6个字节
        }
        pParent->byteRead += chunk.length;
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadObjectInfoChunk( Chunk *pParent )
{
    //assert( pParent != 0 );
    while ( !pParent->EndOfChunk( ) )
    {
        Chunk chunk;
        ReadChunk( &chunk );

        switch ( chunk.id )
        {
        case MATERIAL:// 材质
            //std::cerr << "[i]Reading material chunk.\n";
            m_Materials.append( Material( ) );
            //std::cerr << "   material size: " << m_Materials.size( ) << '\n';
            ReadMaterialChunk( &chunk );
            break;
        case OBJECT:// 对象
        {
            //std::cerr << "[i]Reading object chunk.\n";
            m_Objects.append( Object3D( ) );
            Object3D& object = m_Objects.last( );
            ReadString( object.name );
            //std::cerr << "   Object name: " << qPrintable( object.name ) << '\n';
            chunk.byteRead += object.name.length( );// 附带了\0这一字节
            ReadObjectChunk( &chunk );
        }
            break;
        default:
            //std::cerr << "[x]No matching chunk id( 0x" << std::hex<< chunk.id << std::dec <<
            //             ") in Format3DS::ReadObjectInfoChunk(). Skipping...\n";
            m_pDevice->seek( m_pDevice->pos( ) + chunk.length - 6 );// 跳过读取chunk.length - 6个字节
        }
        pParent->byteRead += chunk.length;
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadObjectChunk( Chunk* pParent )
{
    //assert( pParent != 0 );

    while ( !pParent->EndOfChunk( ) )
    {
        Chunk chunk;
        ReadChunk( &chunk );

        switch ( chunk.id )
        {
        case OBJECT_MESH:// 网格
            //std::cerr << "[i]Reading object mesh chunk.\n";
            ReadMeshChunk( &chunk );
            break;
        default:
            //std::cerr << "[x]No matching chunk id( 0x" << std::hex<< chunk.id << std::dec <<
            //             ") in Format3DS::ReadObjectChunk(). Skipping...\n";
            m_pDevice->seek( m_pDevice->pos( ) + chunk.length - 6 );// 跳过读取chunk.length - 6个字节
        }
        pParent->byteRead += chunk.length;
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadMeshChunk( Chunk* pParent )
{
    //assert( pParent != 0 );
    Object3D& object = m_Objects.last( );

    while ( !pParent->EndOfChunk( ) )
    {
        Chunk chunk;
        bool flawFlag = false;// 缺陷的标识符
        ReadChunk( &chunk );

        switch ( chunk.id )
        {
        case OBJECT_VERTICES:// 顶点
        {
            //std::cerr << "[i]Reading object mesh vertices chunk.\n";
            quint16             vertexNum;
            float               x, y, z;
            Vertex              vertex;

            m_pDevice->read( (char*)&vertexNum, 2 );// 读取顶点个数
            //std::cerr << "   vertex number: " << vertexNum << '\n';
            for ( quint16 i = 0; i < vertexNum; ++i )
            {
                // 读取XYZ坐标
                m_pDevice->read( (char*)&x, 4 ); vertex.vertex.setX( qreal( x ) );
                m_pDevice->read( (char*)&y, 4 ); vertex.vertex.setY( qreal( y ) );
                m_pDevice->read( (char*)&z, 4 ); vertex.vertex.setZ( qreal( z ) );
                object.vertices.append( vertex );
                //std::cerr << "   [x: " << vertex.vertex.x( ) <<
                //             "][y: " << vertex.vertex.y( ) <<
                //             "][z: " << vertex.vertex.z( ) << "]\n";
            }
        }
            break;
        case OBJECT_FACES:// 面
        {
            //std::cerr << "[i]Reading object mesh faces chunk.\n";
            quint16             faceNum;

            m_pDevice->read( (char*)&faceNum, 2 );// 读取面的个数
            //std::cerr << "   face number: " << faceNum << '\n';
            for ( quint16 i = 0; i < faceNum; ++i )
            {
                Face face;
                m_pDevice->read( (char*)&face.vertexIndices[0], 6 );
                m_pDevice->read( (char*)&face.info, 2 );
                object.faces.append( face );
            }

            // flaw：这里的chunk.length与该chunk所包含的内容不符，因此不采用chunk.length
            pParent->byteRead += faceNum * 8 + 2 + 6;// 加上已经读取的字节数
            flawFlag = true;// 设置了缺陷标识符
        }
            break;
        case OBJECT_MATERIAL:// 对象材质
        {
            //std::cerr << "[i]Reading object mesh material chunk.\n";
            ReadString( object.materialName );
            //std::cerr << "   Object material name: " << qPrintable( object.materialName ) << '\n';

            quint16 concernFaceNum;// 有多少面和这个材质相关？
            quint16 faceIndex;

            m_pDevice->read( (char*)&concernFaceNum, 2 );
            //std::cerr << "   face concerned number: " << concernFaceNum << '\n';
            for ( quint16 i = 0; i < concernFaceNum; ++i )
            {
                m_pDevice->read( (char*)&faceIndex, 2 );
                object.facesConcernedIndex.append( faceIndex );
                //std::cerr << "   face concerned index: " << faceIndex << '\n';
            }
        }
            break;
        case OBJECT_UV:// 对象UV坐标
        {
            //std::cerr << "[i]Reading object mesh uv coordiate chunk.\n";
            quint16 uvNum;// UV坐标的数量
            float u, v;

            m_pDevice->read( (char*)&uvNum, 2 );
            for ( quint16 i = 0; i < uvNum; ++i )
            {
                m_pDevice->read( (char*)&u, 4 );
                m_pDevice->read( (char*)&v, 4 );
                object.uvCoordinates.append( QPointF( qreal( u ), qreal( v ) ) );
                //std::cerr << "   [u: " << u << "][v: " << v << "]\n";
            }
        }
            break;
        case OBJECT_TRANS_MATRIX:// 对象矩阵
        {
            //std::cerr << "[i]Reading object mesh matrix chunk.\n";
            float matrix[16];
            m_pDevice->read( (char*)&matrix[0], 12 ); matrix[3] = 0.0f;
            m_pDevice->read( (char*)&matrix[4], 12 ); matrix[7] = 0.0f;
            m_pDevice->read( (char*)&matrix[8], 12 ); matrix[11] = 0.0f;
            m_pDevice->read( (char*)&matrix[12], 12 ); matrix[15] = 1.0f;
            //std::cerr << "   object mesh matrix:\n";
            for ( quint16 i = 0; i < 4; ++i )
            {
                object.matrix.setColumn( i,
                                         QVector4D( qreal( matrix[4 * i] ), qreal( matrix[4 * i + 1] ),
                        qreal( matrix[4 * i + 2] ), qreal( matrix[4 * i + 3] ) ) );
                //object.matrix.transposed( );// 转置
                //std::cerr << matrix[4 * i] << '\t' <<
                //                              matrix[4 * i + 1] << '\t' <<
                //                              matrix[4 * i + 2] << '\t' <<
                //                              matrix[4 * i + 3] << '\n';
            }
        }
            break;
        default:
            //std::cerr << "[x]No matching chunk id( 0x" << std::hex << chunk.id << std::dec <<
            //             ") in Format3DS::ReadMeshChunk(). Skipping...\n";
            m_pDevice->seek( m_pDevice->pos( ) + chunk.length - 6 );// 跳过读取chunk.length - 6个字节
        }
        if ( !flawFlag ) pParent->byteRead += chunk.length;
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::ReadMaterialChunk( Chunk* pParent )
{
    //assert( pParent != 0 );
    Material& material = m_Materials.last( );

    while ( !pParent->EndOfChunk( ) )
    {
        Chunk chunk;
        ReadChunk( &chunk );

        switch ( chunk.id )
        {
        case MAT_NAME:// 材质的名称
            //std::cerr << "[i]Reading material name chunk.\n";
            ReadString( material.name );
            //std::cerr << "   Material name: " << qPrintable( material.name ) << '\n';
            break;
        case MAT_TEXTURE_MAP:// 材质使用纹理的映射
            //std::cerr << "[i]Reading material texture map.\n";
            ReadMaterialChunk( &chunk );
            break;
        case MAT_MAP_FILEPATH:// 文件路径
            //std::cerr << "[i]Reading material file path chunk.\n";
            ReadString( material.filePath );
            material.filePath = m_Dir + material.filePath;
            //std::cerr << "   Texture file path: " << qPrintable( material.filePath ) << '\n';
            break;
        case MAT_AMBIENT:// 材质环境光
            //std::cerr << "[i]Reading material ambient color chunk.\n";
            ReadMaterialChunk( &chunk );
            //std::cerr << "   [red: " << material.ambient.red( ) <<
            //             "][green: " << material.ambient.green( ) <<
            //             "][blue: " << material.ambient.blue( ) << "]\n";
            break;
        case MAT_DIFFUSE:// 材质漫反射光
            //std::cerr << "[i]Reading material diffuse color chunk.\n";
            ReadMaterialChunk( &chunk );
            //std::cerr << "   [red: " << material.ambient.red( ) <<
            //             "][green: " << material.ambient.green( ) <<
            //             "][blue: " << material.ambient.blue( ) << "]\n";
            break;
        case MAT_SPECULAR:// 材质镜面反射光
            //std::cerr << "[i]Reading material specular color chunk.\n";
            ReadMaterialChunk( &chunk );
            //std::cerr << "   [red: " << material.ambient.red( ) <<
            //             "][green: " << material.ambient.green( ) <<
            //             "][blue: " << material.ambient.blue( ) << "]\n";
            break;
        case MAT_FLOAT_COLOR:// 浮点的颜色
        {
            float red, green, blue;
            QColor color;

            //std::cerr << "[i]Reading float color chunk.\n";
            m_pDevice->read( (char*)&red, 4 );
            m_pDevice->read( (char*)&green, 4 );
            m_pDevice->read( (char*)&blue, 4 );
            color.setRgbF( qreal( red ), qreal( green ), qreal( blue ) );

            switch ( pParent->id )
            {
            case MAT_AMBIENT:
                material.ambient = color; break;
            case MAT_DIFFUSE:
                material.diffuse = color; break;
            case MAT_SPECULAR:
                material.specular = color; break;
            default:
                //std::cerr << "[x]No matching color.\n"
                ;
            }
        }
            break;
        case MAT_24BIT_COLOR:// 3字节颜色
        {
            quint8 red, green, blue;
            QColor color;

            //std::cerr << "[i]Reading 24bit color chunk.\n";
            m_pDevice->read( (char*)&red, 1 );
            m_pDevice->read( (char*)&green, 1 );
            m_pDevice->read( (char*)&blue, 1 );
            color.setRgb( qRgb( red, green, blue ) );

            switch ( pParent->id )
            {
            case MAT_AMBIENT:
                material.ambient = color; break;
            case MAT_DIFFUSE:
                material.diffuse = color; break;
            case MAT_SPECULAR:
                material.specular = color; break;
            default:
                //std::cerr << "[x]No matching color.\n"
                ;
            }
        }
            break;
        default:
            //std::cerr << "[x]No matching chunk id( 0x" << std::hex<< chunk.id << std::dec <<
            //             ") in Format3DS::ReadMaterialChunk(). Skipping...\n";
            m_pDevice->seek( m_pDevice->pos( ) + chunk.length - 6 );// 跳过读取chunk.length - 6个字节
        }
        pParent->byteRead += chunk.length;
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::CalculateNormals( void )
{
    for ( int i = 0; i < m_Objects.size( ); ++i )
    {
        Object3D& object = m_Objects[i];
        for ( int j = 0; j < object.faces.size( ); ++j )
        {
            Face& face = object.faces[j];

            // 计算面的法向量
            face.normal = QVector3D::normal( object.vertices[face.vertexIndices[0]].vertex,
                    object.vertices[face.vertexIndices[1]].vertex,
                    object.vertices[face.vertexIndices[2]].vertex );

            // 增加顶点的法向量值，随后会求均值而计算出最终的顶点法向量
            object.vertices[face.vertexIndices[0]].normal += face.normal;
            object.vertices[face.vertexIndices[1]].normal += face.normal;
            object.vertices[face.vertexIndices[2]].normal += face.normal;
        }

        for ( int k = 0; k < object.vertices.size( ); ++k )
        {
            object.vertices[k].normal.normalize( );// 归一化成为法向量
        }
    }
}
/*---------------------------------------------------------------------------*/
void Format3DS::SetMaterials( void )
{
    // 为3D对象设置材质ID
    for ( int i = 0; i < m_Objects.size( ); ++i )
    {
        Object3D& object = m_Objects[i];
        int j;
        for ( j = 0; j < m_Materials.size( ); ++j )
        {
            if ( object.materialName == m_Materials[j].name )
            {
                if ( QFile( m_Materials[j].filePath ).exists( ) )
                {
                    object.hasTexture = true;// 有纹理
                }
                else object.hasTexture = false;
                object.materialID = j; break;// 找到对应的材质，ID设为i
            }
        }
        if ( j == m_Materials.size( ) ) object.materialID = -1;// 没有找到对应的材质那么ID设为-1
    }
}
/*---------------------------------------------------------------------------*/
