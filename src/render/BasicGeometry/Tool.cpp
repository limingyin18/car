    // make sure every face vertex order, normal point to out side
    // for (size_t i = 0; i < indices.size(); i += 3)
    // {

    //     auto v1 = vertices[indices[i]].position_;
    //     auto v2 = vertices[indices[i + 1]].position_;
    //     auto v3 = vertices[indices[i + 2]].position_;
    //     float volume_tetrahedron =
    //         glm::dot(v1, glm::cross(v2, v3)); // Needs to be divided by 6, postpone this until the end of the loop
    //     if (volume_tetrahedron < 0.f)
    //     {
    //         std::swap(indices[i + 1], indices[i + 2]);
    //     }
    // }