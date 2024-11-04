#pragma once

#include <array>
#include <vector>
#include <intrin.h>
#include <iostream>
#include <map>
#include <string>
#include <windows.h>

#define AMD R"(AuthenticAMD)" 
#define Intel R"(GenuineIntel)"

using namespace std;

struct Cache {
    int count;       // Количество кэшей
    int sizeKB;      // Размер кэша в КБ
    int lineSize;    // Размер строки кэша в байтах
    int ways;        // Уровень путей
    int sets;        // Количество наборов
    string type; // Тип кэша

    Cache() : count(0), sizeKB(0), lineSize(0), ways(0), sets(0), type("") {} // Конструктор по умолчанию
};

struct CPAndCacheInfo
{
    string vendor;
    string model;

    string L1_data;
    string L1_code;

    string L2;
    string L3;


    CPAndCacheInfo() = default;
};

class CacheInfo
{
public:
    CacheInfo();
    ~CacheInfo();

    /// <summary>
    /// Геттер производителя процессора
    /// </summary>
    /// <returns></returns>
    string getProcessorVendor();

    /// <summary>
    /// Геттер модели процеесора
    /// </summary>
    /// <returns></returns>
    string getProcessorName();

    /// <summary>
    /// Получение информации о кэш-памяти
    /// </summary>
    void getCacheInfo();

    /// <summary>
    /// Геттер информации о кэш-памяти
    /// </summary>
    /// <returns></returns>
    vector<string> getVector();
    
private:

    /// <summary>
    /// Вектор значений кэша
    /// </summary>
    vector<string> infoCache;

    /// <summary>
    /// Производитель ЦП
    /// </summary>
    std::string vendor;

    /// <summary>
    /// Название ЦП
    /// </summary>
    std::string model;

    /// <summary>
    /// Данные по кэш памяти процессоров Intel
    /// </summary>
    void getCacheInfoForIntel();

    /// <summary>
    /// Данные по кэш памяти процессоров AMD
    /// </summary>
    void getCacheInfoForAMD();

};

std::string ReplaceNewlines(const std::string& input);