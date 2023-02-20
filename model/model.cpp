#include "model.h"

#include <fstream>

#include "bvh/bvh.h"

static int model_num = 0;

Model::Model(Material *mat, Texture *tex): material(mat), texture(tex), id(model_num++) {

}

Model::~Model() {
    delete material;
    delete texture;
}

CustomizedModel::CustomizedModel(const std::string &path, const Vector3f &eye, Material *mat, Texture *tex): Model(mat, tex) {
    glGenBuffers(1, &patch_tbo);
    glGenTextures(1, &patch_tex);
    glGenBuffers(1, &bvh_tbo);
    glGenTextures(1, &bvh_tex);

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "obj file is not found" << std::endl;
        exit(0);
    }

    //检查文件开头的注释，以确保是自己定义的正确格式
    char c;
    file >> c;
    if (c != '#') {
        std::cout << "unexpected content" << std::endl;
        exit(0);
    }

    //数据大小
    GLsizei vertex_num, index_num;
    file >> vertex_num >> index_num;
    patch_num = index_num / 4;
    auto vertices = new Vector3f[vertex_num];
    auto normals = new Vector3f[vertex_num];
    auto indices = new GLuint[index_num];
    patches.assign(patch_num, {});

    //顶点坐标
    GLfloat lowest = 10.0f, highest = -10.0f, widest = -10.0f, x, y, z;
    for (int i = 0; i < vertex_num; i++) {
        file >> c >> x >> y >> z;
        if (y < lowest) lowest = y;
        if (y > highest) highest = y;
        if (x > widest) widest = x;
        vertices[i] = {x, y, z};
    }

    center.y = lowest;
    radius = widest;
    height = highest - lowest;

    //顶点法矢量
    for (int i = 0; i < vertex_num; i++)
        file >> c >> c >> normals[i].x >> normals[i].y >> normals[i].z;

    //忽略纹理坐标
    GLfloat ign;
    for (int i = 0; i < vertex_num; i++)
        file >> c >> c >> ign >> ign;

    //顶点索引
    GLuint tmp;
    for (int i = 0; i < index_num; i += 4) {
        file >> c;
        for (int j = 0; j < 4; j++) {
            file >> tmp;
            indices[i + j] = --tmp;
            file >> c >> tmp >> c >> tmp;
        }
    }

    //面片数据
    int idx = 0;
    GLuint idx1, idx2, idx3, idx4;
    for (int i = 0; i < patch_num; i++) {
        idx1 = indices[idx++];
        idx2 = indices[idx++];
        idx3 = indices[idx++];
        idx4 = indices[idx++];
        patches[i].normal = normalize(normals[idx1] + normals[idx2] + normals[idx3] + normals[idx4]);
        patches[i].samples[1] = vertices[idx1];
        patches[i].samples[3] = vertices[idx2];
        patches[i].samples[2] = vertices[idx3];
        patches[i].samples[0] = vertices[idx4];
    }

    file.close();
}

CustomizedModel::~CustomizedModel() {
    glDeleteBuffers(1, &patch_tbo);
    glDeleteTextures(1, &patch_tex);
    glDeleteBuffers(1, &bvh_tbo);
    glDeleteTextures(1, &bvh_tex);
    delete[] bvh;
}

MODEL_TYPE CustomizedModel::type() {
    return CUSTOMIZED;
}

GLfloat CustomizedModel::hit(Ray r) {
    //按照矩形包围面片处理
    GLfloat t = (center.z - r.startPoint.z) / r.direction.z;
    Vector3f P = r.startPoint + r.direction * t;
    if (P.x >= center.x - radius && P.x <= center.x + radius &&
        P.y >= center.y && P.y <= center.y + height) return t;
    return -1.0f;
}

GLuint CustomizedModel::getPatchTex() {
    return patch_tex;
}

GLuint CustomizedModel::getBVHTex() {
    return bvh_tex;
}

Vector3f CustomizedModel::getCenter() {
    return center;
}

GLfloat CustomizedModel::getHeight() {
    return height;
}

void CustomizedModel::trans(GLfloat scale, Vector3f move) {
    for (auto &patch : patches) {
        patch.samples[0] *= scale;
        patch.samples[1] *= scale;
        patch.samples[2] *= scale;
        patch.samples[3] *= scale;
        patch.samples[0] += move;
        patch.samples[1] += move;
        patch.samples[2] += move;
        patch.samples[3] += move;
    }
    height *= scale;
    radius *= scale;
    center *= scale;
    center += move;
}

void CustomizedModel::build() {
    BVH tree(patches, patch_num, 3);
    GLsizei bvh_size;
    bvh = tree.getLinearBVH(bvh_size);

    glBindBuffer(GL_TEXTURE_BUFFER, patch_tbo);
    glBufferData(GL_TEXTURE_BUFFER, (GLsizei)(sizeof(Patch) * patch_num), patches.data(), GL_STATIC_DRAW);
    glBindTexture(GL_TEXTURE_BUFFER, patch_tex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, patch_tbo);

    glBindBuffer(GL_TEXTURE_BUFFER, bvh_tbo);
    glBufferData(GL_TEXTURE_BUFFER, bvh_size, bvh, GL_STATIC_DRAW);
    glBindTexture(GL_TEXTURE_BUFFER, bvh_tex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, bvh_tbo);
}

QuadModel::QuadModel(Vector3f v1, Vector3f v2, Vector3f v3, Material *mat, Texture *tex): Model(mat, tex) {
    samples[0] = v1;
    samples[1] = v2;
    samples[2] = v3;
    samples[3] = v3 + v2 - v1;

    normal = (v2 - v1) & (v3 - v1);
    normal = normalize(normal);
}

MODEL_TYPE QuadModel::type() {
    return QUAD;
}

GLfloat QuadModel::hit(Ray) {
    return false;
}

Vector3f *QuadModel::getSamples() {
    return samples;
}

Vector3f QuadModel::getNormal() {
    return normal;
}

SphereModel::SphereModel(Vector3f c, GLfloat r, Material *mat, Texture *tex): Model(mat, tex) {
    center = c;
    radius = r;
}

MODEL_TYPE SphereModel::type() {
    return SPHERE;
}

GLfloat SphereModel::hit(Ray r) {
    GLfloat t = (center - r.startPoint) * r.direction;
    Vector3f T = r.startPoint + r.direction * t;
    Vector3f CP = T - center;
    GLfloat l_CP = length(CP);
    if (l_CP <= radius) return t;
    return -1.0f;
}

Vector3f SphereModel::getCenter() {
    return center;
}

GLfloat SphereModel::getRadius() {
    return radius;
}

CylinderModel::CylinderModel(Vector3f c, GLfloat r, GLfloat h, Material *mat, Texture *tex): Model(mat, tex) {
    center = c;
    radius = r;
    height = h;
}

MODEL_TYPE CylinderModel::type() {
    return CYLINDER;
}

GLfloat CylinderModel::hit(Ray r) {
    GLfloat t = (center.z - r.startPoint.z) / r.direction.z;
    Vector3f P = r.startPoint + r.direction * t;
    if (P.x >= center.x - radius && P.x <= center.x + radius &&
        P.y >= center.y && P.y <= center.y + height) return t;
    return -1.0f;
}

Vector3f CylinderModel::getCenter() {
    return center;
}

GLfloat CylinderModel::getRadius() {
    return radius;
}

GLfloat CylinderModel::getHeight() {
    return height;
}
