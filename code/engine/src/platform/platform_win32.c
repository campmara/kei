#include "platform/kei_platform.h"

#if KEI_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "core/kei_logger.h"
#include "core/kei_input.h"

typedef struct internal_state {
    HINSTANCE h_instance;
    HWND hwnd;
} internal_state;

// Clock
static float64 clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32 msg, WPARAM w_param, LPARAM l_param);

bool8 kei_platform_initialize(platform_state *p_state,
                              const char *application_name,
                              int32 x,
                              int32 y,
                              int32 width,
                              int32 height) {
    p_state->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)p_state->internal_state;

    state->h_instance = GetModuleHandleA(0);

    // Setup and register window class
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; // Get double-clicks
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL; // Manage the cursor manually
    wc.hbrBackground = NULL;                  // Transparent
    wc.lpszClassName = "kei_window_class";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Create window
    uint32 client_x = x;
    uint32 client_y = y;
    uint32 client_width = width;
    uint32 client_height = height;

    uint32 window_x = client_x;
    uint32 window_y = client_y;
    uint32 window_width = client_width;
    uint32 window_height = client_height;

    uint32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    uint32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Obtain the size of the border
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // In this case, the border rectangle is negative
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Grow by the size of the OS border
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(window_ex_style,
                                  "kei_window_class",
                                  application_name,
                                  window_style,
                                  window_x,
                                  window_y,
                                  window_width,
                                  window_height,
                                  0,
                                  0,
                                  state->h_instance,
                                  0);

    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        KEI_FATAL("Window creation failed!");
        return FALSE;
    } else {
        state->hwnd = handle;
    }

    // Show the window
    bool32 should_activate = 1; // TODO: if the window should not accept input, this should be false
    int32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
    ShowWindow(state->hwnd, show_window_command_flags);

    // Clock setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (float64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);

    return TRUE;
}

void kei_platform_shutdown(platform_state *p_state) {
    // Simply cold-cast to the known type
    internal_state *state = (internal_state *)p_state->internal_state;

    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

bool8 kei_platform_pump_messages(platform_state *p_state) {
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return TRUE;
}

void *kei_platform_memory_alloc(uint64 size, bool8 is_aligned) {
    return malloc(size);
}

void kei_platform_memory_free(void *block, bool8 is_aligned) {
    free(block);
}

void *kei_platform_memory_zero(void *block, uint64 size) {
    return memset(block, 0, size);
}

void *kei_platform_memory_copy(void *dest, const void *source, uint64 size) {
    return memcpy(dest, source, size);
}

void *kei_platform_memory_set(void *dest, int32 value, uint64 size) {
    return memset(dest, value, size);
}

void kei_platform_console_write(const char *message, uint8 color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static uint8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void kei_platform_console_write_error(const char *message, uint8 color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static uint8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

float64 kei_platform_get_absolute_time() {
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (float64)now_time.QuadPart * clock_frequency;
}

void kei_platform_sleep(uint64 ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32 msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_ERASEBKGND:
            // Notify the OS that erasing will be handled by the application to prevent flicker
            return 1;
        case WM_CLOSE:
            // TODO: fire an event for the application to quit
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            // Get the updated size.
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: Fire an event for window resize.
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // Key pressed / released
            bool8 is_pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            keys key = (uint16)w_param;
            kei_input_process_key(key, is_pressed);
        } break;
        case WM_MOUSEMOVE: {
            // Mouse move
            int32 x_pos = GET_X_LPARAM(l_param);
            int32 y_pos = GET_Y_LPARAM(l_param);
            kei_input_process_mouse_move(x_pos, y_pos);
        } break;
        case WM_MOUSEWHEEL: {
            int32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                // Flatten the input to an OS-independent (-1, 1)
                z_delta = (z_delta < 0) ? -1 : 1;
                kei_input_process_mouse_wheel(z_delta);
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            // b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            //  TODO: input processing
            bool8 is_pressed =
                msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            buttons mouse_button = BUTTON_MAX_BUTTONS;
            switch (msg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouse_button = BUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouse_button = BUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouse_button = BUTTON_RIGHT;
                    break;
            }

            if (mouse_button != BUTTON_MAX_BUTTONS) {
                kei_input_process_button(mouse_button, is_pressed);
            }
        } break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif