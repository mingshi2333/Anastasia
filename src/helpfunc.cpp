#include "helpfunc.h"

std::string helpfunc::getExecutablePath()
{
    char result[PATH_MAX];
    ssize_t count       = readlink("/proc/self/exe", result, PATH_MAX);
    std::string exePath = std::string(result, (count > 0) ? count : 0);
    return exePath.substr(0, exePath.find_last_of("/"));
}

std::vector<char> helpfunc::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}