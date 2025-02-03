#pragma once

#include <filesystem>

class Filesystem {
public:
    static void* loadResource(const char* path, unsigned int& size);
    static void* loadFile(const std::filesystem::path& path, unsigned int& size);
};
