#include "Tool.hpp"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "tools/stb_image.h"

#include <spdlog/spdlog.h>

uint32_t Tool::LoadTexture(const std::string &path)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    uint32_t mode_sized = -1;
    uint32_t mode = -1;
    switch (nrChannels)
    {
    case 1:
        mode_sized = GL_R8;
        mode = GL_RED;
        break;
    case 2:
        mode_sized = GL_RG8;
        mode = GL_RG;
        break;
    case 3:
        mode_sized = GL_RGB8;
        mode = GL_RGB;
        break;
    case 4:
        mode_sized = GL_RGBA8;
        mode = GL_RGBA;
        break;
    default:
        mode_sized = GL_RGB8;
        mode = GL_RGB;
    }

    uint32_t texture_id = -1;
    if (data)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
        glTextureStorage2D(texture_id, 1, mode_sized, width, height);
        glTextureSubImage2D(texture_id, 0, 0, 0, width, height, mode, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(texture_id);

        stbi_image_free(data);
    }
    else
    {
        spdlog::error("Failed to load texture " + path);
        throw std::runtime_error("Failed to load texture " + path);
    }

    return texture_id;
}

uint32_t Tool::LoadTextureHDR(const std::string &path)
{
    // stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &hdrTexture);
        glTextureStorage2D(hdrTexture, 1, GL_RGB32F, width, height);
        glTextureSubImage2D(hdrTexture, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
        glGenerateTextureMipmap(hdrTexture);

        stbi_image_free(data);
    }
    else
    {
        spdlog::error("Failed to load HDR image {}", path);
        throw std::runtime_error("Failed to load texture " + path);
    }
    return hdrTexture;
}

void Tool::PrintGLM(const std::string &name, const glm::mat4 &mat)
{
    spdlog::info(name);
    for (int i = 0; i < 4; i++)
    {
        spdlog::info("{:f} {:f} {:f} {:f}", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
    }
}