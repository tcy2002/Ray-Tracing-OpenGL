/********************************************
 * 此文件定义了一些需要用到的常数、类型与工具函数
 *******************************************/

#pragma once

#include <iostream>
#include <cmath>
#include "GL/glew.h"

#define PI 3.141593f

#define ORI_WIDTH 500
#define ORI_HEIGHT 500

typedef struct vector2f {
    GLfloat x, y;
} Vector2f;

typedef struct vector3f {
    GLfloat x, y, z;
} Vector3f;

typedef struct vector4f {
    GLfloat x, y, z, a;
} Vector4f;

typedef struct matrix4f {
    GLfloat mat[16];
    GLfloat *operator[](int r) {
        return &mat[r * 4];
    }
} Matrix4f;

typedef struct patch {
    Vector3f samples[4];
    Vector3f normal;
} Patch;

typedef struct ray {
    Vector3f direction;
    vector3f startPoint;
} Ray;

//角度/弧度转换
GLfloat degToRad(GLfloat deg);
GLfloat radToDeg(GLfloat rad);

//归一化
Vector3f normalize(const Vector3f &src);
Vector2f normalize(const Vector2f &src);

//求长度
GLfloat length(const Vector3f &src);
GLfloat length(const Vector2f &src);

//加
Vector2f operator+(const Vector2f &a, const Vector2f &b);
Vector3f operator+(const Vector3f &a, const Vector3f &b);
void operator+=(Vector2f &dst, const Vector2f &src);
void operator+=(Vector3f &dst, const Vector3f &src);

//减
Vector2f operator-(const Vector2f &a, const Vector2f &b);
Vector3f operator-(const Vector3f &a, const Vector3f &b);
void operator-=(Vector2f &dst, const Vector2f &src);
void operator-=(Vector3f &dst, const Vector3f &src);

//乘
Vector2f operator*(const Vector2f &a, GLfloat b);
Vector3f operator*(const Vector3f &a, GLfloat b);
void operator*=(Vector2f &dst, GLfloat src);
void operator*=(Vector3f &dst, GLfloat src);

//相等
bool operator==(const Vector2f &a, const Vector2f &b);
bool operator==(const Vector3f &a, const Vector3f &b);

//点积
GLfloat operator*(const Vector3f &a, const Vector3f &b);
//叉积
Vector3f operator&(const Vector3f &src, const Vector3f &dst);

//三维矩阵输出
std::ostream &operator<<(std::ostream &out, const Vector3f &src);

//四维矩阵乘法
Matrix4f operator*(Matrix4f &a, Matrix4f &b);
//四维矩阵输出
std::ostream &operator<<(std::ostream &out, Matrix4f &mat);
