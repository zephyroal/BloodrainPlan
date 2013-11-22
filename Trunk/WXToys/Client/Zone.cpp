#include "StdAfx.h"
#include "Zone.h"
#include <algorithm>

CZone::CZone()
{
}

CZone::~CZone()
{
}

VOID CZone::Initial(VOID)
{
    // �������
    m_setRegisterObject.clear();
    m_listRegion.clear();
}

VOID CZone::RegisterObject(CObject_Map* pObject)
{
    // assert(pObject);

    // �ж��Ƿ�ȷʵ��CObject_Map�̳���
    // if(!g_theKernel.IsKindOf(pObject->GetClass(), GETCLASS(CObject_Map))) return;

    // ע�ᵽSet�У�����Ѿ��и�id������ע�ᣬ��ֱ���滻
    // INT id = pObject->GetID();
    // m_setRegisterObject.insert( id );

    // pObject->m_pZone = this;
}

VOID CZone::UnRegisterObject(CObject_Map* pObject)
{
    // assert(pObject);

    // ��Set�����������壬����ҵ���ɾ��
    // INT id = pObject->GetID();
    // std::set< INT >::iterator it = m_setRegisterObject.find( id );
    // if(it != m_setRegisterObject.end())
    // {
    //	m_setRegisterObject.erase(it);
    // }

    // pObject->m_pZone = NULL;
}

VOID CZone::AddRegion(INT id)
{
    std::list<INT>::iterator it;
    it = std::find(m_listRegion.begin(), m_listRegion.end(), id);
    if (it == m_listRegion.end())
    {
        m_listRegion.push_back(id);
    }
}
