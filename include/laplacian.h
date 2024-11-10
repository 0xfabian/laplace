#pragma once

#include <mesh.h>
#include <Eigen/Sparse>

struct Laplacian
{
    size_t size;
    std::vector<float> M;
    Eigen::SparseMatrix<float> C;

    Laplacian() {}
    Laplacian(const Mesh& mesh);

    float operator()(int i, int j) const
    {
        return C.coeff(i, j) / M[i];
    }
};