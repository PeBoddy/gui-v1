#include <windows.h>
HINSTANCE Instanz;
LRESULT CALLBACK Fensterfunktion(HWND, UINT, WPARAM, LPARAM);
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
            return 255;
    }

    Hauptfenster = CreateWindowEx(
        0,
        "Einfach",
        "Hello MS Winworld",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        dieseInstanz,
        NULL);
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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(Fenster, nachricht, wParam, lParam);
    }
    return 0;
}