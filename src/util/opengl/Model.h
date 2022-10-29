#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include <string>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using std::string;
class Model
{
public:
    Model();
    explicit Model(const string &path);
    ~Model();

    void render(ShaderProgram *program, bool forceColor=false, bool useMeshInfo = true, unsigned int depthMap = -1);

    /// 基础变换矩阵
    glm::mat4 basisTransform = glm::mat4(1.0f);

    /// 模型数据
    vector<Mesh> meshes;

private:
    void loadModel(const string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, const string &name);
    unsigned int loadTexture(const string &filename);

    /// 模型目录
    string m_directory;
    /// 已加载的纹理，避免重复加载
    vector<Texture> m_loadedTextures;

};

#endif