#include "stdafx.h"
#include "image_functions.h"


//extern HWND hWnd;

//ideally the action that displays a BITMAP in the claient area should update these two global variables right after the action
//extern HBITMAP hBitmapG;
//extern HPALETTE hPalG;

/*
#ifdef __cplusplus
	extern "C" {
#endif 
*/

UINT DIBNoOfPalColors(LPBYTE lpDIB)
{
	BITMAPINFOHEADER bi;
	BITMAPCOREHEADER bc;

	UINT uBitCnt = 0, uNoCol = 0;

	if(IS_WIN30_DIB(lpDIB))
	{
		bi = *(BITMAPINFOHEADER *) lpDIB;
		if(bi.biClrUsed != 0)
			return(bi.biClrUsed);
		uBitCnt = bi.biBitCount;
	}
	else
	{
		bc = *(BITMAPCOREHEADER *) lpDIB;
		uBitCnt = bc.bcBitCount;
	}

	if(uBitCnt == 1)
		uNoCol = 2;
	else if(uBitCnt == 4 )
		uNoCol = 16;
	else if(uBitCnt == 8)
		uNoCol = 256;
	else 
		uNoCol = 0;

	return(uNoCol);
}

HBITMAP CopyDIBToDDB(HDC hDC, HBITMAP hDib, HPALETTE hPal)
{
	HBITMAP hbm = NULL, hbmOld;
	HDC hMemDC = NULL;
	BITMAPINFO bmi;
	LPBYTE lpDib = NULL, lpDIBBits = NULL;
	HPALETTE hPalOld;
	
	hMemDC = ::CreateCompatibleDC(hDC);
	
	::SetLastError(0);
	lpDib = (LPBYTE)::GlobalLock(hDib);
	if( !lpDib )
	{
		{
			char xxx[128];
			sprintf_s(xxx, sizeof(char)*128, "CopyDIBToDDB calling GlobalLock Last Err = %d ", ::GetLastError());
			OutputDebugStringA(xxx);
		}
		return(NULL);
	}
	memset(&bmi, 0, sizeof(BITMAPINFO));
	memcpy_s(&bmi, sizeof(BITMAPINFO), (LPVOID)lpDib, sizeof(BITMAPINFO));
	
	
	lpDIBBits = lpDib + *(DWORD *)lpDib + GetDIBPaletteSize(lpDib);
	

	//create the compatible bitmap and select that into the memdc
	hbm = ::CreateCompatibleBitmap(hDC, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);
	hbmOld = (HBITMAP)::SelectObject(hMemDC, hbm);

	if(hPal == NULL)
		hPal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);


//	hPal = (HPALETTE)::SelectPalette(hMemDC, hPal, FALSE);
//	::RealizePalette(hMemDC);
//	hPalOld = (HPALETTE)::SelectPalette(hDC, hPal, FALSE);
//	::RealizePalette(hDC);

	::SetLastError(0);
	::SetDIBits(hMemDC, hbm, 0, bmi.bmiHeader.biHeight, lpDIBBits, &bmi, DIB_RGB_COLORS);
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "CopyDIBToDDB calling SetDIBits Last Err = %d ", ::GetLastError());
		OutputDebugStringA(xxx);
	}

	//cleanup
	::GlobalUnlock(hDib);
	::SelectObject(hMemDC, hbmOld);
//	::SelectPalette(hMemDC, hPal, TRUE);
//	::RealizePalette(hMemDC);
	::DeleteDC(hMemDC);

	return(hbm);
}

HBITMAP CopyDIBToDDB(const BITMAPINFOHEADER *pbmih, HDC hDC, HBITMAP hDib, HPALETTE hPal )
{
	HBITMAP hbm = NULL;
	 




	return(hbm);
}


HBITMAP CopyDDBToDIB(HDC hDC, HBITMAP hbm, HPALETTE hPal, UINT start, UINT cLines)
{
	HANDLE hDib = NULL;
	BITMAP bm;
	DWORD  dwErr, dwPalSize;
	PBYTE pBytes;
	BITMAPINFO bmif;
	BYTE bmBits;


	pBytes = NULL;

	memset(&bm, 0, sizeof(BITMAP));
	memset(&bmif, 0, sizeof(BITMAPINFO));
	//checking for any HBITMAP already allocated

	//getting the size of the bitmap
	
	::GetObject(hbm, sizeof(BITMAP), &bm);

	//size of the BITMAP bits in bytes 
	
	if(bm.bmBitsPixel == 1)
		bmBits = 1;
	else if(bm.bmBitsPixel <= 4)
		bmBits = 4;
	else if(bm.bmBitsPixel <= 8)
		bmBits = 8;
	else if(bm.bmBitsPixel <= 16)
		bmBits = 16;
	else if(bm.bmBitsPixel <= 24)
		bmBits = 24;
	else
		bmBits = 32;


	//setting up the BITMAPINFO header
	bmif.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmif.bmiHeader.biHeight = bm.bmHeight;
	bmif.bmiHeader.biWidth = bm.bmWidth;
	bmif.bmiHeader.biPlanes = bm.bmPlanes;
	bmif.bmiHeader.biSizeImage = (bm.bmPlanes * bm.bmWidth * bm.bmHeight * bmBits)/8; //size of the image no if bytes of bitmap data
	bmif.bmiHeader.biBitCount = bmBits;
	if( (bm.bmBitsPixel == 16) || (bm.bmBitsPixel == 32) )
		bmif.bmiHeader.biCompression = BI_RGB;//BI_BITFIELDS;
	else if(bm.bmBitsPixel == 24)
		bmif.bmiHeader.biCompression = BI_RGB;
	
	//getting palette ready and selected into the DC from which the BITMAP is to be copied
	if(hPal == NULL)
		hPal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);			//a bit risky
	hPal = ::SelectPalette(hDC, hPal, FALSE);
	::RealizePalette(hDC);

	if(cLines == 0)
		cLines = bm.bmHeight;
	
	dwPalSize = GetDIBPaletteSize((LPBYTE)&bmif);

	::SetLastError(0);
	hDib = ::GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER) + dwPalSize + bmif.bmiHeader.biSizeImage);
	{
		dwErr = ::GetLastError();
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "CopyDDBToDIB calling GlobalAlloc Last Err = %d ", dwErr);
		OutputDebugStringA(xxx);
	}
	
	pBytes = (LPBYTE)::GlobalLock(hDib);

	//now copy BITMAPINFO data into the allocated buffer
	CopyMemory(pBytes, &bmif, sizeof(BITMAPINFOHEADER));
	
	//write code for copying the palette
	if(dwPalSize > 0)
	{


	}

	//getting the actual bitmap bits but we are not setting up the palette/color table
	::SetLastError(0);
	::GetDIBits(hDC, hbm, start, cLines, (LPVOID)(pBytes + sizeof(BITMAPINFOHEADER) + dwPalSize), (BITMAPINFO *)pBytes, DIB_RGB_COLORS);
	
	{
		dwErr = ::GetLastError();
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "CopyDDBToDIB calling GetDIBits Last Err = %d ", dwErr);
		OutputDebugStringA(xxx);
	}
	
	::GlobalUnlock(hDib);

	//cleanup
	::SelectPalette(hDC, hPal, TRUE);
	RealizePalette(hDC);

	///////////////////////////////////////////

	return((HBITMAP)hDib);
}

bool CopyDDBToCB(HWND hWnd, HBITMAP hbm, HPALETTE hPal)
{
	bool bRet = false;
	HANDLE hCB = NULL;
	DWORD dwErr = 0;
	::OpenClipboard(hWnd);
	::EmptyClipboard();
	::SetLastError(0);
	hCB = ::SetClipboardData(CF_BITMAP, hbm);
	dwErr = ::GetLastError();
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "SaveDDBToCB calling SetClipboardData Last Err = %d HANDLE = %0X ", dwErr, hCB);
		OutputDebugStringA(xxx);
	}
	if(hCB)
	{
		bRet = true;
		if(hPal == NULL)
			hPal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
		if(hCB = ::SetClipboardData(CF_PALETTE, hPal))		//Palette is also saved in the CB
			bRet = true;
	}
	::CloseClipboard();
	return(bRet);
}

bool CopyDIBToCB(HWND hWnd, HBITMAP hDib, HPALETTE hPal)
{
	bool bRet = false;
	HANDLE hCB = NULL;
	DWORD dwErr = 0;
	::OpenClipboard(hWnd);
	::EmptyClipboard();
	::SetLastError(0);
	hCB = ::SetClipboardData(CF_DIB, hDib);
	dwErr = ::GetLastError();
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "SaveDIBToCB calling SetClipboardData Last Err = %d HANDLE = %0X ", dwErr, hCB);
		OutputDebugStringA(xxx);
	}
	if(hCB)
	{
		bRet = true;
		if(hPal == NULL)
			hPal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
		if(hCB = ::SetClipboardData(CF_PALETTE, hPal))		////Palette is also saved in the CB
			bRet = true;
	}
	::CloseClipboard();
	return(bRet);
}

HPALETTE GetDCPalette(HDC hDC)
{
	HPALETTE hPal = NULL;
	HANDLE hLogPal = NULL;
	LPLOGPALETTE lpLogPal = NULL;
	UINT uEntry;

	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "GetDCPalette GetSystemPaletteEntries  = %d", ::GetSystemPaletteEntries(hDC, 0, 0, NULL));
		OutputDebugStringA(xxx);
	}

	uEntry = PaletteEntryCountOnDevice(hDC);
	hLogPal = ::GlobalAlloc(GPTR, sizeof(LOGPALETTE) + uEntry * sizeof(PALETTEENTRY));

	if(!hLogPal)
		return(NULL);

	lpLogPal = (LPLOGPALETTE)::GlobalLock(hLogPal);
	lpLogPal->palVersion = PALETTE_VERSION;
	lpLogPal->palNumEntries = uEntry;
	::GetSystemPaletteEntries(hDC, 0, uEntry, lpLogPal->palPalEntry);

	::SetLastError(0);

	hPal = ::CreatePalette(lpLogPal);

	{
		DWORD dwErr = ::GetLastError();
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "GetDCPalette CreatePalette lastErr = %d ",dwErr);
		OutputDebugStringA(xxx);
	}

	//Now we have the HPALETTE so no longer need the LOGPALETTE

	::GlobalUnlock(hLogPal);
	::GlobalFree(hLogPal);

	return(hPal);
}

unsigned int PaletteEntryCountOnDevice(HDC hDC)
{
	//we will consider palette only for bitspixel value <=8 because palette is not used for 16/24/32 bits
	/*
	if (!(nNumColors = biClrUsed))
	{
	if (biBitCount != 24)
	nNumColors = 1 << biBitCount;
	}

	*/
	UINT uEntry = 0, uColors = 0;
	if(::GetSystemPaletteEntries(hDC, 0, 0, NULL))
	{
		uEntry = ::GetDeviceCaps(hDC, BITSPIXEL);
		if(uEntry <= 1) 
			uColors = 2;
		else if(uEntry <= 4)
			uColors = 16;
		else if(uEntry <= 8)
			uColors = 256;
		else uColors = 0;
		//do we have to multiply with ::GetDeviceCaps(hDC, BITPLANES) ? 
	}
	return(uColors);
}


DWORD GetDIBPaletteSize(LPBYTE lpDib)
{
	DWORD dwPalSize;
	if(IS_WIN30_DIB(lpDib))
		dwPalSize = DIBNoOfPalColors(lpDib) * sizeof(RGBQUAD);
	else
		dwPalSize = DIBNoOfPalColors(lpDib) * sizeof(RGBTRIPLE);

	return(dwPalSize);
}

bool PaintDDBitmap(HDC hDC, const LPRECT lpRectDC, HBITMAP hDDB, const LPRECT lpRectBM, HPALETTE hPal)
{
	HDC hDCCompatible = NULL;
	HBITMAP hbmOld1;
	HPALETTE hpalOld1, hpalOld2;
	
	hDCCompatible = ::CreateCompatibleDC(hDC);
	if(!hDCCompatible)
		return(false);

	hbmOld1 = (HBITMAP)::SelectObject(hDCCompatible, hDDB);

	//now select the palette in both the DC
	hpalOld1 = ::SelectPalette(hDC, hPal, FALSE);
	hpalOld2 = ::SelectPalette(hDCCompatible, hPal, FALSE); //FALSE will be ingnored as in case of memory DC MSDN

	/*How do we know that the passed in BITMAP is compatible with the passed in DC ?*/

	::SetStretchBltMode(hDC, COLORONCOLOR);
	if( (lpRectDC->right - lpRectDC->left) == (lpRectBM->right - lpRectBM->left)  &&
		(lpRectDC->bottom - lpRectDC->top) == (lpRectBM->bottom - lpRectBM->top) )
		::BitBlt(hDC, lpRectDC->left, lpRectDC->top, lpRectDC->right - lpRectDC->left, lpRectDC->bottom - lpRectDC->top, 
		hDCCompatible, lpRectBM->left, lpRectBM->top, SRCCOPY);
	else
		::StretchBlt(hDC, lpRectDC->left, lpRectDC->top, lpRectDC->right - lpRectDC->left, lpRectDC->bottom - lpRectDC->top, 
		hDCCompatible, lpRectBM->left, lpRectBM->top, lpRectBM->right - lpRectBM->left, lpRectBM->bottom - lpRectBM->top, SRCCOPY); 

	//cleanup
	::SelectObject(hDCCompatible, hpalOld2);
	::SelectObject(hDCCompatible, hbmOld1);
	::DeleteDC(hDCCompatible);
	::SelectObject(hDC, hpalOld1);

	return(true);
}

bool PaintDIBitmap(HDC hDC, const LPRECT lpRectDC, HBITMAP hDIB, const LPRECT lpRectBM, HPALETTE hPal)
{
	if(!hDIB)
		return(false);

	LPBYTE lpDIB, lpDIBBits;

	if(hPal)
	{
		hPal = ::SelectPalette(hDC, hPal, FALSE);
		::RealizePalette(hDC);
	}

	//to get the pointer to the first byte of the DIB
	lpDIB = (LPBYTE) ::GlobalLock(hDIB);

	//calculating the position of the BITMAP BITS
	lpDIBBits = lpDIB + *(DWORD *)lpDIB + GetDIBPaletteSize((LPBYTE)lpDIB);
	
	//now depending on the area to be painted we will either SetDIBItsToDevice or StretchDIBits

	if( (lpRectDC->right - lpRectDC->left) == (lpRectBM->right - lpRectBM->left)  &&
		(lpRectDC->bottom - lpRectDC->top) == (lpRectBM->bottom - lpRectBM->top) )
		::SetDIBitsToDevice(hDC, lpRectDC->left, lpRectDC->top, lpRectDC->right - lpRectDC->left, lpRectDC->bottom - lpRectDC->top,
		lpRectBM->left, lpRectBM->top, 0, ((BITMAPINFOHEADER*)lpDIB)->biHeight, lpDIBBits, (BITMAPINFO *)lpDIB, DIB_RGB_COLORS);
	else
		::StretchDIBits(hDC, lpRectDC->left, lpRectDC->top,  lpRectDC->right - lpRectDC->left, lpRectDC->bottom - lpRectDC->top, 
		lpRectBM->left, lpRectBM->top,  lpRectBM->right - lpRectBM->left, lpRectBM->bottom - lpRectBM->top, lpDIBBits, (BITMAPINFO *)lpDIB, DIB_RGB_COLORS, SRCCOPY);
	
	//restore the old palette
	if(hPal)
	::SelectPalette(hDC, hPal, FALSE);
	::GlobalUnlock(hDIB);

	return(false);
}

HBITMAP ReadDIBFromFile(LPCSTR pstrFileName)
{
	HBITMAP hDib = NULL;
	BYTE *pByte = NULL, *pb = NULL;
	HANDLE hFile = NULL;
	BITMAPFILEHEADER bmfh;
	BOOL bRet = FALSE;
	DWORD dwRead;

   DWORD dwWidth = 0;
   DWORD dwHeight = 0;
   DWORD wSize, wPalSize, dwfilepos;
   BITMAPINFOHEADER bmih;
   UINT i;
   long dwBIH;

	::SetLastError(0);
	hFile = ::CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile CreateFile fail last Error %d ", GetLastError());
		OutputDebugStringA(xxx);
		return(NULL);
	}

	
	::SetLastError(0);
	bRet = ::ReadFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwRead, NULL);
	if(!bRet)
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile ReadFile fail last Error %d Line No: %d", GetLastError(), __LINE__);
		OutputDebugStringA(xxx);
		::CloseHandle(hFile);
		return(NULL);
	}

	//check to see if BITMAP
	if(bmfh.bfType != ImageBM)
		return(NULL);

	//check if this is BITMAPINFOHEADER or BITMAPCOREHEADER
	::SetLastError(0);
	bRet = ::ReadFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwRead, NULL);
	if( !bRet)
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile ReadFile fail last Error %d Line No: %d", GetLastError(), __LINE__);
		OutputDebugStringA(xxx);
		::CloseHandle(hFile);
		return(NULL);
	}
	wSize = bmih.biSize;
	if(wSize == sizeof(BITMAPCOREHEADER))
	{
		BITMAPCOREHEADER bc;

		dwBIH = sizeof(BITMAPINFOHEADER);
		memset(&bmih, 0, dwBIH);
		::SetLastError(0);
		dwfilepos = ::SetFilePointer(hFile, -dwBIH, NULL, FILE_CURRENT);
		if(dwfilepos == INVALID_SET_FILE_POINTER)
		{
			char xxx[128];
			sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile SetFilePointer return value: %d last Error %d Line No: %d", dwfilepos, GetLastError(), __LINE__);
			OutputDebugStringA(xxx);
			::CloseHandle(hFile);
			return(NULL);
		}

		bRet = ::ReadFile(hFile, &bc, sizeof(BITMAPCOREHEADER), &dwRead, NULL);
		if( !bRet)
		{
			char xxx[128];
			sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile ReadFile fail last Error %d Line No: %d", GetLastError(), __LINE__);
			OutputDebugStringA(xxx);
			::CloseHandle(hFile);
			return(NULL);
		}

		//now save the BITMAPCOREHEADER to a BITMAPINFOHEADER
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = bc.bcWidth;
		bmih.biHeight = bc.bcHeight;
		bmih.biPlanes = bc.bcPlanes;
		bmih.biBitCount = bc.bcBitCount;
		bmih.biCompression = BI_RGB;
	}

	if(bmih.biSizeImage == 0)
		bmih.biSizeImage = (((bmih.biWidth * bmih.biBitCount + 31) & ~31) >> 3) * bmih.biPlanes * bmih.biHeight;
	if(bmih.biClrUsed == 0)
		bmih.biClrUsed = DIBNoOfPalColors((LPBYTE)&bmih);
	//calculate the size of the palette in RGBQUAD
	wPalSize = bmih.biClrUsed * sizeof(RGBQUAD);

	//now allocate the whole memory that will be used for the DIB

	hDib = (HBITMAP)::GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER) + wPalSize + bmih.biSizeImage);
	pByte = (BYTE *)::GlobalLock(hDib);

	::CopyMemory(pByte, &bmih, sizeof(BITMAPINFOHEADER));
	pb = pByte + sizeof(BITMAPINFOHEADER);

    //reading the palette if any
	if(bmih.biClrUsed > 0)
	{
		if(wSize == sizeof(BITMAPCOREHEADER))
		{// Convert a old color table (3 byte RGBTRIPLEs) to a new color table (4 byte RGBQUADs)
			LPRGBTRIPLE lptriple = NULL;
			lptriple = new RGBTRIPLE[bmih.biClrUsed];
			bRet = ::ReadFile(hFile, lptriple, sizeof(RGBTRIPLE) * bmih.biClrUsed, &dwRead, NULL);
			if(dwRead !=   (sizeof(RGBTRIPLE) * bmih.biClrUsed))
			{
				char xxx[128];
				sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile ReadFile fail last Error %d Line No: %d", GetLastError(), __LINE__);
				OutputDebugStringA(xxx);
				::CloseHandle(hFile);
				::GlobalUnlock(hDib);
				::GlobalFree(hDib);
				return(NULL);
			}
			for(i = 0; i < bmih.biClrUsed; i = i + 1)
			{
				(((LPRGBQUAD)pb) + i)->rgbBlue = (lptriple + i)->rgbtBlue;
				(((LPRGBQUAD)pb) + i)->rgbGreen = (lptriple + i)->rgbtGreen; 
				(((LPRGBQUAD)pb) + i)->rgbRed = (lptriple + i)->rgbtRed;
				(((LPRGBQUAD)pb) + i)->rgbReserved = 0;
			}
			delete(lptriple);
		}
		else
		{
			bRet = ::ReadFile(hFile, pb, sizeof(RGBQUAD) * bmih.biClrUsed, &dwRead, NULL);
			if(dwRead !=   (sizeof(RGBQUAD) * bmih.biClrUsed))
			{
				char xxx[128];
				sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile ReadFile fail last Error %d Line No: %d", GetLastError(), __LINE__);
				OutputDebugStringA(xxx);
				::CloseHandle(hFile);
				::GlobalUnlock(hDib);
				::GlobalFree(hDib);
				return(NULL);
			}
		}
		//now advance the DIB memory pointer
		pb = pb + sizeof(RGBQUAD) * bmih.biClrUsed;
	}
	
	//now reading the actual bitmap bits
	bRet = ::ReadFile(hFile, pb, bmih.biSizeImage, &dwRead, NULL);
	if(dwRead !=   bmih.biSizeImage)
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "ReadDIBFromFile ReadFile fail last Error %d Line No: %d", GetLastError(), __LINE__);
		OutputDebugStringA(xxx);
		::CloseHandle(hFile);
		::GlobalUnlock(hDib);
		::GlobalFree(hDib);
		return(NULL);
	}

	//cleanup
	::GlobalUnlock(hDib);
	return(hDib);
}


BOOL OpenDLGBitmapFile(HWND hWnd, LPSTR lpstrFileName)
{

	OPENFILENAME ofn;
	char strFilter[512], strDefExt[20];
	char szInitDir[128];
	

	//get the file name
	memset(&ofn, 0, sizeof(OPENFILENAME));
	//	sprintf_s(strFilter, sizeof(char)*256, "Monochrome Bitmap (*.bmp, *.dib)\0 *.bmp;*.dib\016 Color Bitmap   (*.bmp, *.dib)\0 *.bmp;*.dib\0256 Color Bitmap   (*.bmp, *.dib)\0 *.bmp;*.dib\024-Bit Bitmap  (*.bmp, *.dib)\0 *.bmp;*.dib\0Icon  (*.ico)\0  *.ico\0Cursor  (*.cur)\0 *.cur\0\0");
	memcpy_s(strFilter, 512, "Monochrome Bitmap (*.bmp, *.dib)\0 *.bmp\0 16 Color Bitmap   (*.bmp, *.dib)\0 *.bmp\0 256 Color Bitmap   (*.bmp, *.dib)\0 *.bmp\0 24-Bit Bitmap  (*.bmp, *.dib)\0 *.bmp\0 Icon  (*.ico)\0  *.ico\0 Cursor  (*.cur)\0 *.cur\0\0", 300);
	::GetCurrentDirectory(sizeof(szInitDir), szInitDir);
	ofn.lStructSize = sizeof(OPENFILENAME); 
	ofn.hwndOwner = hWnd; 
	ofn.lpstrFilter = strFilter; 
	ofn.nFilterIndex = 4;
	lstrcpy(lpstrFileName, _T("Untitled.bmp"));
	ofn.lpstrFile= lpstrFileName; 
	ofn.nMaxFile = sizeof(char[256]); 
	ofn.lpstrFileTitle = lpstrFileName; 
	ofn.nMaxFileTitle = sizeof(lpstrFileName); 
	ofn.lpstrInitialDir = szInitDir; 
	lstrcpy(strDefExt, "bmp");  
	ofn.lpstrDefExt = strDefExt;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT|OFN_FORCESHOWHIDDEN|OFN_LONGNAMES|OFN_PATHMUSTEXIST; 
	
	return(::GetOpenFileName(&ofn));
}


BOOL SaveDLGBitmapFile(HWND hWnd, LPSTR lpstrFileName)
{
	OPENFILENAME ofn;
	char strFilter[512], strDefExt[20];
	char szInitDir[128];


	//get the file name
	memset(&ofn, 0, sizeof(OPENFILENAME));
	//	sprintf_s(strFilter, sizeof(char)*256, "Monochrome Bitmap (*.bmp, *.dib)\0 *.bmp;*.dib\016 Color Bitmap   (*.bmp, *.dib)\0 *.bmp;*.dib\0256 Color Bitmap   (*.bmp, *.dib)\0 *.bmp;*.dib\024-Bit Bitmap  (*.bmp, *.dib)\0 *.bmp;*.dib\0Icon  (*.ico)\0  *.ico\0Cursor  (*.cur)\0 *.cur\0\0");
	memcpy_s(strFilter, 512, "Monochrome Bitmap (*.bmp, *.dib)\0 *.bmp\0 16 Color Bitmap   (*.bmp, *.dib)\0 *.bmp\0 256 Color Bitmap   (*.bmp, *.dib)\0 *.bmp\0 24-Bit Bitmap  (*.bmp, *.dib)\0 *.bmp\0 Icon  (*.ico)\0  *.ico\0 Cursor  (*.cur)\0 *.cur\0\0", 300);
	::GetCurrentDirectory(sizeof(szInitDir), szInitDir);
	ofn.lStructSize = sizeof(OPENFILENAME); 
	ofn.hwndOwner = hWnd; 
	ofn.lpstrFilter = strFilter; 
	ofn.nFilterIndex = 4;
	lstrcpy(lpstrFileName, _T("Untitled.bmp"));
	ofn.lpstrFile= lpstrFileName; 
	ofn.nMaxFile = sizeof(char[256]); 
	ofn.lpstrFileTitle = lpstrFileName; 
	ofn.nMaxFileTitle = sizeof(lpstrFileName); 
	ofn.lpstrInitialDir = szInitDir; 
	lstrcpy(strDefExt, "bmp");  
	ofn.lpstrDefExt = strDefExt;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT|OFN_FORCESHOWHIDDEN|OFN_LONGNAMES|OFN_PATHMUSTEXIST; 
	 
	return(::GetSaveFileName(&ofn));
}

HRGN GetRGNFromDIB(HBITMAP hDib, COLORREF clrTransparent)
{
	HRGN hRgn = NULL;


	return(hRgn);
}


HPALETTE GetDIBPalette(HBITMAP hDib)
{
	HPALETTE hPal = NULL;
	WORD ctr = 0;
	LPBYTE lpByte = NULL, lpPal = NULL; 
	LPLOGPALETTE lplogpal = NULL;
	errno_t err;

	if(!DIBNoOfPalColors((LPBYTE)hDib))
		return(NULL);
	hPal = (HPALETTE)::GlobalAlloc(GHND, sizeof(LOGPALETTE) + DIBNoOfPalColors((LPBYTE)hDib) * sizeof(PALETTEENTRY));
	lplogpal = (LPLOGPALETTE)::GlobalLock(hPal);
	lplogpal->palVersion = PALETTE_VERSION;
	lplogpal->palNumEntries = DIBNoOfPalColors((LPBYTE)hDib);
	
	if(IS_WIN30_DIB(hDib))
	{
		lpByte = (LPBYTE)hDib + sizeof(BITMAPINFOHEADER);
		err = memcpy_s(lplogpal->palPalEntry, lplogpal->palNumEntries * sizeof(PALETTEENTRY), lpByte, lplogpal->palNumEntries * sizeof(RGBQUAD));
		if(err != 0)
		{
			char xxx[128];
			sprintf_s(xxx, sizeof(char)*128, "GetDIBPalette BITMAPINFOHEADER memcpy_s fail Error %d Line No: %d", err, __LINE__);
			OutputDebugStringA(xxx);
			return(NULL);
		}
		
	}
	else
	{
		lpByte = ((LPBYTE)hDib) + sizeof(BITMAPCOREHEADER);
		for(ctr = 0; ctr < lplogpal->palNumEntries; ctr = ctr + 1)
		{
			(lplogpal->palPalEntry + ctr)->peBlue = (((LPRGBTRIPLE)lpByte) + ctr)->rgbtBlue;
			(lplogpal->palPalEntry + ctr)->peGreen = (((LPRGBTRIPLE)lpByte) + ctr)->rgbtGreen;
			(lplogpal->palPalEntry + ctr)->peRed = (((LPRGBTRIPLE)lpByte) + ctr)->rgbtRed;
		}
	}
#ifdef _DEBUG
	char xxx[128];
	for(ctr = 0; ctr < lplogpal->palNumEntries; ctr = ctr + 1)
	{
		sprintf_s(xxx, sizeof(char)*128, "GetDIBPalette PALETTEENTRY [%d] Red: %d Green: %d Blue: %d Line No: %d", ctr, (lplogpal->palPalEntry + ctr)->peRed, (lplogpal->palPalEntry + ctr)->peGreen, (lplogpal->palPalEntry + ctr)->peBlue, __LINE__);
		OutputDebugStringA(xxx);
	}
#endif
	::GlobalUnlock(hPal);
	//we are ready with the LOGPALETTE
	
	::SetLastError(0);
	hPal = ::CreatePalette(lplogpal);
	if(hPal == NULL)
	{
		char xxx[128];
		sprintf_s(xxx, sizeof(char)*128, "GetDIBPalette CreatePalette fail Last Error %d Line No: %d", ::GetLastError(), __LINE__);
		OutputDebugStringA(xxx);
		return(NULL);
	}

	return(hPal);
}
/*
#ifdef __cplusplus
	}
#endif
*/