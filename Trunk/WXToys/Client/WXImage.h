#pragma once

class CWXImage
{
public:
    CWXImage(HWND hWnd);
    ~CWXImage(void);
    HWND m_hWnd;
    PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);

    void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

    void ScreehShots(HBITMAP hBmp, HDC hDC);
};
