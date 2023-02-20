#include "config.h"

#include <cmath>

GLfloat degToRad(GLfloat deg) {
    return PI * deg / 180.0f;
}

GLfloat radToDeg(GLfloat rad) {
    return rad / PI * 180.0f;
}

Vector3f normalize(const Vector3f &src) {
    float len = std::sqrt(src.x * src.x + src.y * src.y + src.z * src.z);
    return {src.x / len, src.y / len, src.z / len};
}

Vector2f normalize(const Vector2f &src) {
    float len = std::sqrt(src.x * src.x + src.y * src.y);
    return {src.x / len, src.y / len};
}

GLfloat length(const Vector3f &src) {
    return std::sqrt(src.x * src.x + src.y * src.y + src.z * src.z);
}

GLfloat length(const Vector2f &src) {
    return std::sqrt(src.x * src.x + src.y * src.y);
}

Vector2f operator+(const Vector2f &a, const Vector2f &b) {
    return {a.x + b.x, a.y + b.y};
}

Vector3f operator+(const Vector3f &a, const Vector3f &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

void operator+=(Vector2f &dst, const Vector2f &src) {
    dst.x += src.x;
    dst.y += src.y;
}

void operator+=(Vector3f &dst, const Vector3f &src) {
    dst.x += src.x;
    dst.y += src.y;
    dst.z += src.z;
}

Vector2f operator-(const Vector2f &a, const Vector2f &b) {
    return {a.x - b.x, a.y - b.y};
}

Vector3f operator-(const Vector3f &a, const Vector3f &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

void operator-=(Vector2f &dst, const Vector2f &src) {
    dst.x -= src.x;
    dst.y -= src.y;
}

void operator-=(Vector3f &dst, const Vector3f &src) {
    dst.x -= src.x;
    dst.y -= src.y;
    dst.z -= src.z;
}

Vector2f operator*(const Vector2f &a, GLfloat b) {
    return {a.x * b, a.y * b};
}

Vector3f operator*(const Vector3f &a, GLfloat b) {
    return {a.x * b, a.y * b, a.z * b};
}

void operator*=(Vector2f &dst, GLfloat src) {
    dst.x *= src;
    dst.y *= src;
}

void operator*=(Vector3f &dst, GLfloat src) {
    dst.x *= src;
    dst.y *= src;
    dst.z *= src;
}

bool operator==(const Vector2f &a, const Vector2f &b) {
    return (a.x == b.x && a.y == b.y);
}

bool operator==(const Vector3f &a, const Vector3f &b) {
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

GLfloat operator*(const Vector3f &a, const Vector3f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3f operator&(const Vector3f &src, const Vector3f &dst) {
    return {src.y * dst.z - src.z * dst.y,
            src.z * dst.x - src.x * dst.z,
            src.x * dst.y - src.y * dst.x};
}

std::ostream &operator<<(std::ostream &out, const Vector3f &src) {
    out << src.x << ", " << src.y << ", " << src.z;
    return out;
}

Matrix4f operator*(Matrix4f &a, Matrix4f &b) {
    Matrix4f ret{};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ret[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
    return ret;
}

std::ostream &operator<<(std::ostream &out, Matrix4f &mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            out << mat[i][j] << " ";
        out << std::endl;
    }
    return out;
}
