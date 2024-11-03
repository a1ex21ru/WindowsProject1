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

string CacheInfo::getCacheInfo()
{
    string info;
    info = "������������� �� : " + vendor + '\n';
    info += "�������� �� : " + model + "\n\n";

    if (vendor == Intel)
    {
        info += getCacheInfoForIntel();
        return info;
    }
    else if (vendor == AMD)
    {
        info += getCacheInfoForAMD();
        return info;
    }
}

string CacheInfo::getProcessorVendor()
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 0); // �������� ���������� � ������������� ����������

    // ��������� ������������� �� �������� EBX, EDX, ECX
    char vendor[13];
    *((int*)&vendor[0]) = cpuInfo[1]; // EBX
    *((int*)&vendor[4]) = cpuInfo[3]; // EDX
    *((int*)&vendor[8]) = cpuInfo[2]; // ECX
    vendor[12] = '\0'; // ��������� ������
    
    return string(vendor);
}

string CacheInfo::getProcessorName()
{
    array<int, 4> cpuInfo;
    char cpuBrandString[48] = { 0 }; // ����� ��� �������� ����������, ������ 48 ����

    // �������� ���������� � �������������
    __cpuid(cpuInfo.data(), 0x80000000);
    unsigned int nIds = cpuInfo[0];

    // �������� ���������� � ������ ����������
    if (nIds >= 0x80000004) {
        __cpuid((int*)&cpuBrandString[0], 0x80000002);
        __cpuid((int*)&cpuBrandString[16], 0x80000003);
        __cpuid((int*)&cpuBrandString[32], 0x80000004);
    }

    return string(cpuBrandString);
}

string CacheInfo::getCacheInfoForAMD() {
    DWORD bufferSize = 0;
    string cinfo;
    GetLogicalProcessorInformation(nullptr, &bufferSize); // �������� ����������� ������ ������

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);

    if (buffer == nullptr) 
{
        cinfo = "������ ��������� ������\n" ;
        return cinfo;
    }

    if (GetLogicalProcessorInformation(buffer, &bufferSize)) 
    {
        // ������� ��� �������� ���������� � �����
        map<string, Cache> cacheInfo;

        for (DWORD i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
            if (buffer[i].Relationship == RelationCache) {
                string cacheType;
                int lineSize = buffer[i].Cache.LineSize; // ������ ������ ����
                int cacheSizeKB = buffer[i].Cache.Size / 1024; // ������ ���� � ��
                int ways = (buffer[i].Cache.Type == CacheUnified) ? 8 : 4; // ��������� �������� ��� ������ �����
                int sets = (cacheSizeKB * 1024) / (lineSize * ways); // ���������� �������

                switch (buffer[i].Cache.Level) {
                case 1:
                    if (buffer[i].Cache.Type == CacheData) {
                        cacheType = "������� ����: L1";
                    }
                    else if (buffer[i].Cache.Type == CacheInstruction) {
                        cacheType = "������� ����: L1 ";
                    }
                    break;
                case 2:
                    cacheType = "������� ����: L2";
                    break;
                case 3:
                    cacheType = "������� ����: L3";
                    break;
                }

                if (!cacheType.empty()) {
                    auto& info = cacheInfo[cacheType];
                    info.count++; // ����������� ����������
                    info.sizeKB = cacheSizeKB; // ���������� ������
                    info.lineSize = lineSize; // ���������� ������ ������
                    info.ways = ways; // ���������� ������� �����
                    info.sets = sets; // ���������� ���������� �������

                    // ���������� ��� ����
                    if (buffer[i].Cache.Type == CacheData) {
                        info.type = "��� ������";
                    }
                    else if (buffer[i].Cache.Type == CacheInstruction) {
                        info.type = "��� ����������";
                    }
                    else {
                        info.type = "����������� ���";
                    }
                }
            }
        }
        

        // ������� ����������
        for (const auto& entry : cacheInfo) {
            const string& type = entry.first;
            const Cache& info = entry.second;

            cinfo += type + string(":\n")
                + "  ���: " + info.type + string("\n")
                + "  ����������: " + to_string(info.count) + string("\n")
                + "  ������: " + to_string(info.sizeKB) + string(" KB\n")
                + "  ������ ������: " + to_string(info.lineSize) + string(" Bytes\n")
                + "  ������� �����: " + to_string(info.ways) + string("\n")
                + "  ���������� �������: " + to_string(info.sets) + string("\n")
                + string("\n---------------------------\n\n"); // ������ ������ ��� ���������� ����� ������
        }
    }
    else {
        cinfo += "������ ��������� ����������: \n";
    }

    free(buffer); // ����������� ������
    return cinfo;
}

string CacheInfo::getCacheInfoForIntel() {
    std::array<int, 4> cpuInfo; // ������ ��� �������� �������� ��������� EAX, EBX, ECX � EDX ����� ������ CPUID
    string info;

    // ���� ��� ������ ������� ���� �� L1 �� L3, ��� ������� ������ ����������� CPUID � ECX = i
    for (int i = 0; i < 4; i++) 
    {
#if defined(_MSC_VER)
        // ���� ������������ ���������� MSVC, �� �������� CPUID � ������� ���������� ������� __cpuidex
        __cpuidex(cpuInfo.data(), 4, i);
#else
        // ��� ������ ������������ ���������� ������������ ������� ��� ������ CPUID
        asm volatile (
            "cpuid"                            // ���������� cpuid ��������� �������� �������
            : "=a" (cpuInfo[0]),               // �������� EAX ��������� � cpuInfo[0]
            "=b" (cpuInfo[1]),               // �������� EBX ��������� � cpuInfo[1]
            "=c" (cpuInfo[2]),               // �������� ECX ��������� � cpuInfo[2]
            "=d" (cpuInfo[3])                // �������� EDX ��������� � cpuInfo[3]
            : "a" (4), "c" (i)                 // ������������� EAX = 4 ��� ���������� � ����, ECX = i (������� ����)
            );
#endif

        // ���� ������� 5 ��� EAX ����� 0, ��� ������, ��� ������� ���� ������ ���, ��������� ����
        if ((cpuInfo[0] & 0x1F) == 0) {
            break;
        }

        // ���������� ��������� ����, �������� ������ �� ��������������� ���������
        int cacheType = cpuInfo[0] & 0x1F;                  // ��� ���� (������, ���������� ��� �����������) �� ������� 5 ����� EAX
        int cacheLevel = (cpuInfo[0] >> 5) & 0x7;           // ������� ���� (L1, L2, L3), ���� 5-7 � EAX
        int cacheLineSize = (cpuInfo[1] & 0xFFF) + 1;       // ������ ������ ���� (� ������) �� ����� 0-11 � EBX
        int cachePartitions = ((cpuInfo[1] >> 12) & 0x3FF) + 1; // ���������� �������� (partitions) �� ����� 12-21 � EBX
        int cacheWays = ((cpuInfo[1] >> 22) & 0x3FF) + 1;   // ���������� ����� (associativity) �� ����� 22-31 � EBX
        int cacheSets = cpuInfo[2] + 1;                     // ���������� ������� (sets) �� �������� ECX

        // ������������ ����� ������ ����, ������� ��� ���������
        int cacheSize = cacheWays * cachePartitions * cacheLineSize * cacheSets;

        // ���������� ��� ���� � ���� ������
        std::string cacheTypeStr;
        switch (cacheType) {
        case 1: cacheTypeStr = "��� ������"; break;      // ���� cacheType = 1, �� ��� ��� ������
        case 2: cacheTypeStr = "��� ����������"; break;   // ���� cacheType = 2, �� ��� ��� ����������
        case 3: cacheTypeStr = "����������� ���"; break; // ���� cacheType = 3, �� ��� ����������� ���
        default: cacheTypeStr = "����������� ��� ����"; break; // � ��������� ������� ��� ����������
        }
        
        // ������� ���������� � ������� ������ ����
        info += "������� ����: L" + to_string(cacheLevel) + '\n';             // ������� ���� (L1, L2, L3)
        info += "��� ����: " + cacheTypeStr + '\n';                // ��� ���� (������, ���������� ��� �����������)
        info += "������ ����: " + to_string(cacheSize / 1024) + string(" ��") + '\n'; // ������ ���� � ����������
        info += "������ ������ ����: " + to_string(cacheLineSize) + string(" ����") + '\n'; // ������ ������ ���� � ������
        info += "���������� �����: " + to_string(cacheWays) +'\n';            // ���������� ����� (associativity)
        info += "���������� �������: " + to_string(cacheSets) + '\n';          // ���������� �������
        info += "\n\n";                                             // ����������� ��� ����������� ��������

        //std::cout << "������� ����: L" << cacheLevel << std::endl;             // ������� ���� (L1, L2, L3)
        //std::cout << "��� ����: " << cacheTypeStr << std::endl;                // ��� ���� (������, ���������� ��� �����������)
        //std::cout << "������ ����: " << cacheSize / 1024 << " ��" << std::endl; // ������ ���� � ����������
        //std::cout << "������ ������ ����: " << cacheLineSize << " ����" << std::endl; // ������ ������ ���� � ������
        //std::cout << "���������� �����: " << cacheWays << std::endl;            // ���������� ����� (associativity)
        //std::cout << "���������� �������: " << cacheSets << std::endl;          // ���������� �������
        //std::cout << "---------------------------" << "\n\n";                // ����������� ��� ����������� ��������
    }
    return info;
}

