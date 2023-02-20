#pragma once

#include <vector>
#include <random>
#include <GL/glew.h>

#include "shader/shader.h"
#include "model/model.h"
#include "material/material.h"

#define MAX_FRAME 512
#define MAX_ITER 8

class Scene {
private:
    bool finished = false;
    DWORD start;

    int frame = 0;
    int iter = 0;
    int maxIter = 1;
    GLuint fbo[MAX_ITER]{};
    GLuint tbo[MAX_ITER]{};

    //着色器
    Shader *tracerShader; //光线追踪着色器
    Shader *renderShader; //绘制着色器

    //模型
    std::vector<Model *> models;

    //传递uniform变量的工具函数
    void setMaterial(const std::string &name, Material *material);
    void setTexture(const std::string &name, Texture *texture, int id);
    void setQuad(const std::string &name, Model *model);
    void setSphere(const std::string &name, Model *model);
    void setCylinder(const std::string &name, Model *model);
    void setCustomized(const std::string &name, Model *model);

public:
    Scene();
    void hitModel(GLfloat x, GLfloat y);
    void incIter();
    void render();

private:
    const Vector3f eyePos = {0.0f, 0.0f, 4.0f};
    const Vector3f screen[4] = {{-1.0f, 1.0f, 0.0f},
                                {-1.0f, -1.0f, 0.0f},
                                {1.0f, -1.0f, 0.0f},
                                {1.0f, 1.0f, 0.0f}};
    GLuint VAO{}, VBO{};
    const GLuint sobol[64] = {2147483648u, 1073741824u, 536870912u, 268435456u,
                              134217728u, 67108864u, 33554432u, 16777216u,
                              8388608u, 4194304u, 2097152u, 1048576u,
                              524288u, 262144u, 131072u, 65536u,
                              32768u, 16384u, 8192u, 4096u,
                              2048u, 1024u, 512u, 256u,
                              128u, 64u, 32u, 16u,
                              8u, 4u, 2u, 1u,
                              2147483648u, 3221225472u, 2684354560u, 4026531840u,
                              2281701376u, 3422552064u, 2852126720u, 4278190080u,
                              2155872256u, 3233808384u, 2694840320u, 4042260480u,
                              2290614272u, 3435921408u, 2863267840u, 4294901760u,
                              2147516416u, 3221274624u, 2684395520u, 4026593280u,
                              2281736192u, 3422604288u, 2852170240u, 4278255360u,
                              2155905152u, 3233857728u, 2694881440u, 4042322160u,
                              2290649224u, 3435973836u, 2863311530u, 4294967295u};
};