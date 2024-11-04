#include "Cache.h"

using namespace std;

CacheInfo::CacheInfo()
{
    vendor = getProcessorVendor();

    model = getProcessorName();
}

CacheInfo::~CacheInfo()
{
}

void CacheInfo::getCacheInfo(bool CP = 1)
{
    if (vendor == Intel)
    {
        getCacheInfoForIntel();
    }
    else if (vendor == AMD)
    {
        getCacheInfoForAMD();
    }
}

string CacheInfo::getProcessorVendor()
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 0); // Получаем информацию о производителе процессора

    // Извлекаем производителя из регистра EBX, EDX, ECX
    char vendor[13];
    *((int*)&vendor[0]) = cpuInfo[1]; // EBX
    *((int*)&vendor[4]) = cpuInfo[3]; // EDX
    *((int*)&vendor[8]) = cpuInfo[2]; // ECX
    vendor[12] = '\0'; // Завершаем строку
    
    return string(vendor);
}

string CacheInfo::getProcessorName()
{
    array<int, 4> cpuInfo;
    char cpuBrandString[48] = { 0 }; // Буфер для названия процессора, размер 48 байт

    // Получаем информацию о производителе
    __cpuid(cpuInfo.data(), 0x80000000);
    unsigned int nIds = cpuInfo[0];

    // Получаем информацию о бренде процессора
    if (nIds >= 0x80000004) {
        __cpuid((int*)&cpuBrandString[0], 0x80000002);
        __cpuid((int*)&cpuBrandString[16], 0x80000003);
        __cpuid((int*)&cpuBrandString[32], 0x80000004);
    }

    return string(cpuBrandString);
}

void CacheInfo::getCacheInfoForAMD() 
{
    DWORD bufferSize = 0;
    string cinfo;
    //vector<string> str_info;
    GetLogicalProcessorInformation(nullptr, &bufferSize); // Получаем необходимый размер буфера

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);

    if (buffer == nullptr) 
{
        cinfo = "Ошибка выделения памяти\n";
        infoCache.push_back(cinfo);
    }

    if (GetLogicalProcessorInformation(buffer, &bufferSize)) 
    {
        // Словарь для хранения информации о кэшах
        map<string, Cache> cacheInfo;

        for (DWORD i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
            if (buffer[i].Relationship == RelationCache) {
                string cacheType;
                int lineSize = buffer[i].Cache.LineSize; // Размер строки кэша
                int cacheSizeKB = buffer[i].Cache.Size / 1024; // Размер кэша в КБ
                int ways = (buffer[i].Cache.Type == CacheUnified) ? 8 : 4; // Примерное значение для уровня путей
                int sets = (cacheSizeKB * 1024) / (lineSize * ways); // Количество наборов

                switch (buffer[i].Cache.Level) {
                case 1:
                    if (buffer[i].Cache.Type == CacheData) {
                        cacheType = "Уровень кэша: L1";
                    }
                    else if (buffer[i].Cache.Type == CacheInstruction) {
                        cacheType = "Уровень кэша: L1 ";
                    }
                    break;
                case 2:
                    cacheType = "Уровень кэша: L2";
                    break;
                case 3:
                    cacheType = "Уровень кэша: L3";
                    break;
                }

                if (!cacheType.empty()) {
                    auto& info = cacheInfo[cacheType];
                    info.count++; // Увеличиваем количество
                    info.sizeKB = cacheSizeKB; // Записываем размер
                    info.lineSize = lineSize; // Записываем размер строки
                    info.ways = ways; // Записываем уровень путей
                    info.sets = sets; // Записываем количество наборов

                    // Определяем тип кэша
                    if (buffer[i].Cache.Type == CacheData) {
                        info.type = "Кэш данных";
                    }
                    else if (buffer[i].Cache.Type == CacheInstruction) {
                        info.type = "Кэш инструкций ";
                    }
                    else {
                        info.type = "Объединённый кэш";
                    }
                }
            }
        }
        
        int i = 0;
        // Выводим информацию
        for (const auto& entry : cacheInfo) {
            const string& type = entry.first;
            const Cache& info = entry.second;

            cinfo = ""; // зануление строки

            cinfo += type + string(":\n")
                + "  Тип: " + info.type + string("\n")
                + "  Количество: " + to_string(info.count) + string("\n")
                + "  Размер: " + to_string(info.sizeKB) + string(" KB\n")
                + "  Размер строки: " + to_string(info.lineSize) + string(" Bytes\n")
                + "  Уровень путей: " + to_string(info.ways) + string("\n")
                + "  Количество наборов: " + to_string(info.sets) + string("\n")
                + string("\n\n"); // Пустая строка для разделения между кэшами
            infoCache.push_back(cinfo);
        }
    }
    else {
        cinfo += "Ошибка получения информации: \n";
        infoCache.push_back(cinfo);
    }

    free(buffer); // Освобождаем память
}

void CacheInfo::getCacheInfoForIntel() 
{
    std::array<int, 4> cpuInfo; // Массив для хранения значений регистров EAX, EBX, ECX и EDX после вызова CPUID
    string info;

    // Цикл для обхода уровней кэша от L1 до L3, для каждого уровня запрашиваем CPUID с ECX = i
    for (int i = 0; i < 4; i++) 
    {
        info = "";
#if defined(_MSC_VER)
        // Если используется компилятор MSVC, то вызываем CPUID с помощью встроенной функции __cpuidex
        __cpuidex(cpuInfo.data(), 4, i);
#else
        // Для других компиляторов используем ассемблерные вставки для вызова CPUID
        asm volatile (
            "cpuid"                            // Инструкция cpuid заполняет регистры данными
            : "=a" (cpuInfo[0]),               // Значение EAX сохранено в cpuInfo[0]
            "=b" (cpuInfo[1]),               // Значение EBX сохранено в cpuInfo[1]
            "=c" (cpuInfo[2]),               // Значение ECX сохранено в cpuInfo[2]
            "=d" (cpuInfo[3])                // Значение EDX сохранено в cpuInfo[3]
            : "a" (4), "c" (i)                 // Устанавливаем EAX = 4 для информации о кэше, ECX = i (уровень кэша)
            );
#endif

        // Если младшие 5 бит EAX равны 0, это значит, что уровней кэша больше нет, прерываем цикл
        if ((cpuInfo[0] & 0x1F) == 0) {
            break;
        }

        // Определяем параметры кэша, извлекая данные из соответствующих регистров
        int cacheType = cpuInfo[0] & 0x1F;                  // Тип кэша (данные, инструкции или объединённый) из младших 5 битов EAX
        int cacheLevel = (cpuInfo[0] >> 5) & 0x7;           // Уровень кэша (L1, L2, L3), биты 5-7 в EAX
        int cacheLineSize = (cpuInfo[1] & 0xFFF) + 1;       // Размер строки кэша (в байтах) из битов 0-11 в EBX
        int cachePartitions = ((cpuInfo[1] >> 12) & 0x3FF) + 1; // Количество разделов (partitions) из битов 12-21 в EBX
        int cacheWays = ((cpuInfo[1] >> 22) & 0x3FF) + 1;   // Количество путей (associativity) из битов 22-31 в EBX
        int cacheSets = cpuInfo[2] + 1;                     // Количество наборов (sets) из значения ECX

        // Рассчитываем общий размер кэша, умножая все параметры
        int cacheSize = cacheWays * cachePartitions * cacheLineSize * cacheSets;

        // Определяем тип кэша в виде строки
        std::string cacheTypeStr;
        switch (cacheType) {
        case 1: cacheTypeStr = "Кэш данных"; break;      // Если cacheType = 1, то это кэш данных
        case 2: cacheTypeStr = "Кэш инструкций"; break;   // Если cacheType = 2, то это кэш инструкций
        case 3: cacheTypeStr = "Объединённый кэш"; break; // Если cacheType = 3, то это объединённый кэш
        default: cacheTypeStr = "Неизвестный тип кэша"; break; // В остальных случаях тип неизвестен
        }
        
        // Выводим информацию о текущем уровне кэша
        info += "Уровень кэша: L" + to_string(cacheLevel) + '\n';             // Уровень кэша (L1, L2, L3)
        info += "  Тип кэша: " + cacheTypeStr + '\n';                // Тип кэша (данные, инструкции или объединённый)
        info += "  Размер кэша: " + to_string(cacheSize / 1024) + string(" КБ") + '\n'; // Размер кэша в килобайтах
        info += "  Размер строки кэша: " + to_string(cacheLineSize) + string(" байт") + '\n'; // Размер строки кэша в байтах
        info += "  Количество путей: " + to_string(cacheWays) +'\n';            // Количество путей (associativity)
        info += "  Количество наборов: " + to_string(cacheSets) + '\n';          // Количество наборов
        info += "\n\n";
        infoCache.push_back(info);
    }
    
}


std::string ReplaceNewlines(const std::string& input)
{
    std::string output;
    for (char c : input) {
        if (c == '\n') {
            output += "\r\n"; // Добавляем \r\n вместо \n
        }
        else {
            output += c;
        }
    }
    return output;
}