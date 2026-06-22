#pragma once
#define NOMINMAX
#include <string>
#include <windows.h>
#include <algorithm>

inline std::string getExeDir()
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exePath(path);
    return exePath.substr(0, exePath.find_last_of("\\/")) + "\\";
}

inline std::string xorEncryptDecrypt(const std::string& data)
{
    const std::string KEY = "d1rtych34t3r";

    std::string result = data;

    for (size_t i = 0; i < data.size(); i++)
        result[i] = data[i] ^ KEY[i % KEY.size()];

    return result;
}