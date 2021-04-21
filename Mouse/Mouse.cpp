// Mouse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	return 0;
//}

#include <windows.h>
#include <stdio.h>

HANDLE hStdin;
DWORD fdwSaveOldMode;

VOID ErrorExit(LPSTR);
VOID KeyEventProc(KEY_EVENT_RECORD);
VOID MouseEventProc(MOUSE_EVENT_RECORD);
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD);



//typedef struct _INPUT_RECORD {
//    WORD EventType;
//    union {
//        KEY_EVENT_RECORD KeyEvent;
//        MOUSE_EVENT_RECORD MouseEvent;
//        WINDOW_BUFFER_SIZE_RECORD WindowBufferSizeEvent;
//        MENU_EVENT_RECORD MenuEvent;
//        FOCUS_EVENT_RECORD FocusEvent;
//    } Event;
//} INPUT_RECORD, *PINPUT_RECORD;

//typedef struct _MOUSE_EVENT_RECORD {
//    COORD dwMousePosition;
//    DWORD dwButtonState;
//    DWORD dwControlKeyState;
//    DWORD dwEventFlags;
//} MOUSE_EVENT_RECORD, *PMOUSE_EVENT_RECORD;

//typedef struct _COORD {
//    SHORT X;
//    SHORT Y;
//} COORD, *PCOORD;


int main(VOID)
{
    DWORD cNumRead, fdwMode, i;
    INPUT_RECORD irInBuf[4096];
    int counter=0;

    // Get the standard input handle.

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
        ErrorExit("GetStdHandle");

    // Save the current input mode, to be restored on exit.

    if (! GetConsoleMode(hStdin, &fdwSaveOldMode) )
        ErrorExit("GetConsoleMode");

    // Enable the window and mouse input events.

    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    if (! SetConsoleMode(hStdin, fdwMode) )
        ErrorExit("SetConsoleMode");

    // Loop to read and handle the next 100 input events.

    while (counter++ <= 4000)
    {
        // Wait for the events.

        if (! ReadConsoleInput(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead) ) // number of records read
            ErrorExit("ReadConsoleInput");

        // Dispatch the events to the appropriate handler.

        for (i = 0; i < cNumRead; i++)
        {
            switch(irInBuf[i].EventType)
            {
                case KEY_EVENT: // keyboard input
                    KeyEventProc(irInBuf[i].Event.KeyEvent);
                    break;

                case MOUSE_EVENT: // mouse input
                    MouseEventProc(irInBuf[i].Event.MouseEvent);
                    break;

                case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
                    ResizeEventProc( irInBuf[i].Event.WindowBufferSizeEvent );
                    break;

                case FOCUS_EVENT:  // disregard focus events

                case MENU_EVENT:   // disregard menu events
                    break;

                default:
                    ErrorExit("Unknown event type");
                    break;
            }
        }
    }

    // Restore input mode on exit.

    SetConsoleMode(hStdin, fdwSaveOldMode);

    return 0;
}

VOID ErrorExit (LPSTR lpszMessage)
{
    fprintf(stderr, "%s\n", lpszMessage);

    // Restore input mode on exit.

    SetConsoleMode(hStdin, fdwSaveOldMode);

    ExitProcess(0);
}

VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
    printf("Key event: ");

    if(ker.bKeyDown)
        printf("key pressed\n");
    else printf("key released\n");
}

VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
   // printf("Mouse event: ");
	printf("x%d y%d\n", mer.dwMousePosition.X, mer.dwMousePosition.Y);
	printf("kS%XeF%x bS%X\n", mer.dwControlKeyState, mer.dwEventFlags, mer.dwButtonState );
    switch(mer.dwEventFlags)
    {
		
        case 0:

            if(mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
                printf("<");
            else if(mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
                printf(">");
            else
                printf("button press\n");
            break;
        case DOUBLE_CLICK:
            printf("2");
            break;
        case MOUSE_HWHEELED:
            printf("o");
            break;
        case MOUSE_MOVED:
            printf("--");
            break;
        case MOUSE_WHEELED:
            printf("v");
            break;
        default:
            printf("unknown\n");
            break;
    }
}

VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    printf("Resize event\n");
    printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
}