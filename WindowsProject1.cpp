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
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
        1100,
        150,
        400,
        700,
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
    // Получаем информацию о кэше
    string cacheInfo = proc.getCacheInfo();
    switch (message)
    {
    case WM_CREATE:
    {
        SetWindowText(hWnd, L"Кэш-память процессора");
        // Создание кнопки
        //HWND hButton = CreateWindow(
        //    L"BUTTON",  // Предопределенный класс кнопки
        //    L"Получить информацию о кэше", // Текст кнопки
        //    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Стиль кнопки
        //    50,         // Положение X
        //    50,         // Положение Y
        //    250,        // Ширина
        //    30,         // Высота
        //    hWnd,       // Родительское окно
        //    NULL,       // Идентификатор кнопки
        //    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), // Дескриптор приложения
        //    NULL        // Дополнительные параметры
        //);
        // Создание текстового поля
        HWND hEdit = CreateWindowExW(
            0,
            L"EDIT",
            L"CPU Information",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT,
            1,
            1,
            350,
            560,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );

        /// Создаем шрифт
        HFONT hFont = CreateFontW(
            16,                // Высота шрифта
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

        /// Устанавливаем текст в текстовое поле
        std::string cacheWithR = ReplaceNewlines(cacheInfo);
        SetWindowTextA(hEdit, cacheWithR.c_str());

        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        
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
