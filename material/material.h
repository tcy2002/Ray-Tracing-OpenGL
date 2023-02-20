#pragma once

#include <GL/glew.h>

#include "config/config.h"

// 表面材质定义
class Material {
public:
    //0 <= specularRate + refractRate <= 1
    bool lighting = false;                         //是否是光源
    Vector3f color = {0.0f, 0.0f, 0.0f}; //颜色
    GLfloat specularRate = 0.0f;                   //镜面反射概率：0-1
    GLfloat specularTint = 0.0f;                   //镜面反射属性（颜色到光照的插值）：0-1
    GLfloat specularRoughness = 0.0f;              //镜面反射模糊度：0-1
    GLfloat refractRate = 0.0f;                    //折射概率：0-1
    GLfloat refractTint = 0.0f;                    //折射属性（颜色到光照的插值）：0-1
    GLfloat refractIndex = 1.0f;                   //折射率
    GLfloat refractRoughness = 0.0f;               //折射模糊度：0-1

    static const Material metal;
    static const Material plastic;
    static const Material glass;
    static const Material smoothChina;
    static const Material smoothWood;
    static const Material wall;
};
