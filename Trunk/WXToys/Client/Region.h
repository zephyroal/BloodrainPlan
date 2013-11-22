#ifndef __REGION_H__
#define __REGION_H__

#pragma once

#include <vector>
#include "TDMath.h"

class tEntityNode;

#define TDU_PI                  (3.1415926535)
#define TDU_MINFLOAT    (1.0E-9)
#define INVALID_ID                      0xffffffff      // !< Invalid dword ID (counterpart of null pointers)


class CRegion
{
public:
    CRegion(VOID);
    ~CRegion(VOID);

    BOOL AddPoint(const fVector2* pfvPoint);
    BOOL IsBulge(INT nIndex) const;
    BOOL JudgeBulge(INT nIndex);
    VOID                  JudgeAllBulge(VOID);

    // �õ�һ����������,���������㲻�ڶ������,����-1
    INT GetIndex(const fVector2* pfvPoint) const;

    // �õ����ڵ�͹��
    INT GetNeighborBulgeIndex(INT nIndex, BOOL bCountSelf, BOOL bClockwise) const;
    INT GetNeighborBulgeIndex(const fVector2* pfvPoint, BOOL bCountSelf, BOOL bClockwise) const;
    const fVector2* GetNeighborBulge(INT nIndex, BOOL bCountSelf, BOOL bClockwise) const;
    const fVector2* GetNeighborBulge(const fVector2* pfvPoint, BOOL bCountSelf, BOOL bClockwise) const;

    const fVector2* GetPoint(INT nIndex, BOOL bLoop) const;
    const fVector2* GetPoint(INT nIndex) const;

public:
    INT                   m_RegionID;
    INT                   m_nCount;
    INT                   m_nPassRegionLevel;                              // ��ͨ������� ���ߵȼ� fujia 2007.12.27
    std::vector<fVector2> m_vfPoints;
    std::vector<BOOL>     m_vbBulge;
};

#endif
