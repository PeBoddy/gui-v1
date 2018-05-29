#include <windows.h>
#include <Windowsx.h>
#include <iostream>
#include <sstream>

using namespace std;

HINSTANCE Instanz;
LRESULT CALLBACK Fensterfunktion(HWND, UINT, WPARAM, LPARAM);

#define UP_BUTTON 1
#define DOWN_BUTTON 2

/*
 * UINT_MAX         = 4294967295
 * Fibonacci(48)    = 4807526976
 * -> Maximal bis Index 47
 */
#define FIBO_MAX 47

#define LABEL_W 50
#define BASE_H 20
#define MARGIN 10
#define WINDOW_PADDING 20
#define BUTTON_W 75
#define BUTTON_H 30
#define INPUT_W 100

// Prototypes
void AddMenu(HWND);
void AddControls(HWND);
void createDialog(HINSTANCE);
void displayDialog(HWND);
unsigned int fibVoni(int);

// Window-Handler
HMENU hMenu;
HWND hDialog, hintIOut, hFIOut;

// Variables
int i = 0;
int output = 0;
stringstream ss;
string str;

unsigned int *cache;

//-----------------Windows - Hauptprogramm ------------------------------------
int WINAPI WinMain(HINSTANCE dieseInstanz,
                   HINSTANCE vorherigeInstanz,
                   LPSTR lpszArgument,
                   int FensterStil)
{
    MSG Meldung;
    HWND Hauptfenster;
    Instanz = dieseInstanz;
    if (!vorherigeInstanz)
    {
        WNDCLASSEX wincl;
        wincl.style = 0;
        wincl.lpfnWndProc = Fensterfunktion;
        wincl.cbClsExtra = 0;
        wincl.cbWndExtra = 0;
        wincl.hInstance = dieseInstanz;
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
        createDialog(dieseInstanz);
    }

    cache = (unsigned int *)calloc(sizeof(unsigned int), FIBO_MAX - 3);

    Hauptfenster = CreateWindowEx(0, "Einfach", "Fibonacci-Zahlen", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 500, NULL, NULL, dieseInstanz, NULL);

    if (!Hauptfenster)
        return 255;
    ShowWindow(Hauptfenster, FensterStil);
    while (GetMessage(&Meldung, NULL, 0, 0))
        DispatchMessage(&Meldung);
    return Meldung.wParam;
} // Ende WinMain

//-----------------Callback Funktion des Hauptfensters ------------------------
LRESULT CALLBACK Fensterfunktion(HWND Fenster, UINT nachricht, WPARAM wParam,
                                 LPARAM lParam)
{
    switch (nachricht)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case 1:
            displayDialog(Fenster);
            break;
        }
    case WM_CREATE:
        AddMenu(Fenster);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(Fenster, nachricht, wParam, lParam);
    }
    return 0;
}

//-----------------Callback Funktion des Dialogs ------------------------
LRESULT CALLBACK DialogFunktion(HWND dialogFenster, UINT nachricht, WPARAM wParam,
                                LPARAM lParam)
{
    switch (nachricht)
    {
    case WM_CLOSE:
        DestroyWindow(dialogFenster);
        break;
    case WM_COMMAND:
        if (wParam == UP_BUTTON || wParam == DOWN_BUTTON)
        {
            output = fibVoni(i);

            cout << i << ": " << output << endl;

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
                hintIOut,
                stringIndex);

            char stringValue[20];
            sprintf(stringValue, "%d", output);
            Edit_SetText(
                hFIOut,
                stringValue);
        }
    default:
        return DefWindowProcW(dialogFenster, nachricht, wParam, lParam);
    }
}

// Add Menübar
void AddMenu(HWND Fenster)
{
    hMenu = CreateMenu();

    AppendMenu(hMenu, MF_STRING, 1, "Open Dialog");
    SetMenu(Fenster, hMenu);
}

// Create Dialog
void createDialog(HINSTANCE dieseInstanz)
{
    WNDCLASSW dialog = {0};
    dialog.lpfnWndProc = DialogFunktion;
    dialog.hInstance = dieseInstanz;
    dialog.hCursor = LoadCursor(NULL, IDC_ARROW);
    dialog.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    dialog.lpszClassName = L"DialogClass";

    RegisterClassW(&dialog);
}

// Display Dialog
void displayDialog(HWND dialogFenster)
{
    // http://www.directxtutorial.com/Lesson.aspx?lessonid=11-1-4

    int dialogWidth = (int)(WINDOW_PADDING + LABEL_W + MARGIN + INPUT_W + WINDOW_PADDING);
    int dialogHeight = (int)(WINDOW_PADDING + BASE_H + MARGIN + BASE_H + MARGIN + BUTTON_H + WINDOW_PADDING);

    RECT rect = {0, 0, dialogWidth, dialogHeight};

    // cout << "not adjusted: " << rect.left << " " << rect.top << " " << rect.right << " " << rect.bottom << endl;

    bool adjusted = AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    // cout << "adjusted: " << adjusted << " " << rect.left << " " << rect.top << " " << rect.right << " " << rect.bottom << endl;

    hDialog = CreateWindowW(
        L"DialogClass",
        L"Fibonaccifolge",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        200,                    // X
        200,                    // Y
        rect.right - rect.left, // WIDTH
        rect.bottom - rect.top, // HEIGHT
        dialogFenster,
        NULL,
        NULL,
        NULL);

    AddControls(hDialog);
}

// Labels
void AddControls(HWND dialogFenster)
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
        dialogFenster,
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
        dialogFenster,
        NULL,
        NULL,
        NULL);

    // Output
    hintIOut = CreateWindowW(
        L"edit",
        L"",
        WS_VISIBLE | WS_CHILD,
        WINDOW_PADDING + LABEL_W + MARGIN, // X
        WINDOW_PADDING,                    // Y
        INPUT_W,                           // WIDTH
        BASE_H,                            // HEIGHT
        dialogFenster,
        NULL,
        NULL,
        NULL);

    hFIOut = CreateWindowW(
        L"edit",
        L"",
        WS_VISIBLE | WS_CHILD,
        WINDOW_PADDING + LABEL_W + MARGIN, // X
        WINDOW_PADDING + BASE_H + MARGIN,  // Y
        INPUT_W,                           // WIDTH
        BASE_H,                            // HEIGHT
        dialogFenster,
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
        dialogFenster,
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
        dialogFenster,
        (HMENU)UP_BUTTON,
        NULL,
        NULL);
}

// FibVoni
unsigned int fibVoni(int fib)
{

    if (fib <= 0)
    {
        return 0;
    }

    if (fib < 3)
    {
        return 1;
    }

    if (fib >= FIBO_MAX)
    {
        fib = FIBO_MAX;
    }

    if (cache[fib] == 0)
    {
        cache[fib] = fibVoni(fib - 1) + fibVoni(fib - 2);
    }

    return cache[fib];
}
