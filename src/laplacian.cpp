#include <laplacian.h>

using namespace std;

Laplacian::Laplacian(const Mesh& mesh)
{
    size = mesh.vertices.size();

    M.resize(size, 0);
    C.resize(size, size);
    C.reserve(6 * size);

    std::vector<Eigen::Triplet<float>> triplets;

    for (int i = 0; i < mesh.indices.size(); i += 3)
    {
        int a = mesh.indices[i + 0];
        int b = mesh.indices[i + 1];
        int c = mesh.indices[i + 2];

        glm::vec3 ab = mesh.vertices[b].pos - mesh.vertices[a].pos;
        glm::vec3 ac = mesh.vertices[c].pos - mesh.vertices[a].pos;
        glm::vec3 bc = mesh.vertices[c].pos - mesh.vertices[b].pos;

        float ab2 = glm::dot(ab, ab);
        float ac2 = glm::dot(ac, ac);
        float bc2 = glm::dot(bc, bc);

        float two_area = glm::length(glm::cross(ab, ac));
        float area_3 = two_area / 6.0;
        float i_8_area = -1.0 / (4.0 * two_area);

        M[a] += area_3;
        M[b] += area_3;
        M[c] += area_3;

        float f_a = (ac2 + ab2 - bc2) * i_8_area;
        float f_b = (ab2 + bc2 - ac2) * i_8_area;
        float f_c = (ac2 + bc2 - ab2) * i_8_area;

        triplets.emplace_back(a, b, f_c);
        triplets.emplace_back(a, c, f_b);
        triplets.emplace_back(b, c, f_a);

        triplets.emplace_back(b, a, f_c);
        triplets.emplace_back(c, a, f_b);
        triplets.emplace_back(c, b, f_a);

        triplets.emplace_back(a, a, -f_b - f_c);
        triplets.emplace_back(b, b, -f_a - f_c);
        triplets.emplace_back(c, c, -f_a - f_b);
    }

    C.setFromTriplets(triplets.begin(), triplets.end());
    C.makeCompressed();
}