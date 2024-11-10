#pragma once

#include <core.h>
#include <mesh.h>
#include <algorithm>

#include <Eigen/Eigenvalues>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

#include <laplacian.h>

enum Mode
{
    SPECTRUM,
    MEAN_CURV,
    HEAT_EQ,
    WAVE_EQ,
};

struct App
{
    Mode mode = Mode::SPECTRUM;

    Shader shader;
    Mesh* mesh;

    Laplacian const_lap;

    std::vector<glm::vec3> vertices_copy;
    std::vector<glm::vec3> normals_copy;

    Eigen::VectorXf eigenvalues;
    Eigen::MatrixXf eigenvectors;

    bool spectrum_computed = false;
    int state = 0;
    float e_scale = 1.0f;
    float amplitude = 1.0f;
    float intensity = 1.0f;
    float frequency = 1.0f;

    float dt_pow = -4;
    float time_step = 0.0001f;
    float damp = 0.0f;

    bool draw_triangles = false;
    bool draw_normals = false;
    float normal_size = 0.1f;

    bool auto_update = false;

    float brush_value = 1.0f;
    int brush_smooth = 100;

    void init(const char* name);
    void open_mesh(const char* name);
    void compute_spectrum();
    void bump(float sign);
    void update_mesh(float dt);
    void reset_mesh();
    void update(float dt);
    void draw();
    void clean();
};