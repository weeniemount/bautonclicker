#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include "resource/resource.h"
#include <mmsystem.h>

#define GRID_SIZE 16
#define BUTTON_SIZE 16

// Global variables
HINSTANCE hInst;
bool buttonPressed[GRID_SIZE * GRID_SIZE] = {false};
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool deletebutton = true;

// WinMain function (entry point)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInst = hInstance;

	// Register the main window class
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "buttonclicker";
	wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

	#define EXIT 128
	#define NEW_GAME 129
    #define ABOUT 256
    #define HELP_TOPICS 260
	#define BUTTON_DELETE 261

    HMENU menu = CreateMenu();
    HMENU game = CreateMenu();
	HMENU options = CreateMenu();
    HMENU help = CreateMenu();
    

    AppendMenu(menu, MF_POPUP, (UINT_PTR)game, "gam");
	AppendMenu(menu, MF_POPUP, (UINT_PTR)options, "optionz");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)help, "hlep");

    AppendMenu(game, MF_STRING, NEW_GAME, "new gam");
	AppendMenu(game, MF_SEPARATOR, 0, NULL);  // Divider
    AppendMenu(game, MF_STRING, EXIT, "exitearino");
    AppendMenu(help, MF_STRING, HELP_TOPICS, "help topicals...");
    AppendMenu(help, MF_STRING, ABOUT, "abaut");
	AppendMenu(options, MF_STRING, BUTTON_DELETE, "toggle deleting button");

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, "Window registration failed!", "Error", MB_OK);
		return 1;
	}

	// Create the main application window
	HWND hWnd = CreateWindow(
		"buttonclicker",
		"bauton clicker",
		WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,
		GRID_SIZE * BUTTON_SIZE + 6, // Width
		GRID_SIZE * BUTTON_SIZE + 39 + 10, // Height
		NULL, menu, hInstance, NULL
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

void PlayResourceSound(HINSTANCE hInstance, int resourceID) {
    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (hResource) {
        HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
        if (hLoadedResource) {
            LPVOID pResourceData = LockResource(hLoadedResource);
            if (pResourceData) {
                PlaySound((LPCSTR)pResourceData, NULL, SND_MEMORY | SND_ASYNC);
                // Unlock the resource after use
                UnlockResource(hLoadedResource);
            }
            FreeResource(hLoadedResource); // Free the resource after use
        }
    }
}


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

 			buttonPressed[buttonID] = true;

            // Check if all buttons are pressed
            bool allPressed = true;
            for (int i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
                if (!buttonPressed[i]) {
                    allPressed = false;
                    break;
                }
            }

			// Disable the button
			if (hButton) {
				PlayResourceSound(hInst, IDR_POPSFX);
				if (deletebutton) {
					DestroyWindow(hButton);
				} else {
					EnableWindow(hButton, FALSE);
					// Change button appearance to "held down"
					SetWindowLong(hButton, GWL_STYLE, GetWindowLong(hButton, GWL_STYLE) | BS_FLAT);
					SendMessage(hButton, WM_CTLCOLORBTN, (WPARAM)GetSysColorBrush(COLOR_BTNFACE), 0);
					InvalidateRect(hButton, NULL, TRUE); // Force redraw
				}
			}

            if (allPressed) {
                MessageBox(hWnd, "u pressed all ze buttons!", "congrat", MB_OK);
            }

			switch (LOWORD(wParam)) {
				case EXIT:
                    PostQuitMessage(0);
                    break;
                case ABOUT:
                    MessageBox(hWnd, "just a funny game to click buttons\nmade by boinkwer to push", "abaut button pushher", MB_OK | MB_ICONINFORMATION);
                    break;
				case BUTTON_DELETE:
					if (deletebutton == true) {
						deletebutton = false;
					} else {
						deletebutton = true;
					}
					break;
				case NEW_GAME:
                    // Remove all buttons
                    for (int i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
                        DestroyWindow(GetDlgItem(hWnd, i));
                    }
                    // Clear the buttonPressed array
                    memset(buttonPressed, 0, sizeof(buttonPressed));

                    // Recreate the grid of buttons
                    for (int row = 0; row < GRID_SIZE; ++row) {
                        for (int col = 0; col < GRID_SIZE; ++col) {
                            HWND hButton = CreateWindow(
                                "BUTTON", 
                                "", 
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
				case HELP_TOPICS:
                    {
                        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(INT_HELPCHM), RT_RCDATA);
                        if (hResInfo) {
                            HGLOBAL hRes = LoadResource(NULL, hResInfo);
                            if (hRes) {
                                DWORD resSize = SizeofResource(NULL, hResInfo);
                                LPVOID pResData = LockResource(hRes);

                                if (pResData) {
                                    TCHAR tempPath[MAX_PATH];
                                    GetTempPath(MAX_PATH, tempPath);
                                    TCHAR tempFile[MAX_PATH];
                                    GetTempFileName(tempPath, TEXT("CHM"), 0, tempFile);

                                    HANDLE hFile = CreateFile(tempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
                                    if (hFile != INVALID_HANDLE_VALUE) {
                                        DWORD bytesWritten;
                                        WriteFile(hFile, pResData, resSize, &bytesWritten, NULL);
                                        CloseHandle(hFile);

                                        // Rename .tmp to .chm
                                        TCHAR newFilePath[MAX_PATH];
                                        wsprintf(newFilePath, TEXT("%s.chm"), tempFile);
                                        if (MoveFile(tempFile, newFilePath)) {
                                            // Open the CHM file
                                            ShellExecute(hWnd, TEXT("open"), newFilePath, NULL, NULL, SW_SHOWNORMAL);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
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