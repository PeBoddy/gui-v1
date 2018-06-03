#include <windows.h>
#include <Windowsx.h>
#include <iostream>
#include <sstream>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

using namespace std;

HINSTANCE instance;
LRESULT CALLBACK windowCallback(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK dialogCallback(HWND, UINT, WPARAM, LPARAM);

#define UP_BUTTON 1
#define DOWN_BUTTON 2

/*
 * UINT_MAX         = 4294967295
 * Fibonacci(48)    = 4807526976
 * -> Maximal bis Index 47
 */
#define FIBO_MAX 47

// Layout definitions
#define LABEL_W 50
#define BASE_H 20
#define MARGIN 10
#define WINDOW_PADDING 20
#define BUTTON_W 75
#define BUTTON_H 30
#define INPUT_W 100

#define MAIN_WINDOW_W 800
#define MAIN_WINDOW_H 600

// Prototypes
void addMenu(HWND);
void addControls(HWND);
void createDialog(HINSTANCE);
void displayDialog(HWND);
void OnPaint(Graphics &);
unsigned int fibVoni(int);

// Window-Handler
HMENU hMenu;
HWND hDialog;
HWND hIndexOut;
HWND hValueOut;
HWND hMainWindow;

// Variables
int i = 0;
int output = 0;
int coordinatesZeroX = 2 * WINDOW_PADDING;
int coordinatesZeroY = MAIN_WINDOW_H - (2 * WINDOW_PADDING);
int xLength = MAIN_WINDOW_W - (3 * WINDOW_PADDING);
int yLength = MAIN_WINDOW_H - (3 * WINDOW_PADDING);
int xOffset = xLength / FIBO_MAX;

unsigned int *cache;

//-----------------Windows - Hauptprogramm ------------------------------------
int WINAPI WinMain(HINSTANCE thisInstance,
                   HINSTANCE previousInstance,
                   LPSTR lpszArgument,
                   int windowStyle)
{
    MSG message;
    instance = thisInstance;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    if (!previousInstance)
    {
        WNDCLASSEX wincl;
        wincl.style = 0;
        wincl.lpfnWndProc = windowCallback;
        wincl.cbClsExtra = 0;
        wincl.cbWndExtra = 0;
        wincl.hInstance = thisInstance;
        wincl.hIcon = 0;
        wincl.hIconSm = 0;
        wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
        wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
        wincl.lpszMenuName = NULL;
        wincl.lpszClassName = "Einfach";
        wincl.cbSize = sizeof(WNDCLASSEX);

        if (!RegisterClassEx(&wincl))
        {
            return 255;
        }

        createDialog(thisInstance);
    }

    cache = (unsigned int *)calloc(sizeof(unsigned int), FIBO_MAX);

    // http://www.directxtutorial.com/Lesson.aspx?lessonid=11-1-4
    RECT rect = {0, 0, MAIN_WINDOW_W, MAIN_WINDOW_H};
    bool adjusted = AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

    hMainWindow = CreateWindowEx(
        0,
        "Einfach",
        "Fibonacci-Zahlen",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left, // WIDTH
        rect.bottom - rect.top, // HEIGHT
        NULL,
        NULL,
        thisInstance,
        NULL);

    if (!hMainWindow)
        return 255;

    ShowWindow(hMainWindow, windowStyle);

    while (GetMessage(&message, NULL, 0, 0))
        DispatchMessage(&message);

    GdiplusShutdown(gdiplusToken);

    return message.wParam;
} // Ende WinMain

//-----------------Callback Funktion des hMainWindows ------------------------
LRESULT CALLBACK windowCallback(HWND window, UINT messageCode, WPARAM wParam,
                                LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (messageCode)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case 1:
            displayDialog(window);
            break;
        }
    case WM_CREATE:
        addMenu(window);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        if (window == hMainWindow)
        {
            hdc = BeginPaint(window, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_BACKGROUND));
            Graphics graphics(hdc);
            OnPaint(graphics);
            EndPaint(window, &ps);
        }
    default:
        return DefWindowProc(window, messageCode, wParam, lParam);
    }

    return 0;
}

//-----------------Callback Funktion des Dialogs ------------------------
LRESULT CALLBACK dialogCallback(HWND dialog, UINT messageCode, WPARAM wParam, LPARAM lParam)
{
    switch (messageCode)
    {
    case WM_CLOSE:
        DestroyWindow(dialog);
        break;
    case WM_COMMAND:
        if (wParam == UP_BUTTON || wParam == DOWN_BUTTON)
        {
            switch (wParam)
            {
            case UP_BUTTON:
                if (i < 47)
                {
                    i++;
                }
                break;
            case DOWN_BUTTON:
                if (i > 0)
                {
                    i--;
                }
                break;
            }

            char stringIndex[3];
            sprintf(stringIndex, "%d", i);
            Edit_SetText(
                hIndexOut,
                stringIndex);

            char stringValue[20];
            sprintf(stringValue, "%d", fibVoni(i));
            Edit_SetText(
                hValueOut,
                stringValue);

            const RECT rect = {0, 0, MAIN_WINDOW_W, MAIN_WINDOW_H};
            InvalidateRect(hMainWindow, &rect, true);
        }
    default:
        return DefWindowProcW(dialog, messageCode, wParam, lParam);
    }
}

// Add Menübar
void addMenu(HWND window)
{
    hMenu = CreateMenu();

    AppendMenu(hMenu, MF_STRING, 1, "Open Dialog");
    SetMenu(window, hMenu);
}

// Create Dialog
void createDialog(HINSTANCE thisInstance)
{
    WNDCLASSW dialog = {0};
    dialog.lpfnWndProc = dialogCallback;
    dialog.hInstance = thisInstance;
    dialog.hCursor = LoadCursor(NULL, IDC_ARROW);
    dialog.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    dialog.lpszClassName = L"DialogClass";

    RegisterClassW(&dialog);
}

// Display Dialog
void displayDialog(HWND dialog)
{
    // http://www.directxtutorial.com/Lesson.aspx?lessonid=11-1-4
    int dialogWidth = (int)(WINDOW_PADDING + LABEL_W + MARGIN + INPUT_W + WINDOW_PADDING);
    int dialogHeight = (int)(WINDOW_PADDING + BASE_H + MARGIN + BASE_H + MARGIN + BUTTON_H + WINDOW_PADDING);

    RECT rect = {0, 0, dialogWidth, dialogHeight};
    bool adjusted = AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    hDialog = CreateWindowW(
        L"DialogClass",
        L"Fibonaccifolge",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        200,                    // X
        200,                    // Y
        rect.right - rect.left, // WIDTH
        rect.bottom - rect.top, // HEIGHT
        dialog,
        NULL,
        NULL,
        NULL);

    addControls(hDialog);
}

// Labels
void addControls(HWND dialog)
{
    // Header
    CreateWindowW(
        L"static",
        L"int i",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        WINDOW_PADDING, // X
        WINDOW_PADDING, // Y
        LABEL_W,        // WIDTH
        BASE_H,         // HEIGHT
        dialog,
        NULL,
        NULL,
        NULL);

    CreateWindowW(
        L"static",
        L"F ( i )",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        WINDOW_PADDING,                   // X
        WINDOW_PADDING + BASE_H + MARGIN, // Y
        LABEL_W,                          // WIDTH
        BASE_H,                           // HEIGHT
        dialog,
        NULL,
        NULL,
        NULL);

    // Output
    hIndexOut = CreateWindowW(
        L"edit",
        L"",
        WS_VISIBLE | WS_CHILD,
        WINDOW_PADDING + LABEL_W + MARGIN, // X
        WINDOW_PADDING,                    // Y
        INPUT_W,                           // WIDTH
        BASE_H,                            // HEIGHT
        dialog,
        NULL,
        NULL,
        NULL);

    hValueOut = CreateWindowW(
        L"edit",
        L"",
        WS_VISIBLE | WS_CHILD,
        WINDOW_PADDING + LABEL_W + MARGIN, // X
        WINDOW_PADDING + BASE_H + MARGIN,  // Y
        INPUT_W,                           // WIDTH
        BASE_H,                            // HEIGHT
        dialog,
        NULL,
        NULL,
        NULL);

    // Buttons
    CreateWindowW(
        L"button",
        L"Down",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        WINDOW_PADDING,                           // X
        WINDOW_PADDING + (2 * (MARGIN + BASE_H)), // Y
        BUTTON_W,                                 // WIDTH
        BUTTON_H,                                 // HEIGHT
        dialog,
        (HMENU)DOWN_BUTTON,
        NULL,
        NULL);

    CreateWindowW(
        L"button",
        L"Up",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        WINDOW_PADDING + BUTTON_W + MARGIN,       // X
        WINDOW_PADDING + (2 * (MARGIN + BASE_H)), // Y
        BUTTON_W,                                 // WIDTH
        BUTTON_H,                                 // HEIGHT
        dialog,
        (HMENU)UP_BUTTON,
        NULL,
        NULL);

    char stringIndex[3];
    sprintf(stringIndex, "%d", i);
    Edit_SetText(
        hIndexOut,
        stringIndex);

    char stringValue[20];
    sprintf(stringValue, "%d", fibVoni(i));
    Edit_SetText(
        hValueOut,
        stringValue);
}

// FibVoni
unsigned int fibVoni(int fib)
{

    if (fib <= 0)
    {
        cache[fib] = 0;
    }

    if (fib < 3)
    {
        cache[fib] = 1;
    }

    if (fib >= FIBO_MAX - 1)
    {
        fib = FIBO_MAX - 1;
    }

    if (cache[fib] == 0)
    {
        cache[fib] = fibVoni(fib - 1) + fibVoni(fib - 2);
    }

    return cache[fib];
}

Status drawPoint(Graphics &graphics, int x, int y)
{
    SolidBrush brush(Color(255, 255, 0, 0));
    return graphics.FillEllipse(&brush, x - 2, y - 2, 4, 4);
}

void OnPaint(Graphics &graphics)
{
    // Draw X and Y axis
    Pen pen(Color(255, 0, 0, 0), 2.0f);

    graphics.DrawLine(&pen, WINDOW_PADDING, coordinatesZeroY, MAIN_WINDOW_W - WINDOW_PADDING, coordinatesZeroY);
    graphics.DrawLine(&pen, coordinatesZeroX, MAIN_WINDOW_H - WINDOW_PADDING, coordinatesZeroX, WINDOW_PADDING);

    // Draw Points
    for (int j = 0; j < FIBO_MAX; j++)
    {
        int x = coordinatesZeroX + (j * xOffset);
        int y = 0;
        Status status;

        if (j == 0 || cache[j] != 0)
        {
            y = coordinatesZeroY - cache[j];

            status = drawPoint(graphics, x, y);

            cout << "Point drawn at [" << x << ", " << y << "]"
                 << " Status: " << status << endl;
        }

        if (j == i)
        {
            Pen pen2(Color(255, 0, 0, 255));
            graphics.DrawEllipse(&pen2, x - 10, y - 10, 20, 20);
        }
    }

    cout << endl
         << endl;
}
