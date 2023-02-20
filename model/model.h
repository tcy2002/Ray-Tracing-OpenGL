#pragma once

#include <GL/glew.h>
#include <vector>

#include "config/config.h"
#include "material/material.h"
#include "texture/texture.h"
#include "bvh/bvh.h"

//模型类别：自定义类型（扫描表面）、四边形、球体、圆柱体
enum MODEL_TYPE {CUSTOMIZED, QUAD, SPHERE, CYLINDER};

//模型基类
class Model {
protected:
    int id;
    Material *material{};
    Texture *texture{};

public:
    explicit Model(Material *mat, Texture *tex = nullptr);
    ~Model();

    Material *getMaterial() {return material;}
    Texture *getTexture() {return texture;}
    int getId() const {return id;}
    void setLighting() {material->lighting = !material->lighting;}

    virtual MODEL_TYPE type() = 0;
    virtual GLfloat hit(Ray r) = 0;

    virtual Vector3f *getSamples() {return nullptr;}
    virtual GLuint getPatchTex() {return 0;}
    virtual GLuint getBVHTex() {return 0;}
    virtual Vector3f getCenter() {return {0.0f, 0.0f, 0.0f};}
    virtual GLfloat getRadius() {return 0.0f;}
    virtual GLfloat getHeight() {return 0.0f;}
    virtual Vector3f getNormal() {return {0.0f, 0.0f, 0.0f};}
    virtual void trans(GLfloat scale, Vector3f move) {}
    virtual void build() {}
};

class CustomizedModel : public Model {
private:
    std::vector<Patch> patches{};
    GLuint *bvh{};

    Vector3f center{};
    GLfloat radius{};
    GLfloat height{};

    GLuint patch_tbo{};
    GLuint patch_tex{};
    GLsizei patch_num{};
    GLuint bvh_tbo{};
    GLuint bvh_tex{};

public:
    explicit CustomizedModel(const std::string &path, const Vector3f &eye, Material *mat, Texture *tex = nullptr);
    ~CustomizedModel();

    MODEL_TYPE type() override;
    GLfloat hit(Ray r) override;

    GLuint getPatchTex() override;
    GLuint getBVHTex() override;
    Vector3f getCenter() override;
    GLfloat getHeight() override;
    void trans(GLfloat scale, Vector3f move) override;
    void build() override;
};

class QuadModel : public Model {
private:
    Vector3f samples[4]{};
    vector3f normal{};

public:
    QuadModel(Vector3f left_up, Vector3f left_down, Vector3f right_up, Material *mat, Texture *tex = nullptr);

    MODEL_TYPE type() override;
    GLfloat hit(Ray r) override;

    Vector3f *getSamples() override;
    Vector3f getNormal() override;
};

class SphereModel : public Model {
private:
    Vector3f center{};
    GLfloat radius{};

public:
    SphereModel(Vector3f center, GLfloat radius, Material *mat, Texture *tex = nullptr);

    MODEL_TYPE type() override;
    GLfloat hit(Ray r) override;

    Vector3f getCenter() override;
    GLfloat getRadius() override;
};

class CylinderModel : public Model {
private:
    Vector3f center{};
    GLfloat radius{};
    GLfloat height{};

public:
    CylinderModel(Vector3f bottom_center, GLfloat radius, GLfloat height, Material *mat, Texture *tex = nullptr);

    MODEL_TYPE type() override;
    GLfloat hit(Ray r) override;

    Vector3f getCenter() override;
    GLfloat getRadius() override;
    GLfloat getHeight() override;
};
