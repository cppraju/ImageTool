/*
This file will have the basic utility image functions:
- copy
- save
- convert DDB to DIB
- convert DIB to DDB
- load images


p.s. these functions are generic and can be used for BITMAP handling in win32 
*/



#ifndef _IMAGE_FUNCTIONS
#define _IMAGE_FUNCTIONS

/*
	macro to check if the DIB buffer is a BITMAPINFOHEADER or a BITMAPCOREHEADER
*/
#define IS_WIN30_DIB(lpbi)(*(LPDWORD)lpbi == sizeof(BITMAPINFOHEADER))
#define PALETTE_VERSION 0x0300				//indicating Windows 3.0 compatibility
enum  imageType {DIB, DDB, ICON, CUR};
#define ImageBM 0x4D42

/*
#ifdef __cplusplus
	extern "C" {
#endif
*/
/*
	This returns the no of colors in the color table BITMAPINFOHEADER is assumed. If bit count is more than 8 there is no color table
	lpDIB - handle to the BITMAPINFOHEADER
	return number of colors
*/
UINT DIBNoOfPalColors(LPBYTE lpDIB);

/*
	Copy a DDB to DIB. Once the DIB is used it is the responsibility of the caller to cleanup by ::GlobalFree()
	hDC - the DC for which the source bitmap is compatible
	hbm - handle to the DDB
	hPalG - the palette of the DC if any 
	start - position from where the bits are to be copied
	cLines - number of scan lines to read
	return DIB
*/
HBITMAP CopyDDBToDIB(HDC hDC, HBITMAP hbm, HPALETTE hPal, UINT start = 0, UINT cLines = 0);

/*
	Copy a DIB to DDB based on the passed in DC. Once the the DDB is used it is the responsibilty of the of the caller to cleanup by ::DeleteObject()
	hDC - the DC for which the target bitmap is compatible. This DC cannot be a MEMORY DC. This has to be the DC of actual device, else this function will fail
	hDib - handle to the DIB
	hPalG - the palette of the DC if any 

	return DDB 
*/
HBITMAP CopyDIBToDDB(HDC hDC, HBITMAP hDib, HPALETTE hPal);

/*
	Copy a DIB to DDB based on the passed in DC. Once the the DDB is used it is the responsibilty of the of the caller to cleanup by ::DeleteObject()
	hDC - the DC for which the target bitmap is compatible. This DC cannot be a MEMORY DC. This has to be the DC of actual device, else this function will fail
	hDib - handle to the DIB
	hPalG - the palette of the DC if any 

	return DDB 
*/
HBITMAP CopyDIBToDDB(const BITMAPINFOHEADER *pbmih, HDC hDC, HBITMAP hDib, HPALETTE hPal);

/*
	lpstrFileName - name of the file 
	hDib - handle to the DIB
*/
bool SaveDIB(LPCSTR lpstrFileName, HANDLE hDib);

/*
	Save a DDB to the Clipboard
	hWnd - handle to the main application window
	hbm - handle to the bitmap to save to clipboard
	return true if success
*/
bool CopyDDBToCB(HWND hWnd, HBITMAP hbm, HPALETTE hPal);

/*
	Save a DIB to the Clipboard
	hWnd - handle to the main application window
	hbm - handle to the bitmap to save to clipboard
	return true if success
*/
bool CopyDIBToCB(HWND hWnd, HBITMAP hDib, HPALETTE hPal);
/*
	Get the palette for the supplied DC
	hDC - DC for which the palette has to extracted
	return HPALETTE
*/
HPALETTE GetDCPalette(HDC hDC);

/*
	Get the number of entries in the palette as selected in the DC
	hDC - the DC for which palette entries has to be counted
	return number of entries in the PALETTE as selected in the hDC parameter 
*/
unsigned int PaletteEntryCountOnDevice(HDC hDC);

/*

	Gets the DIB palette size in bytes based on the color used and the bits per pixel from the BITMAPINFO structure
	lpDib - byte pointer to the DIB
	return - size of the palette in bytes

*/
DWORD GetDIBPaletteSize(LPBYTE lpDib);

/*
	General DDB painting function
	hDC - DC on which the bitmap has to be painted
	lpRectDC - rectangle of the DC that needs to be painted
	hDDB - bitmap handle to the DDB
	lpRectBM - area of the bitmap that has to be painted on the DC
	hPal - the palette for the hDC
*/
bool PaintDDBitmap(HDC hDC, const LPRECT lpRectDC, HBITMAP hDDB, const LPRECT lpRectBM, HPALETTE hPal); 

/*
	General DIB painting function
	hDC - DC on which the bitmap has to be painted
	lpRectDC - rectangle of the DC that needs to be painted
	hDIB - handle to the DIB ie the BITMAPINFO and the bits full buffer
	lpRectBM - area of the bitmap that has to be painted on the DC. This cannot be NULL
	hPal - the palette for the hDC. This can be NULL.
*/
bool PaintDIBitmap(HDC hDC, const LPRECT lpRectDC, HBITMAP hDIB, const LPRECT lpRectBM, HPALETTE hPal); 

/*
	General DIB painting function
	hDC - DC on which the bitmap has to be painted
	lpRectDC - rectangle of the DC that needs to be painted
	hDIB - handle to the DIB ie the BITMAPINFO and the bits full buffer
	lpRectBM - area of the bitmap that has to be painted on the DC.This cannot be NULL
	hPal - the palette for the hDC. This can be NULL.
*/

/*
	This function will take a file name and will read the file to return the HANDLE of a DIB
	pstrFileName	-	name of the file
*/

HBITMAP ReadDIBFromFile(LPCSTR pstrFileName);


/*
	This is a helper function to open/select a image file

	lpstrFilename - name of the selected file along with folder name
						  it is assumed that the string will be allocated with proper length by the caller
	hWnd - handle of the owner this might be NULL
*/

BOOL OpenDLGBitmapFile(HWND hWnd, LPSTR lpstrFileName);

/*
	This is a helper function to select a name for saving a DIB to the disk
	lpstrFilename - name of the selected file along with folder name
						  it is assumed that the string will be allocated with proper length by the caller
	hWnd - handle of the owner this might be NULL
*/

BOOL SaveDLGBitmapFile(HWND hWnd, LPSTR lpstrFileName);

/*

	This will generate a HRGN from a DIB. A color that should be considered transperent should be provided. There can be a list of 
	colors that can be considered as tranperent. 
	hDib - handle to the DIB. A 24 bit DIB. 
	clrTransparent - the color that should be ommitted from the RGN 
	HRGN - return value is a handle to the RGN if successful else NULL
*/
HRGN GetRGNFromDIB(HBITMAP hDib, COLORREF clrTransparent);

/*
	hDib - handle to the DIB whose palette needs to be extracted
	HPALETTE - handle to the palette if any otherwise NULL. The memory for the 
					 palette will be allocated. The caller needs to call ::DeleteObject() when 
					 no longer needed.
*/
HPALETTE GetDIBPalette(HBITMAP hDib); 






/*
#ifdef __cplusplus
	}
#endif
*/

#endif