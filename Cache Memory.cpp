#include <array>
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

class CacheInfo
{
public:
    CacheInfo();
    ~CacheInfo();

    /// <summary>
    /// Производитель процессора
    /// </summary>
    /// <returns></returns>
    string getProcessorVendor();

    /// <summary>
    /// Модель процеесора
    /// </summary>
    /// <returns></returns>
    string getProcessorName();


    void printCacheInfo()
    {
        cout << "Производитель ЦП : " << vendor << endl;
        cout << "Название ЦП : " << model << endl << endl;

        if (vendor == Intel)
        {
            printCacheInfoForIntel();
        }
        else if (vendor == AMD)
        {
            printCacheInfoForAMD();
        }
    }

private:

    std::string vendor;

    std::string model;

    /// <summary>
    /// Данные по кэш памяти процессоров Intel
    /// </summary>
    void printCacheInfoForIntel();

    /// <summary>
    /// Данные по кэш памяти процессоров AMD
    /// </summary>
    void printCacheInfoForAMD();

};

CacheInfo::CacheInfo()
{
    vendor = getProcessorVendor();
    
    model = getProcessorName();
}

CacheInfo::~CacheInfo()
{
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

    return std::string(vendor);
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

void CacheInfo::printCacheInfoForAMD() {
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize); // Получаем необходимый размер буфера

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);

    if (buffer == nullptr) {
        cerr << "Ошибка выделения памяти" << endl;
        return;
    }

    if (GetLogicalProcessorInformation(buffer, &bufferSize)) {
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
                        cacheType = "Уровень кэша: L1";
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
                        info.type = "Кэш инструкций";
                    }
                    else {
                        info.type = "Объединённый кэш";
                    }
                }
            }
        }

        // Выводим информацию
        for (const auto& entry : cacheInfo) {
            const string& type = entry.first;
            const Cache& info = entry.second;

            std::cout << type << ":\n"
                << "  Тип: " << info.type << "\n"
                << "  Количество: " << info.count << "\n"
                << "  Размер: " << info.sizeKB << " KB\n"
                << "  Размер строки: " << info.lineSize << " Bytes\n"
                << "  Уровень путей: " << info.ways << "\n"
                << "  Количество наборов: " << info.sets << "\n"
                << "\n" << "---------------------------" << "\n\n"; // Пустая строка для разделения между кэшами
        }
    }
    else {
        cerr << "Ошибка получения информации: " << GetLastError() << endl;
    }

    free(buffer); // Освобождаем память
}

void CacheInfo::printCacheInfoForIntel() {
    std::array<int, 4> cpuInfo; // Массив для хранения значений регистров EAX, EBX, ECX и EDX после вызова CPUID

    // Цикл для обхода уровней кэша от L1 до L3, для каждого уровня запрашиваем CPUID с ECX = i
    for (int i = 0; i < 4; i++) {
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
        std::cout << "Уровень кэша: L" << cacheLevel << std::endl;             // Уровень кэша (L1, L2, L3)
        std::cout << "Тип кэша: " << cacheTypeStr << std::endl;                // Тип кэша (данные, инструкции или объединённый)
        std::cout << "Размер кэша: " << cacheSize / 1024 << " КБ" << std::endl; // Размер кэша в килобайтах
        std::cout << "Размер строки кэша: " << cacheLineSize << " байт" << std::endl; // Размер строки кэша в байтах
        std::cout << "Количество путей: " << cacheWays << std::endl;            // Количество путей (associativity)
        std::cout << "Количество наборов: " << cacheSets << std::endl;          // Количество наборов
        std::cout << "---------------------------" << "\n\n";                // Разделитель для визуального удобства
    }
}


int main() 
{
    setlocale(LC_ALL, "ru");
    
    CacheInfo Proc;
    Proc.printCacheInfo();

    system("pause");
    return 0;
}
