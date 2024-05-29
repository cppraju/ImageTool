/*
class and functions for capturing the screen
*/

#ifndef _SCREEN_CAPTURE
#define _SCREEN_CAPTURE




/*
	The basic screen capture function assumes that there is a valid HWND hWnd puts the capture on the client area
	returns bool
*/
bool CaptureScreen(HWND hWnd);

bool copyscreen(HWND hwnd);


#endif