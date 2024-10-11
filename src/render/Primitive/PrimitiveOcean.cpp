#include "PrimitiveOcean.hpp"
#include "Primitive.hpp"
#include "render/Render.hpp"
#include "render/Shader/Shader.hpp"

#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"

using namespace std;

void PrimitiveOcean::Draw()
{
    ComputePass();
    RenderPass();
}

void PrimitiveOcean::ComputePass()
{
    // ampl
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_ampl_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer_normal_x_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer_normal_z_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffer_dx_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buffer_dz_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, buffer_h_tlide_0_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, buffer_h_tlide_0_conj_);

        shader_ampl_->use();
        static float t = 0.f;
        t += 0.016f;
        shader_ampl_->setFloat("t", t);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // ifft2
    {
        shader_ifft2_->use();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_ampl_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_normal_x_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_normal_z_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_dx_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_dz_);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // sign
    {
        shader_sign_->use();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_ampl_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_normal_x_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_normal_z_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_dx_);
        glDispatchCompute(1, 1, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_dz_);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void PrimitiveOcean::RenderPass()
{
    shader_->use();
    shader_->setMat4("model", transform_ * mesh_->GetModel());

    auto &textures = mesh_->GetTextures();
    for (size_t i = 0; i < textures.size(); ++i)
    {
        glBindTextureUnit(i, textures[i].id);
    }

    if (material_ != nullptr)
    {
        material_->Use(shader_);
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_ampl_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer_normal_x_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer_normal_z_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffer_dx_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buffer_dz_);

    glBindVertexArray(mesh_->GetVAO());
    glDrawElements(GL_TRIANGLES, mesh_->GetIndicesCount(), GL_UNSIGNED_INT, 0);
}

void PrimitiveOcean::Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
                          const std::shared_ptr<Shader> &shader_ampl, const std::shared_ptr<Shader> &shader_ifft2,
                          const std::shared_ptr<Shader> &shader_sign, uint32_t n, uint32_t m, float lx, float lz,
                          const std::vector<std::complex<float>> &h_tlide_0,
                          const std::vector<std::complex<float>> &h_tlide_0_conj)
{
    Primitive::Init(mesh, shader);
    shader_ampl_ = shader_ampl;
    shader_ifft2_ = shader_ifft2;
    shader_sign_ = shader_sign;

    shader_->use();
    shader_->setFloat("lambda", 1.0f);

    size_t length = n * m * 2 * 4;

    glCreateBuffers(1, &buffer_ampl_);
    glNamedBufferStorage(buffer_ampl_, length, 0, 0);

    glCreateBuffers(1, &buffer_normal_x_);
    glNamedBufferStorage(buffer_normal_x_, length, 0, 0);

    glCreateBuffers(1, &buffer_normal_z_);
    glNamedBufferStorage(buffer_normal_z_, length, 0, 0);

    glCreateBuffers(1, &buffer_dx_);
    glNamedBufferStorage(buffer_dx_, length, 0, 0);

    glCreateBuffers(1, &buffer_dz_);
    glNamedBufferStorage(buffer_dz_, length, 0, 0);

    glCreateBuffers(1, &buffer_h_tlide_0_);
    glNamedBufferStorage(buffer_h_tlide_0_, length, h_tlide_0.data(), 0);

    glCreateBuffers(1, &buffer_h_tlide_0_conj_);
    glNamedBufferStorage(buffer_h_tlide_0_conj_, length, h_tlide_0_conj.data(), 0);

    shader_ampl_->use();
    shader_ampl_->setInt("N", n);
    shader_ampl_->setInt("M", m);
    shader_ampl_->setFloat("Lx", lx);
    shader_ampl_->setFloat("Lz", lz);

    shader_ifft2_->use();
    shader_ifft2_->setInt("N", n);

    shader_sign_->use();
    shader_sign_->setInt("N", n);
}