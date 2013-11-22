#pragma once

class NullInfo : public FileInfoInterface
{
public:
    NullInfo(void);
    ~NullInfo(void);

    virtual BOOL LoadFile(CONST CString& strFile);
    virtual void Draw(CDC& dc, int width, int height);
};
