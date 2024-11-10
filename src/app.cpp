#include <app.h>

using namespace std;
using namespace glm;

void App::init(const char* name)
{
    shader = Shader("solid");
    mesh = nullptr;

    glClearColor(0.8, 0.8, 0.8, 1);

    open_mesh(name);

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 4;
    style.FrameRounding = 2;
    style.FramePadding = { 6, 4 };
    style.ItemSpacing = { 8, 6 };
}

void App::open_mesh(const char* name)
{
    if (name == nullptr)
        return;

    if (mesh)
        delete mesh;

    mesh = new Mesh(name);

    vertices_copy.resize(mesh->vertices.size());
    normals_copy.resize(mesh->vertices.size());

    for (int i = 0; i < mesh->vertices.size(); i++)
    {
        vertices_copy[i] = mesh->vertices[i].pos;
        normals_copy[i] = mesh->vertices[i].normal;
    }

    spectrum_computed = false;

    if (mode == Mode::MEAN_CURV)
        update_mesh(0);

    const_lap = Laplacian(*mesh);
}

void App::compute_spectrum()
{
    if (spectrum_computed)
        return;

    Laplacian lap(*mesh);

    int k = 100;

    if (k > lap.size - 1)
        k = lap.size - 1;

    int M_size = lap.M.size();

    Eigen::SparseMatrix<float> M(M_size, M_size);

    M.reserve(M_size);

    for (int i = 0; i < M_size; i++)
        M.insert(i, i) = lap.M[i];

    M.makeCompressed();

    Spectra::SymShiftInvert<float, Eigen::Sparse, Eigen::Sparse> op(lap.C, M);
    Spectra::SparseSymMatProd<float> bop(M);

    Spectra::SymGEigsShiftSolver<Spectra::SymShiftInvert<float, Eigen::Sparse, Eigen::Sparse>, Spectra::SparseSymMatProd<float>, Spectra::GEigsMode::ShiftInvert> solver(op, bop, k, k + 1, 0.0);

    solver.init();
    int nconv = solver.compute(Spectra::SortRule::LargestMagn);

    Eigen::VectorXf unsorted_eigenvalues = solver.eigenvalues();
    Eigen::MatrixXf unsorted_eigenvectors = solver.eigenvectors();

    vector<int> indices(unsorted_eigenvalues.size());
    for (int i = 0; i < indices.size(); i++)
        indices[i] = i;

    sort(indices.begin(), indices.end(), [&](int i1, int i2)
        {
            return unsorted_eigenvalues[i1] < unsorted_eigenvalues[i2];
        });

    eigenvalues = Eigen::VectorXf(unsorted_eigenvalues.size());
    eigenvectors = Eigen::MatrixXf(unsorted_eigenvectors.rows(), unsorted_eigenvectors.cols());

    for (int i = 0; i < indices.size(); i++)
    {
        eigenvalues[i] = unsorted_eigenvalues[indices[i]];
        eigenvectors.col(i) = unsorted_eigenvectors.col(indices[i]);
    }

    state = 0;
    e_scale = 1;

    for (int i = 0; i < eigenvalues.size(); i++)
    {
        if (eigenvalues[i] > 0.0001)
        {
            e_scale /= eigenvalues[i];
            break;
        }
    }

    spectrum_computed = true;
}

void App::bump(float sign)
{
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    float x = (2.0f * mx / window.width) - 1.0f;
    float y = 1.0f - (2.0f * my / window.height);

    vec4 near_hom = inverse(cam.matrix(vec3(0))) * vec4(x, y, -1.0f, 1.0f);
    vec3 near = vec3(near_hom) / near_hom.w;

    vec4 far_hom = inverse(cam.matrix(vec3(0))) * vec4(x, y, 1.0f, 1.0f);
    vec3 far = vec3(far_hom) / far_hom.w;;

    vec3 origin = near;
    vec3 dir = normalize(far - near);

    int i = mesh->intersect(origin, dir);

    if (i == -1)
        return;

    Eigen::VectorXf u = Eigen::VectorXf::Zero(const_lap.size);

    u[mesh->indices[i]] = 1;

    for (int t = 0; t < brush_smooth; t++)
    {
        Eigen::VectorXf C_u = const_lap.C * u;

        for (int i = 0; i < const_lap.size; i++)
            u[i] -= 0.0001f * (C_u[i] / const_lap.M[i]);
    }

    for (int j = 0; j < mesh->vertices.size(); j++)
        mesh->vertices[j].color += sign * brush_value * vec3(u[j], u[j], 0);

    update_mesh(0);
}

void App::update_mesh(float dt)
{
    if (mode == Mode::SPECTRUM)
    {
        if (!spectrum_computed)
            return;

        for (int i = 0; i < mesh->vertices.size(); i++)
        {
            float e = eigenvectors.col(state)[i];
            mesh->vertices[i].color = vec3(e);
            mesh->vertices[i].pos = vertices_copy[i] + normals_copy[i] * (float)(amplitude * e * sin(frequency * e_scale * eigenvalues[state] * total_time));
        }
    }
    else if (mode == Mode::MEAN_CURV)
    {
        Laplacian lap(*mesh);
        size_t n = lap.size;

        Eigen::VectorXf x(n), y(n), z(n);

        for (int i = 0; i < n; i++)
        {
            x[i] = mesh->vertices[i].pos.x;
            y[i] = mesh->vertices[i].pos.y;
            z[i] = mesh->vertices[i].pos.z;
        }

        Eigen::VectorXf res_x = lap.C * x;
        Eigen::VectorXf res_y = lap.C * y;
        Eigen::VectorXf res_z = lap.C * z;

        for (int i = 0; i < n; i++)
        {
            vec3 dir = vec3(res_x[i], res_y[i], res_z[i]) / lap.M[i];

            float H = glm::dot(dir, mesh->vertices[i].normal) / 2.0;

            mesh->vertices[i].color = vec3(H);
            mesh->vertices[i].pos -= dt * H * mesh->vertices[i].normal;
        }
    }
    else if (mode == Mode::HEAT_EQ)
    {
        Eigen::VectorXf u(const_lap.size);

        for (int i = 0; i < const_lap.size; i++)
            u[i] = mesh->vertices[i].color.x;

        Eigen::VectorXf C_u = const_lap.C * u;

        for (int i = 0; i < const_lap.size; i++)
            mesh->vertices[i].color.x -= dt * C_u[i] / const_lap.M[i];
    }
    else if (mode == Mode::WAVE_EQ)
    {
        Eigen::VectorXf u(const_lap.size);

        for (int i = 0; i < const_lap.size; i++)
            u[i] = mesh->vertices[i].color.x;

        Eigen::VectorXf C_u = const_lap.C * u;

        for (int i = 0; i < const_lap.size; i++)
        {
            float lap_u = C_u[i] / const_lap.M[i];

            float now = mesh->vertices[i].color.x;
            float past = mesh->vertices[i].color.y;

            float future = 2 * now - past - dt * lap_u - damp * (now - past) * sqrt(dt);

            mesh->vertices[i].color.x = future;
            mesh->vertices[i].color.y = now;

            mesh->vertices[i].pos = vertices_copy[i] + amplitude * normals_copy[i] * mesh->vertices[i].color.x;
        }
    }

    mesh->recalculate_normals();
    mesh->rebuffer();
}

void App::reset_mesh()
{
    for (int i = 0; i < vertices_copy.size(); i++)
    {
        mesh->vertices[i].pos = vertices_copy[i];
        mesh->vertices[i].normal = normals_copy[i];
        mesh->vertices[i].color = vec3(0);
    }

    mesh->rebuffer();
}

void App::update(float dt)
{
    if (!mesh)
        return;

    if (mode == Mode::SPECTRUM)
        update_mesh(dt);
    else
    {
        if (auto_update || is_key_pressed(SDLK_e))
            update_mesh(time_step);

        if (is_key_pressed(SDLK_r))
        {
            reset_mesh();
            update_mesh(0);
        }

        if (mode != Mode::MEAN_CURV && !mouse_control)
        {
            if (is_button_pressed(SDL_BUTTON_LEFT))
                bump(1.0f);
            else if (is_button_pressed(SDL_BUTTON_RIGHT))
                bump(-1.0f);
        }
    }
}

void App::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid(vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), 1, 20, vec3(0.7));

    ImGui::Begin("Menu");

    if (ImGui::Button("Open mesh"))
    {
        string file;
        if (open_file_dialog(file))
            open_mesh(file.c_str());
    }

    if (mesh)
    {
        ImGui::Text("%lu vertices", mesh->vertices.size());
        ImGui::Text("%lu triangles", mesh->indices.size() / 3);

        ImGui::Checkbox("draw triangles", &draw_triangles);
        ImGui::Checkbox("draw normals", &draw_normals);

        if (draw_normals)
            ImGui::DragFloat("size", &normal_size, 0.01f, 0, 1);

        ImGui::Separator();

        static const char* modes = "Spectrum\0Mean curvature flow\0Heat equation\0Wave equation\0";

        if (ImGui::Combo("Mode", (int*)&mode, modes))
        {
            reset_mesh();
            update_mesh(0);
        }

        ImGui::Separator();

        if (mode == Mode::SPECTRUM)
        {
            if (ImGui::Button("Compute spectrum"))
                compute_spectrum();

            if (spectrum_computed)
            {
                ImGui::Text("eigenvalue %.2f", eigenvalues[state]);

                if (ImGui::InputInt("state", &state, 1, 10))
                    state = SDL_clamp(state, 0, eigenvalues.size() - 1);

                ImGui::DragFloat("intensity", &intensity, 0.1f, 0, 100);
                ImGui::DragFloat("frequency", &frequency, 0.1f, 0, 100);
                ImGui::DragFloat("amplitude", &amplitude, 0.1f, 0, 100);
            }
        }
        else
        {
            ImGui::DragFloat("intensity", &intensity, 0.1f, 0, 100);

            if (mode != Mode::MEAN_CURV)
                ImGui::DragFloat("amplitude", &amplitude, 0.1f, 0, 100);

            if (mode == Mode::WAVE_EQ)
                ImGui::DragFloat("damping", &damp, 0.1f, 0, 100);

            time_step = pow(10, dt_pow);
            string ts = to_string(time_step);
            ImGui::DragFloat("dt", &dt_pow, 0.01f, -6.0f, -3.0f, ts.c_str());

            if (ImGui::Button("Reset mesh"))
            {
                reset_mesh();
                update_mesh(0);
            }

            ImGui::Separator();
            ImGui::Checkbox("auto update", &auto_update);

            if (mode != Mode::MEAN_CURV)
            {
                ImGui::DragFloat("brush value", &brush_value, 0.1f, 0, 100);
                ImGui::DragInt("brush smooth", &brush_smooth, 1, 0, 200);
            }
        }
    }

    ImGui::End();

    if (mesh)
    {
        shader.bind();
        shader.upload_vec3("sun", cam.forward);
        shader.upload_vec3("eye", cam.forward);
        shader.upload_float("intensity", intensity);
        shader.upload_mat4("cam_mat", cam.matrix(vec3(0)));

        mesh->draw();

        if (draw_triangles)
            for (int i = 0; i < mesh->indices.size(); i += 3)
                triangle(mesh->vertices[mesh->indices[i]].pos, mesh->vertices[mesh->indices[i + 1]].pos, mesh->vertices[mesh->indices[i + 2]].pos, vec3(1));


        if (draw_normals)
            for (auto& vert : mesh->vertices)
                line(vert.pos, vert.pos + normal_size * vert.normal, vec3(0, 1, 1));
    }
}

void App::clean()
{
    delete mesh;

    shader.destroy();
}