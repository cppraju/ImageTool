#include "stdafx.h"

#include "ScreenCapture.h"
#include "image_functions.h"


//HCURSOR hcur, hcurOld;
extern HBITMAP hBitmapG = NULL;
extern HPALETTE hPalG = NULL;
imageType IMAGE;



bool CaptureScreen(HWND hWnd)
{
	HBITMAP hbmOld1, hbmScreen;
	HDC hDCScreen, hDCClient, hDCCompatible;
	PBYTE pBytes;
	RECT client, screen;
	BITMAPINFO bmif;

	//client and the screen coordinates
	::GetClientRect(hWnd, &client);

	//minimizing this window
	::SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, -1);
	::ShowWindow(hWnd, SW_HIDE);

	memset(&screen, 0, sizeof(RECT));
	//getting the screen DC
	hDCScreen = ::CreateDC("DISPLAY", NULL, NULL, NULL);
	screen.right = ::GetDeviceCaps(hDCScreen, HORZRES);
	screen.bottom = ::GetDeviceCaps(hDCScreen, VERTRES);

	//creating the memory DC
	hDCCompatible = ::CreateCompatibleDC(hDCScreen);
	hbmScreen = ::CreateCompatibleBitmap(hDCScreen, screen.right, screen.bottom);
	hbmOld1 = (HBITMAP)::SelectObject(hDCCompatible, hbmScreen);
		
	//now drawing the screen on the memory bitmap
	::BitBlt(hDCCompatible, 0, 0, screen.right, screen.bottom, hDCScreen, 0, 0, SRCCOPY);

	//getting the client DC and the bitmap
	hDCClient = ::GetDC(hWnd);
	

//	::StretchBlt(hDCClient, 0, 0, client.right , client.bottom, hDCCompatible, 0, 0, screen.right, screen.bottom, SRCCOPY);
	//now we will use the ::GetDiBits to get the bitmap bits of the DDB alternatively we could hae also used ::Stretchblt
	//so selecting the memory bitmap out of device context
	memset(&bmif, 0, sizeof(BITMAPINFO));
	bmif.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	::SelectObject(hDCCompatible, hbmOld1);		//selecting the memory bitmap out of the DC as per MSDN
	::GetDIBits(hDCCompatible, hbmScreen, 0, screen.bottom, NULL, &bmif, DIB_RGB_COLORS);
	
	//now we got the size of the bitmap data size
	pBytes = (PBYTE)::GlobalAlloc(GPTR, bmif.bmiHeader.biSizeImage);
	//now call the ::GetDIBits for the 2nd time
	::GetDIBits(hDCCompatible, hbmScreen, 0, screen.bottom, pBytes, &bmif, DIB_RGB_COLORS);

	//before displaying restoring the app window
	::ShowWindow(hWnd, SW_SHOW);
	::SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, -1);

	::StretchDIBits(hDCClient, 0, 0, client.right, client.bottom, 0, 0, screen.right, screen.bottom, pBytes, &bmif, DIB_RGB_COLORS, SRCCOPY);
	
	//////saving the captured bitmap handle///////////////////////////////////////////////////////////
	//we are now saving directly to the hBitmapG to save some extra work
	
	hPalG = GetDCPalette(hDCClient);
	if(hPalG == NULL)
		hPalG = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
	hBitmapG = hbmScreen;
	hbmScreen = NULL;

	///////////////////////////////
	//cleanup
	
	
	::ReleaseDC(hWnd, hDCClient);		//got by calling GetDC()
	::DeleteDC(hDCCompatible);			//got by CreateCompatibleDC()
	::DeleteDC(hDCScreen);				//got by Create
	::DeleteObject(hbmScreen);
	::GlobalFree(pBytes);
	IMAGE = DDB;
	return(true);
}

//this is just a screen capture using the basic BitBlt/StretchBlt
bool copyscreen(HWND hWnd)
{
	HBITMAP hbmScreen, hbmClient, hbmOld1, hbmOld2;
	HDC hDCScreen, hDCClient, hDCCompatible;
	
	RECT client, screen;
	

	//client and the screen coordinates
	::GetClientRect(hWnd, &client);

	//getting the client DC and the bitmap
	hDCClient = ::GetDC(hWnd);
	hbmClient = ::CreateCompatibleBitmap(hDCClient, client.right, client.bottom);
	hbmOld2 = (HBITMAP)::SelectObject(hDCClient, hbmClient);
	::SelectObject(hDCClient, hbmOld2);

	//minimizing this window
	::SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, -1);
	::ShowWindow(hWnd, SW_HIDE);

	memset(&screen, 0, sizeof(RECT));
	//getting the screen DC
	hDCScreen = ::CreateDC("DISPLAY", NULL, NULL, NULL);
	screen.right = ::GetDeviceCaps(hDCScreen, HORZRES);
	screen.bottom = ::GetDeviceCaps(hDCScreen, VERTRES);

	//creating the memory DC
	hDCCompatible = ::CreateCompatibleDC(hDCScreen);
	hbmScreen = ::CreateCompatibleBitmap(hDCScreen, screen.right, screen.bottom);
	hbmOld1 = (HBITMAP)::SelectObject(hDCCompatible, hbmScreen);
		
	//now drawing the screen on the memory bitmap
	::BitBlt(hDCCompatible, 0, 0, screen.right, screen.bottom, hDCScreen, 0, 0, SRCCOPY);


	
	::ShowWindow(hWnd, SW_SHOW);
	::SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, -1);
	//::BitBlt(hDCClient, 0, 0, client.right , client.bottom, hDCScreen, 0, 0, SRCCOPY);
	::StretchBlt(hDCClient, 0, 0, client.right , client.bottom, hDCCompatible, 0, 0, screen.right, screen.bottom, SRCCOPY);
	


//	hBitmapG = CopyDDBToDIB(hDCScreen, hbmClient);
//	CopyDIBToCB(hWnd, hBitmapG, hPalG);
	CopyDDBToCB(hWnd, hbmOld2, hPalG);

//	::SelectObject(hDCClient, hbmOld2);
	::SelectObject(hDCCompatible, hbmOld1);

	//cleanup
	::ReleaseDC(hWnd, hDCClient);		//got by calling GetDC()
	::DeleteDC(hDCCompatible);			//got by CreateCompatibleDC()
	::DeleteDC(hDCScreen);				//got by Create
	::DeleteObject(hbmClient);
	::DeleteObject(hbmScreen);

	return(true);
}