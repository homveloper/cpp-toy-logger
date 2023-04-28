#include <iostream>
#include "logger.hpp"
#include <unistd.h>
#include <chrono>

int64_t StringLog1(const int size)
{
    auto start = std::chrono::system_clock::now();
    for(int i=0; i<size; i++)
    {
        std::wstring log = L"Hello, world!\n";
        std::wcout << log;
    }
    auto end = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int64_t StringLog2(const int size)
{
    auto start = std::chrono::system_clock::now();
    std::wstring log;
    for(int i=0; i<size; i++)
    {
        log += L"Hello, world!\n";
    }
    std::wcout << log;
    auto end = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int64_t StringLog3(const int size, const int batchSize)
{
    auto start = std::chrono::system_clock::now();
    std::wstring log;
    for(int i=0; i<size; i++)
    {
        log += L"Hello, world!\n";
        if(i % batchSize == 0)
        {
            std::wcout << log;
            log.clear();
        }
    }
    std::wcout << log;
    auto end = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main(int, char**) {
    CConsoleLogger logger(1);

    auto start = std::chrono::system_clock::now();
    for(int i=0; i<1000; i++)
    {
        logger.Log(ELogLevel::INFO, L"Hello, world!");
    }
    auto end = std::chrono::system_clock::now();

    logger.EndThread();
    sleep(10);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
}
