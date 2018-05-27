#include <windows.h>
#include <iostream>
#include <sstream>

using namespace std;

HINSTANCE Instanz;
LRESULT CALLBACK Fensterfunktion(HWND, UINT, WPARAM, LPARAM);

#define UP_BUTTON 1
#define DOWN_BUTTON 2

// Prototypes
void AddMenu(HWND);
void AddControls(HWND);
void createDialog(HINSTANCE);
void displayDialog(HWND);
int fibVoni(int);

// Window-Handler
HMENU hMenu;
HWND hDialog, hintIOut, hFIOut;

// Variables
int i = 0;
int output = 0;
stringstream ss;
string str;



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
        
        if (!RegisterClassEx(&wincl)) {
            return 255;
        }
        createDialog(dieseInstanz);
    }

    Hauptfenster = CreateWindowEx( 0,"Einfach","Fibonacci-Zahlen",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,800,500,NULL,NULL,dieseInstanz,NULL);

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
        switch(wParam) {
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
            switch(wParam) {
                case UP_BUTTON:     
                output = fibVoni(i);
                    i++;
                    break;
                case DOWN_BUTTON:
                    i--;
                break;
            }
        default:
        return DefWindowProcW(dialogFenster,nachricht,wParam,lParam);
    }
}

// Add Menübar
void AddMenu(HWND Fenster) {
    hMenu = CreateMenu();

    AppendMenu(hMenu, MF_STRING, 1,"Open Dialog");
    SetMenu(Fenster, hMenu);
}

// Create Dialog
void createDialog(HINSTANCE dieseInstanz) {
    WNDCLASSW dialog = {0};
        dialog.lpfnWndProc = DialogFunktion;
        dialog.hInstance = dieseInstanz;
        dialog.hCursor = LoadCursor(NULL, IDC_CROSS);
        dialog.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
        dialog.lpszClassName = L"DialogClass";

        RegisterClassW(&dialog);
}


// Display Dialog
void displayDialog(HWND dialogFenster) {
    hDialog = CreateWindowW(L"DialogClass",L"Fibonaccifolge", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 200,200,450,400,dialogFenster,NULL,NULL,NULL);

    AddControls(hDialog);
}

// Labels
void AddControls(HWND dialogFenster) {
    // Header
    CreateWindowW(L"static",L"int i",WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER, 20,100,50,20,dialogFenster,NULL,NULL,NULL);
    CreateWindowW(L"static",L"F ( i )",WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER, 20,125,50,20,dialogFenster,NULL,NULL,NULL);

    // Output
    hintIOut = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD | WS_BORDER, 80,100,25,20,dialogFenster,NULL,NULL,NULL);
    hFIOut = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD | WS_BORDER,  80,125,25,20,dialogFenster,NULL,NULL,NULL);

    // Buttons
    CreateWindowW(L"button",L"Up",WS_VISIBLE | WS_CHILD | SS_CENTER, 20,170,70,30,dialogFenster,(HMENU) UP_BUTTON,NULL,NULL);
    CreateWindowW(L"button",L"Down",WS_VISIBLE | WS_CHILD | SS_CENTER, 100,170,70,30,dialogFenster,(HMENU) DOWN_BUTTON,NULL,NULL);
}

// FibVoni
int fibVoni(int fib) {
    
    if(fib < 0) {
        //cout << "Bitte eine Zahl größer gleich 0 eingeben!"
    }

    if(fib == 0) {
        return 0;
    }
    if(fib == 1 || fib == 2) {
        return 1;
    }
    else {
        return fibVoni(fib-1) + fibVoni(fib-2);
    }
}


