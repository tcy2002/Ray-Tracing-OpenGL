#include "texture.h"

Texture::Texture(const char *buf) {
    FileLoader file(buf);
    BmpLoader bl(file.buf);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, bl.format, bl.width, bl.height,
                 0, bl.format, GL_UNSIGNED_BYTE, bl.textureData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture_id);
}

void Texture::bind(int texture) const {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}
