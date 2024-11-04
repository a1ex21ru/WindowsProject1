// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"
#include "Cache.h"
#include <intrin.h>
#include <string>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CacheInfo proc;

vector<string> info;


int CALLBACK wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    HBRUSH hBrushLightGray = CreateSolidBrush(RGB(240, 240, 240));
    wcex.hbrBackground = hBrushLightGray; // Установка кисти как фона
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    /// Создание окна
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        483,
        43,
        500,
        500,
        nullptr, nullptr, hInstance, nullptr);
    
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{



    vector<HWND*> windowsForInfo;

    switch (message)
    {
    case WM_CREATE:
    {    
        // Получаем информацию о кэше
        proc.getCacheInfo();
        info = proc.getVector();

        SetWindowText(hWnd, L"Кэш-память процессора");

        // Создание текстового поля
        HWND hEditL1D = CreateWindowExW(
            0,
            L"EDIT",
            L"CPU Information",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_READONLY,
            1,
            50,
            200,
            100,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );
        windowsForInfo.push_back(&hEditL1D);

        HWND hEditL1I = CreateWindowExW(
            0,
            L"EDIT",
            L"CPU Information",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_READONLY,
            200,
            50,
            200,
            100,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );
        windowsForInfo.push_back(&hEditL1I);

        HWND hEditL2 = CreateWindowExW(
            0,
            L"EDIT",
            L"CPU Information",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_READONLY,
            1,
            200,
            200,
            100,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );
        windowsForInfo.push_back(&hEditL2);

        HWND hEditL3 = CreateWindowExW(
            0,
            L"EDIT",
            L"CPU Information",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_READONLY,
            200,
            200,
            200,
            100,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );
        windowsForInfo.push_back(&hEditL3);

        HWND hEditName = CreateWindowExW(
            0,
            L"EDIT",
            L"CPU Information",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_CENTER | ES_READONLY,
            100,
            0,
            300,
            50,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );
        windowsForInfo.push_back(&hEditName);

        /// Создаем шрифт
        HFONT hFont = CreateFontW(
            14,                // Высота шрифта
            0,                 // Ширина шрифта (0 - автоматически)
            0,                 // Угол наклона
            0,                 // Угол наклона
            FW_MEDIUM,          // Толщина шрифта (FW_BOLD - жирный)
            FALSE,            // Курсив
            FALSE,            // Подчеркивание
            FALSE,            // Зачеркивание
            DEFAULT_CHARSET,  // Набор символов
            OUT_DEFAULT_PRECIS, // Прецизионный вывод
            CLIP_DEFAULT_PRECIS, // Обрезка
            DEFAULT_QUALITY,  // Качество
            DEFAULT_QUALITY, // Высокое качество
            L"Consolas"        // Имя шрифта
        );

        // Информация о процессоре
        std::string CP = proc.getProcessorVendor() + '\n' + proc.getProcessorName();
        info.push_back(CP);

        /// Устанавливаем текст в текстовое поле
        std::string cacheWithR;
        for (size_t i = 0; i < windowsForInfo.size(); i++)
        {
            cacheWithR = ReplaceNewlines(info[i]);
            SetWindowTextA(*windowsForInfo[i], cacheWithR.c_str());

            /// УСтановка шрифта
            SendMessage(*windowsForInfo[i], WM_SETFONT, (WPARAM)hFont, TRUE);
        }

    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:

             
            // Отображаем информацию о кэше
            //MessageBoxA(hWnd, cacheInfo.c_str(), "Информация о кэше", MB_OK);
            
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        //HDC hdc = GetDC(hWnd);
        //string cacheInfo = proc.getCacheInfo();
        //TextOutA(hdc, 100, 10, cacheInfo.c_str(), cacheInfo.length());
        ReleaseDC(hWnd, hdc);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
