#include "Tool.hpp"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "tools/stb_image.h"

#include <spdlog/spdlog.h>

int Tool::LoadTexture(std::string path)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    uint32_t mode = nrChannels == 3 ? GL_RGB : GL_RGBA;

    uint32_t texture_id;
    if (data)
    {
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
    else
    {
        spdlog::error("Failed to load texture " + path);
        throw std::runtime_error("Failed to load texture " + path);
    }

    return texture_id;
}