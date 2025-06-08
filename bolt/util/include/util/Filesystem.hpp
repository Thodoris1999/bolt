#pragma once

#include <filesystem>

class Filesystem {
public:
    static void* loadFile(const std::filesystem::path& path, unsigned int& size);
};
