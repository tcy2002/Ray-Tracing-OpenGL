#pragma once

#include "GL/glew.h"

#include "loader/loader.h"

class Texture {
private:
    GLuint texture_id{};

public:
    explicit Texture(const char *buf);
    ~Texture();

    void bind(int texture) const;
};