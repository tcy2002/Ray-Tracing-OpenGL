#pragma once

#include <iostream>
#include "GL/glew.h"

#include "config/config.h"

class Shader {
private:
    int program_id;

    static int createShader(const char *buf, int type);
    static void compileShaderAndCheck(int shader);
    static void linkProgramAndCheck(int program);

public:
    Shader(const char *vertPath, const char *fragPath,
           const char *tcsPath = nullptr, const char *tesPath = nullptr, const char *gsPath = nullptr);
    ~Shader();

    void use() const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, GLint value) const;
    void setUint(const std::string &name, GLuint value) const;
    void setUint(const std::string &name, GLuint *value, GLsizei n) const;
    void setFloat(const std::string &name, GLfloat value) const;
    void setVec2(const std::string &name, Vector2f value) const;
    void setVec2(const std::string &name, Vector2f *value, GLsizei n) const;
    void setVec3(const std::string &name, Vector3f value) const;
    void setVec3(const std::string &name, Vector3f *value, GLsizei n) const;
    void setVec4(const std::string &name, Vector4f value) const;
    void setVec4(const std::string &name, Vector4f *value, GLsizei n) const;
    void setMat4(const std::string &name, Matrix4f value) const;
};