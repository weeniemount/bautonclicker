#include <windows.h>

#define GRID_SIZE 16
#define BUTTON_SIZE 16

// Global variables
HINSTANCE hInst;

// Callback function for the main window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE: {
			// Create a grid of buttons
			for (int row = 0; row < GRID_SIZE; ++row) {
				for (int col = 0; col < GRID_SIZE; ++col) {
					HWND hButton = CreateWindow(
						"BUTTON", 
						"", // No text
						WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
						col * BUTTON_SIZE, 
						row * BUTTON_SIZE, 
						BUTTON_SIZE, 
						BUTTON_SIZE, 
						hWnd, 
						(HMENU)(row * GRID_SIZE + col), 
						hInst, 
						NULL
					);
					if (!hButton) {
						MessageBox(NULL, "Button creation failed!", "Error", MB_OK);
					}
				}
			}
			break;
		}
		case WM_COMMAND: {
			// Get the button ID
			int buttonID = LOWORD(wParam);
			HWND hButton = (HWND)lParam;

			// Disable the button
			if (hButton) {
				EnableWindow(hButton, FALSE);
				// Change button appearance to "held down"
				SetWindowLong(hButton, GWL_STYLE, GetWindowLong(hButton, GWL_STYLE) | BS_FLAT);
				InvalidateRect(hButton, NULL, TRUE); // Force redraw
			}
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// WinMain function (entry point)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInst = hInstance;

	// Register the main window class
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "buttonclicker";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, "Window registration failed!", "Error", MB_OK);
		return 1;
	}

	// Create the main application window
	HWND hWnd = CreateWindow(
		"buttonclicker",
		"bauton clicker",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		GRID_SIZE * BUTTON_SIZE + 16, // Width
		GRID_SIZE * BUTTON_SIZE + 39, // Height
		NULL, NULL, hInstance, NULL
	);

	if (!hWnd) {
		MessageBox(NULL, "Window creation failed!", "Error", MB_OK);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
