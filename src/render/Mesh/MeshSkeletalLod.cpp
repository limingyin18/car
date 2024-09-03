#include "MeshSkeletalLod.hpp"
#include "Vertex.hpp"
#include <meshoptimizer.h>

#include <spdlog/spdlog.h>

void MeshSkeletalLod::Init(const std::vector<VertexSkeletal> &_vertices, const std::vector<uint32_t> &_indices,
                           const std::vector<Texture> &_textures)
{
    vertices_ = _vertices;
    indices_ = _indices;
    textures_ = _textures;

    GenerateLOD();
    SetupBuffer();
    SetupVAO();
}
void MeshSkeletalLod::GenerateLOD()
{

    size_t index_count = indices_.size();
    size_t vertex_count = vertices_.size();

    std::vector<unsigned int> remap(index_count); // allocate temporary memory for the remap table
    size_t remap_vertex_count = meshopt_generateVertexRemap(&remap[0], indices_.data(), index_count, &vertices_[0],
                                                            vertex_count, sizeof(VertexSkeletal));
    std::vector<unsigned int> indices_new(index_count);
    meshopt_remapIndexBuffer(indices_new.data(), indices_.data(), index_count, &remap[0]);
    std::vector<VertexSkeletal> vertices_new(remap_vertex_count);
    meshopt_remapVertexBuffer(vertices_new.data(), &vertices_[0], vertex_count, sizeof(VertexSkeletal), &remap[0]);

    vertex_count = remap_vertex_count;
    vertices_ = vertices_new;
    indices_ = indices_new;

    meshopt_optimizeVertexCache(indices_.data(), indices_.data(), index_count, vertex_count);
    meshopt_optimizeOverdraw(indices_.data(), indices_.data(), index_count, &vertices_[0].position_.x, vertex_count,
                             sizeof(VertexSkeletal), 1.05f);
    meshopt_optimizeVertexFetch(vertices_.data(), indices_.data(), index_count, vertices_.data(), vertex_count,
                                sizeof(VertexSkeletal));

    lods_.push_back(indices_.size());

    for (int i = 0; i < 2; i++)
    {
        float threshold = 1.0f - 0.3f * (i + 1);
        size_t target_index_count = size_t(index_count * threshold);
        float target_error = 1.;
        unsigned int options = 0; // meshopt_SimplifyX flags, 0 is a safe default
        // options = meshopt_SimplifyErrorAbsolute;
        float lod_error = 0.f;
        std::vector<unsigned int> lod(index_count);

        lod.resize(meshopt_simplify(&lod[0], indices_.data(), index_count, &vertices_[0].position_.x, vertices_.size(),
                                    sizeof(VertexSkeletal), target_index_count, target_error, options, &lod_error));

        // lod.resize(meshopt_simplifySloppy(&lod[0], indices_.data(), index_count, &vertices_[0].position_.x,
        // vertices_.size(),
        //                             sizeof(VertexSkeletal), target_index_count, target_error, &lod_error));

        lods_.push_back(lod.size());
        spdlog::debug("target_index_count: {}", target_index_count);
        spdlog::debug("target_error: {}", target_error);
        spdlog::debug("threshold: {}", threshold);
        spdlog::debug("LOD {} error: {}", i, lod_error);
        spdlog::debug("LOD {} index count: {}", i, lod.size());
        indices_.insert(indices_.end(), lod.begin(), lod.end());
    }

    {
        // float threshold = 0.1f;
        // size_t target_index_count = size_t(index_count * threshold);
        // float target_error = 0.99f;
        // unsigned int options = 0; // meshopt_SimplifyX flags, 0 is a safe default
        // options = meshopt_SimplifyErrorAbsolute;
        // float lod_error = 0.f;
        // std::vector<unsigned int> lod(index_count);

        // lod.resize(meshopt_simplifySloppy(&lod[0], indices_.data(), index_count, &vertices_[0].position_.x,
        //                                   vertices_.size(), sizeof(VertexSkeletal), target_index_count, target_error,
        //                                   &lod_error));

        // lods_.push_back(lod.size());
        // indices_.insert(indices_.end(), lod.begin(), lod.end());
    }

    // vertex_count_ = vertices_.size();
}