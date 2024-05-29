/*
some menu handlers are writen here as File, Edit, all these functions are specific to this application

*/


/*
	Simply copies the global BM and pal to CB
*/
bool CopyToCB(void);

/*
	This is a wrapper function for prepairing the actual paint functions
	hDC - DC for painting
	ps - PAINTSTRUCT as prepared by BeginPaint()
*/
bool PaintClientWnd(HDC hDC, PAINTSTRUCT &ps);

/*
	This is a general save function
	Basic assumtions:
			the global BITMAP/ICON/CUR & PALETTE handle is valid 

*/
bool SaveDisplayedImage(void);

/*

	just a function tester
*/

void test(void);

/*
	This function will create a memory DC and will draw on that using various fonts, pens and brushes
	After that save that memory image to disk using GDI+ and also display that in the client area

	Takes no argument
*/
void DrawOnMemDC(void);