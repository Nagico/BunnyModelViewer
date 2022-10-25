#include "Model.h"
#include "Image.h"
#include <iostream>
#include "glad/glad.h"


/// 从文件中加载模型
/// \param path 路径
Model::Model(const string &path)
{
    loadModel(path);
}

Model::~Model()
{

}

/// 渲染模型
/// \param program 着色器对象
void Model::render(ShaderProgram *program, bool useMeshInfo)
{
    for (auto it : m_meshes)
        it.render(program, useMeshInfo);
}

/// 从文件中加载模型
/// \param path 路径
void Model::loadModel(const string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate |   // 将所有多边形转换为三角形
                                             aiProcess_FlipUVs |    // 翻转纹理y轴坐标
                                             aiProcess_GenNormals |   // 生成法线
                                             aiProcess_CalcTangentSpace  // 生成切线和副切线
                                             );
    /*
     * 当使用Assimp导入一个模型的时候，它通常会将整个模型加载进一个场景(Scene)对象，它会包含导入的模型/场景中的所有数据。
     * Assimp会将场景载入为一系列的节点(Node)，每个节点包含了场景对象中所储存数据的索引，每个节点都可以有任意数量的子节点。
     * Assimp数据结构的（简化）模型如下：
     * - 和材质和网格(Mesh)一样，所有的场景/模型数据都包含在Scene对象中。Scene对象也包含了场景根节点的引用。
     * - 场景的Root node（根节点）可能包含子节点（和其它的节点一样），它会有一系列指向场景对象中mMeshes数组中储存的网格数据的索引。
     *   Scene下的mMeshes数组储存了真正的Mesh对象，节点中的mMeshes数组保存的只是场景中网格数组的索引。
     * - 一个Mesh对象本身包含了渲染所需要的所有相关数据，像是顶点位置、法向量、纹理坐标、面(Face)和物体的材质。
     * - 一个网格包含了多个面。Face代表的是物体的渲染图元(Primitive)（三角形、方形、点）。一个面包含了组成图元的顶点的索引。
     *   由于顶点和索引是分开的，使用一个索引缓冲来渲染是非常简单的。
     * - 最后，一个网格也包含了一个Material对象，它包含了一些函数能让我们获取物体的材质属性，比如说颜色和纹理贴图（比如漫反射和镜面光贴图）。
     */

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw std::runtime_error(importer.GetErrorString());

    m_directory = path.substr(0, path.find_last_of("//"));  // 获取模型所在目录
    processNode(scene->mRootNode, scene);  // 递归处理节点
}

/// 处理节点
/// \param node 节点
/// \param scene 场景
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 处理节点所有的网格
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];  // 获取节点的网格
        m_meshes.push_back(processMesh(mesh, scene));  // 处理网格
    }

    // 接下来对它的子节点重复这一过程
    for (size_t i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

/// 处理网格
/// \param mesh 网格
/// \param scene 场景
/// \return 标准化网格对象
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<VertexData> vertices;  // 顶点数据
    vector<unsigned int> indices;  // 索引数据
    vector<Face> faces;  // 面数据
    vector<Texture> textures;  // 纹理数据
    MeshInfo meshInfo;  // 网格信息

    glm::vec3 maxVertex = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
    glm::vec3 minVertex = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);

    // 处理顶点数据 VAO
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        VertexData vertexData;  // 顶点数据

        // 顶点位置
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertexData.position = position;

        // 最值计算
        maxVertex.x = fmax(maxVertex.x, position.x);
        maxVertex.y = fmax(maxVertex.y, position.y);
        maxVertex.z = fmax(maxVertex.z, position.z);
        minVertex.x = fmin(minVertex.x, position.x);
        minVertex.y = fmin(minVertex.y, position.y);
        minVertex.z = fmin(minVertex.z, position.z);

        // 顶点法向量
        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertexData.normal = normal;

        // 顶点纹理坐标
        if (mesh->mTextureCoords[0])  // 只关心第一组纹理坐标
        {
            glm::vec2 texCoord;
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
            vertexData.texCoord = texCoord;
        }
        else vertexData.texCoord = glm::vec2(0.0f, 0.0f);  // 如果没有纹理坐标则设置为0

//        // 顶点切线
//        glm::vec3 tangent;
//        tangent.x = mesh->mTangents[i].x;
//        tangent.y = mesh->mTangents[i].y;
//        tangent.z = mesh->mTangents[i].z;
//        vertexData.tangent = tangent;
//
//        // 顶点副切线
//        glm::vec3 bitangent;
//        bitangent.x = mesh->mBitangents[i].x;
//        bitangent.y = mesh->mBitangents[i].y;
//        bitangent.z = mesh->mBitangents[i].z;
//        vertexData.bitangent = bitangent;

        // 将顶点数据添加到顶点数组中
        vertices.push_back(vertexData);
    }

    // 处理索引数据 EBO 及面数据
    for (auto i = 0; i < mesh->mNumFaces; i++)
    {
        auto face = mesh->mFaces[i];  // 获取面
        if (face.mNumIndices == 3)  // 只处理三角形面
        {
            Face f{};  // 面数据
            for (int j = 0; j < 3; j++)
                f.vertex[j] = face.mIndices[j];
            faces.push_back(f);  // 将面数据添加到面数组中
        }
        else
        {
            std::cout << "Face is not a triangle!" << std::endl;
        }
        for (size_t j = 0; j < face.mNumIndices; j++)  // 遍历面的所有索引
            indices.push_back(face.mIndices[j]);  // 将索引添加到索引数组中
    }


    // 处理材质数据
    if (mesh->mMaterialIndex > 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];  // 获取材质
        vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                           aiTextureType_DIFFUSE, "diffuse");  // 获取漫反射贴图
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());  // 将漫反射贴图添加到纹理数组中

        vector<Texture> specularMaps = loadMaterialTextures(material,
                                                            aiTextureType_SPECULAR, "specular");  // 获取镜面贴图
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());  // 将镜面贴图添加到纹理数组中

        float infoFloat;
        if (material->Get(AI_MATKEY_SHININESS, infoFloat) == AI_SUCCESS)
            meshInfo.shininess = infoFloat;  // 获取材质的高光指数
        if (material->Get(AI_MATKEY_OPACITY, infoFloat) == AI_SUCCESS)
            meshInfo.dissolve = infoFloat;  // 获取材质的透明度
        if (material->Get(AI_MATKEY_REFRACTI, infoFloat) == AI_SUCCESS)
            meshInfo.dissolve = infoFloat;  // 获取材质的折射率

        int infoInt;
        if (material->Get(AI_MATKEY_SHADING_MODEL, infoInt) == AI_SUCCESS)
            meshInfo.illum = infoInt;  // 获取材质的光照模型

        aiColor3D color;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            meshInfo.diffuse = glm::vec3(color.r, color.g, color.b);
        if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
            meshInfo.specular = glm::vec3(color.r, color.g, color.b);
        if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
            meshInfo.ambient = glm::vec3(color.r, color.g, color.b);
        if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
            meshInfo.emission = glm::vec3(color.r, color.g, color.b);

        meshInfo.valid = true;
    }

    // 统计xyz最远距离
    float disX = maxVertex.x - minVertex.x;
    float disY = maxVertex.y - minVertex.y;
    float disZ = maxVertex.z - minVertex.z;

    meshInfo.maxDis = fmax(disX, fmax(disY, disZ));

    return Mesh(vertices, indices, faces, textures, meshInfo);  // 返回标准化网格对象
}

/// 加载材质纹理
/// \param material 材质
/// \param type 纹理类型
/// \param name 纹理名称
/// \return 纹理数组
vector<Texture> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, const string &name)
{
    vector<Texture> textures;  // 纹理数组

    for (size_t i = 0; i < material->GetTextureCount(type); i++)  // 遍历材质中的纹理
    {
        bool loaded = false;  // 纹理是否已经加载
        aiString path;  // 纹理路径
        material->GetTexture(type, i, &path);  // 获取纹理路径

        for (const auto& it : m_loadedTextures)  // 遍历已加载的纹理，检查是否已经加载过
        {
            if (it.path == path.C_Str())  // 路径相同
            {
                textures.push_back(it);  // 将纹理添加到纹理数组中
                loaded = true;  // 纹理已经加载，跳过加载
                break;  // 跳出循环
            }
        }
        if (!loaded)
        {
            Texture texture;
            texture.id = loadTexture(path.C_Str());  // 加载纹理
            texture.name = name;  // 设置纹理名称
            texture.path = path.C_Str();  // 设置纹理路径

            textures.push_back(texture);  // 将纹理添加到纹理数组中
            m_loadedTextures.push_back(texture);  // 将纹理添加到已加载的纹理数组中
        }
    }

    return textures;
}

/// 加载纹理
/// \param filename 纹理文件名
/// \return 纹理ID
unsigned int Model::loadTexture(const string &filename)
{
    Image texture(m_directory + '/' + filename);  // 加载纹理图片
    GLuint textureId;  // 纹理ID
    glGenTextures(1, &textureId);  // 生成纹理ID
    std::cout << "Directory :" << m_directory + '/' + filename << std::endl;
    if (texture.data())  // 加载成功
    {
        GLenum format;  // 纹理格式
        if (texture.channels() == 1)
            format = GL_RED;
        else if (texture.channels() == 3)
            format = GL_RGB;
        else if (texture.channels() == 4)
            format = GL_RGBA;
        else
            throw std::runtime_error("Unsupported texture format.");

        glBindTexture(GL_TEXTURE_2D, textureId);  // 绑定纹理
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, texture.width(), texture.height(),
                     0, format, GL_UNSIGNED_BYTE, texture.data());  // 生成纹理
        glGenerateMipmap(GL_TEXTURE_2D);  // 生成Mipmap
        // 设置纹理环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // 设置纹理过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else throw std::runtime_error( "Open Texture " + filename + " Error!");

    return textureId;
}

Model::Model() {
}


vector<Mesh> Model::getMeshes() const {
    return m_meshes;
}
