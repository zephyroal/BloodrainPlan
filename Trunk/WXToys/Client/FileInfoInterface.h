#pragma once

// #define  SUPER FileInfoInterface
class FileInfoInterface
{
protected:
    static int m_width;
    static int m_height;
    static int m_titlSize;
    CString    m_strInfomaiton;
public:
    FileInfoInterface()
        : m_strInfomaiton("NULL information")
    {
    }
    static void SetInfo(int w, int h, int title)
    {
        m_width    = w;
        m_height   = h;
        m_titlSize = title;
    }

    virtual ~FileInfoInterface()
    {
        int i = 0;
    }
    virtual BOOL LoadFile(CONST CString& strFile) = 0;
    virtual void Draw(CDC& dc, int width, int height)
    {
        CRect rcDraw(0, 0, width, height);
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(RGB(255, 255, 0));
        dc.DrawText(m_strInfomaiton, rcDraw, DT_TOP | DT_LEFT | DT_WORDBREAK);
    }

    void  DrawPoints(CDC& dc, const vector<fVector2>       points, COLORREF color, fVector2 factor)
    {
        CPen     pen;
        pen.CreatePen(PS_SOLID, 1,  color);
        CPen*    pOldPen = dc.SelectObject(&pen);

        fVector2 startPoint;
        for (int i = 0; i < points.size(); ++i )
        {
            fVector2 point = points[i];
            point.x *= factor.x;
            point.y *= factor.y;
            point.x += 3;
            point.y += 3;
            if (i == 0)
            {
                startPoint = point;
                dc.MoveTo(point.x, point.y);
            }

            dc.LineTo(point.x, point.y);
            if (i == points.size() - 1)
            {
                dc.LineTo(startPoint.x, startPoint.y);
            }
        }
    }
};
