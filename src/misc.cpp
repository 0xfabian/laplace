#include <misc.h>
#include <Windows.h>
#include <shobjidl.h> 

char* read_from_file(const char* path)
{
    FILE* f = fopen(path, "rb");

    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = (char*)malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = 0;

    fclose(f);

    return data;
}

void crash(bool condition, const char* msg, int code)
{
    if (condition)
    {
        printf("%s\n", msg);
        exit(code);
    }
}

glm::vec3 hsv_to_rgb(float h, float s, float v)
{
    float c = v * s;
    float x = c * (1.0f - std::fabs(fmod(h / 60.0f, 2) - 1.0f));
    float m = v - c;

    glm::vec3 rgb;

    if (0 <= h && h < 60)
    {
        rgb = glm::vec3(c, x, 0);
    }
    else if (60 <= h && h < 120)
    {
        rgb = glm::vec3(x, c, 0);
    }
    else if (120 <= h && h < 180)
    {
        rgb = glm::vec3(0, c, x);
    }
    else if (180 <= h && h < 240)
    {
        rgb = glm::vec3(0, x, c);
    }
    else if (240 <= h && h < 300)
    {
        rgb = glm::vec3(x, 0, c);
    }
    else
    {
        rgb = glm::vec3(c, 0, x);
    }

    return rgb + glm::vec3(m);
}

glm::vec3 hue_from_angle(float angle)
{
    if (angle < 0)
        angle += 2 * M_PI;

    float hue = angle * 180 / M_PI;
    float saturation = 1.0f;
    float value = 1.0f;

    return hsv_to_rgb(hue, saturation, value);
}

float randf(float min, float max)
{
    return (max - min) * (rand() / (float)RAND_MAX) + min;
}

bool open_file_dialog(std::string& file)
{
    HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (FAILED(f_SysHr))
        return false;

    IFileOpenDialog* f_FileSystem;

    f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));

    if (FAILED(f_SysHr))
    {
        CoUninitialize();

        return false;
    }

    f_SysHr = f_FileSystem->Show(NULL);

    if (FAILED(f_SysHr))
    {
        f_FileSystem->Release();
        CoUninitialize();

        return false;
    }

    IShellItem* f_Files;

    f_SysHr = f_FileSystem->GetResult(&f_Files);

    if (FAILED(f_SysHr))
    {
        f_FileSystem->Release();
        CoUninitialize();

        return false;
    }

    PWSTR f_Path;

    f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);

    if (FAILED(f_SysHr))
    {
        f_Files->Release();
        f_FileSystem->Release();
        CoUninitialize();

        return false;
    }

    std::wstring wpath(f_Path);
    std::string path(wpath.begin(), wpath.end());

    file = path;

    CoTaskMemFree(f_Path);
    f_Files->Release();
    f_FileSystem->Release();
    CoUninitialize();

    return true;
}