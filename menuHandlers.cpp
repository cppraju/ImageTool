/*
some menu handlers are writen here as File, Edit, all these functions are specific to this application

*/

#include "stdafx.h"
#include "image_functions.h"
#include "menuHandlers.h"


extern HWND hWnd;

//ideally the action that displays a BITMAP in the claient area should update these two global variables right after the action
extern HBITMAP hBitmapG;
extern HPALETTE hPalG;
extern imageType IMAGE;

extern char strOpenFileName[256];			//name of the currently open file name
extern bool IsDirty;								//flag to determine if the currently displayed image is altered after the last save
extern char szFileTitle[128];					//title of the opened file
extern char szInitDir[256];						//last opened directory

bool CopyToCB(void)
{
	if( (!hBitmapG) || (!hPalG) )
		return(false);

	if(IMAGE == DDB)
		CopyDDBToCB(hWnd, hBitmapG, hPalG);
	else if(IMAGE == DIB)
		CopyDIBToCB(hWnd, hBitmapG, hPalG);

	return(true);
}

bool PaintClientWnd(HDC hDC, PAINTSTRUCT &ps)
{
	RECT recC, recB;
	BITMAP bm;
	DIBSECTION ds;
	double rx, ry;					//ratio of client rec to bitmap rec
	
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "PaintClientWnd 1-------------------------------------" );
		OutputDebugStringA(xxx);
	}

	if( (!hBitmapG) || (!hPalG) )
		return(false);
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "PaintClientWnd 2-------------------------------------" );
		OutputDebugStringA(xxx);
	}

	::GetClientRect(hWnd, &recC);
	
	//getting the bitmap dimensions because this will change base on the feature of this app used
	if(IMAGE == DDB)
	{
		::SetLastError(0);
		::GetObject(hBitmapG, sizeof(BITMAP), &bm);
	{
		DWORD dwErr = ::GetLastError();
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "PaintClientWnd GetObject last Err: % d BM Width: %d  BM HEIGHT: %d", dwErr, bm.bmWidth, bm.bmHeight );//ds.dsBm.bmWidth, ds.dsBm.bmHeight
		OutputDebugStringA(xxx);
	}

		recB.left = recB.top = 0;
		recB.right = bm.bmWidth;
		recB.bottom = bm.bmHeight;
	}
	else if(IMAGE ==DIB)
	{
		LPBYTE pByte = NULL;
		pByte = (LPBYTE) ::GlobalLock(hBitmapG);
		BITMAPINFOHEADER bih = *(BITMAPINFOHEADER *)pByte;

	{
		DWORD dwErr = ::GetLastError();
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "PaintClientWnd GetObject last Err: % d DS Width: %d  DS HEIGHT: %d", dwErr, bih.biWidth, bih.biHeight );
		OutputDebugStringA(xxx);
	}

		recB.left = recB.top = 0;
		recB.right = bih.biWidth;
		recB.bottom = bih.biHeight;
		::GlobalUnlock(hBitmapG);
	}
	{
		DWORD dwErr = ::GetLastError();
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "PaintClientWnd GetObject last Err: % d PS Width: %d  PS HEIGHT: %d", dwErr, ps.rcPaint.right, ps.rcPaint.bottom );//ds.dsBm.bmWidth, ds.dsBm.bmHeight
		OutputDebugStringA(xxx);
	}
	
/*	rx = (recB.right - recB.left) / (recC.right - recC.left);
//	double a = (recB.right - recB.left) % (recC.right - recC.left);
//	a = a /10;
//	rx = rx +  a;  //% operator returning only upto 1 decimal place

	ry = (recB.bottom - recB.top) / (recC.bottom - recC.top);
//	ry = ry + ( (recB.bottom - recB.top) / (recC.bottom - recC.top) ) / 10;
	
	recD.left = (LONG) rx * ps.rcPaint.left;
	recD.top = (LONG) ry * ps.rcPaint.top;
	recD.right = (LONG) rx * ps.rcPaint.right;
	recD.bottom = (LONG) ry * ps.rcPaint.bottom;
*/
	if(IMAGE == DDB)
		return(PaintDDBitmap(hDC, &ps.rcPaint, hBitmapG, &recB, hPalG));
	else if(IMAGE == DIB)
		return(PaintDIBitmap(hDC, &ps.rcPaint, hBitmapG, &recB, hPalG));
	else
		return(false);
}

bool SaveDisplayedImage(void)
{
	
	OPENFILENAME ofn;
	char strFilter[512], strDefExt[20];

	if( IsDirty && hBitmapG && hPalG)
	{
		//check to see if we already have a name
		if(strlen(strOpenFileName) == 0)
		{
			//get the file name
			memset(&ofn, 0, sizeof(OPENFILENAME));
		//	sprintf_s(strFilter, sizeof(char)*256, "Monochrome Bitmap (*.bmp, *.dib)\0 *.bmp;*.dib\016 Color Bitmap   (*.bmp, *.dib)\0 *.bmp;*.dib\0256 Color Bitmap   (*.bmp, *.dib)\0 *.bmp;*.dib\024-Bit Bitmap  (*.bmp, *.dib)\0 *.bmp;*.dib\0Icon  (*.ico)\0  *.ico\0Cursor  (*.cur)\0 *.cur\0\0");
			memcpy_s(strFilter, 512, "Monochrome Bitmap (*.bmp, *.dib)\0 *.bmp\0 16 Color Bitmap   (*.bmp, *.dib)\0 *.bmp\0 256 Color Bitmap   (*.bmp, *.dib)\0 *.bmp\0 24-Bit Bitmap  (*.bmp, *.dib)\0 *.bmp\0 Icon  (*.ico)\0  *.ico\0 Cursor  (*.cur)\0 *.cur\0\0", 300);
			::GetCurrentDirectory(sizeof(szInitDir), szInitDir);
			ofn.lStructSize = sizeof(OPENFILENAME); 
			ofn.hwndOwner = hWnd; 
			ofn.lpstrFilter = strFilter; 
			ofn.nFilterIndex = 4;
			lstrcpy(strOpenFileName, _T("Untitled.bmp"));
			ofn.lpstrFile= strOpenFileName; 
			ofn.nMaxFile = sizeof(char[256]); 
			ofn.lpstrFileTitle = szFileTitle; 
			ofn.nMaxFileTitle = sizeof(szFileTitle); 
			ofn.lpstrInitialDir = szInitDir; 
			lstrcpy(strDefExt, "bmp");  
			ofn.lpstrDefExt = strDefExt;
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT|OFN_FORCESHOWHIDDEN|OFN_LONGNAMES|OFN_PATHMUSTEXIST; 
			::GetSaveFileName(&ofn); 

		}
	}

		switch(IMAGE)
		{
		case DDB:


			break;
		case DIB:

			break;
		case ICON:

			break;
		case CUR:

			break;
		}



	return(true);
}

void test(void)
{
	HBITMAP hbmDIB = NULL, hbmDDB = NULL;
	HDC hdc;
	RECT rec;
	::GetClientRect(hWnd, &rec);

	hdc = ::GetDC(hWnd);
	hbmDIB = CopyDDBToDIB(hdc, hBitmapG, hPalG);
//	CopyDIBToCB(hWnd, hbmDIB, hPalG);

	::DeleteObject(hBitmapG);
	hBitmapG = NULL;
	//hBitmapG = CopyDIBToDDB1(hdc, hbmDIB, hPalG);
	//CopyDDBToCB(hWnd, hbmDDB, hPalG);
	//CopyDIBToCB(hWnd, hbmDIB, hPalG);
	
	InvalidateRect(hWnd, &rec, TRUE);
	//cleanup
	::GlobalFree(hbmDIB);
	::ReleaseDC(hWnd, hdc);


}

/*
	This function will create a memory DC and will draw on that using various fonts, pens and brushes
	After that save that memory image to disk using GDI+ and also display that in the client area

	Takes no argument
*/
void DrawOnMemDC(void)
{

}