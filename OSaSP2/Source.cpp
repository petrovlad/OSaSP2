#include <windows.h>
#include <fstream>
#include <string>

#define INPUT_FILE "C:\\Users\\vladi\\Desktop\\TestFiles\\input.txt"
#define DEFAULT_N 5
#define DEFAULT_M 7

VOID OnPaint(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam);

int N, M;
int minClientRectHeight = 0;
const int minClientRectWidth = 200;
std::string* input;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow) {
    // read file
    std::string line;
    std::ifstream in(INPUT_FILE);
    if (in.is_open()) {
        if (getline(in, line)) {
            N = (int)std::strtol(line.c_str(), NULL, 10);
            if (N <= 0) {
                N = DEFAULT_N;
            }
        }
        else {
            N = DEFAULT_N;
        }

        if (getline(in, line)) {
            M = (int)std::strtol(line.c_str(), NULL, 10);
            if (M <= 0) {
                M = DEFAULT_M;
            }
        }
        else {
            M = DEFAULT_M;
        }

        int size = N * M, j = 0;
        input = new std::string[size];
        while ((j < size) && (getline(in, line))) {
            input[j] = line;
            j++;
        }
    }
    else {
        return EXIT_FAILURE;
    }

    MSG msg{};
    HWND hWnd{};
    WNDCLASSEX wc{ sizeof(WNDCLASSEX) };

    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"MyAppClass";
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW; //!!!

    if (!RegisterClassEx(&wc))
        return EXIT_FAILURE;

    if ((hWnd = CreateWindow(wc.lpszClassName, L"AAaaAa",
        WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, nullptr, nullptr,
        wc.hInstance, nullptr)) == INVALID_HANDLE_VALUE)
        return EXIT_FAILURE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (msg.wParam);
}

LRESULT CALLBACK OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
    RECT rect = { 0 };
    rect.right = minClientRectWidth;
    rect.bottom = minClientRectHeight;
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, FALSE);
    
    minMaxInfo->ptMinTrackSize.x = rect.right;
    minMaxInfo->ptMinTrackSize.y = rect.bottom - rect.top; //????? wtf

    return EXIT_SUCCESS;
}

VOID OnPaint(HWND hWnd) {
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect, clientRect;
    // count offsets
    GetClientRect(hWnd, &clientRect);
    int* offsetX = new int[M + 1];
    offsetX[0] = 0;

    for (int i = 1, increment = clientRect.right / M; i < M + 1; i++) {
        offsetX[i] = offsetX[i - 1] + increment;
    }
    int* offsetY = new int[N + 1]{ 0 };
    
    hdc = BeginPaint(hWnd, &ps);
    // first draw text
    // then draw table
    for (int i = 0; i < N; i++) {
        int maxHeight = 0;
        // they are same for each rectangle in a row
        rect.top = offsetY[i];
        rect.bottom = offsetY[i] + 1;
        for (int j = 0; j < M; j++) {
            // get widestring from usual string
            std::wstring bufWStr(input[i * M + j].begin(), input[i * M + j].end());
            // convert widestring to wchar_t
            LPCWSTR str = bufWStr.c_str();
            // init rectangle for textblock
            rect.left = offsetX[j];
            rect.right = offsetX[j + 1];

            int height = DrawText(hdc, str, bufWStr.length(), &rect, DT_EDITCONTROL | DT_NOCLIP | DT_WORDBREAK);
            if (height > maxHeight) {
                maxHeight = height;
            }
        }
        offsetY[i + 1] = offsetY[i] + maxHeight;
        
    }

    // offsetY
    for (int i = 0; i < N + 1; i++) {
        MoveToEx(hdc, 0, offsetY[i], nullptr);
        LineTo(hdc, offsetX[M], offsetY[i]);
    }
    // offsetX
    for (int i = 0; i < M + 1; i++) {
        MoveToEx(hdc, offsetX[i], 0, nullptr);
        LineTo(hdc, offsetX[i], offsetY[N]);
    }

    EndPaint(hWnd, &ps);

    minClientRectHeight = offsetY[N];
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case WM_DESTROY:
        PostQuitMessage(EXIT_SUCCESS);//!!!
        break;

  /*  case WM_LBUTTONDOWN: why it doesnt work without WM_PAINT switch??? is it matter what event generate an msg?
        OnPaint(hWnd);
        break;*/

    case WM_PAINT:
        OnPaint(hWnd); 
        break;

    case WM_GETMINMAXINFO:
        return OnGetMinMaxInfo(hWnd, wParam, lParam);
        break;

    default:
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

