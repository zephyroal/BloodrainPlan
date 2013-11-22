/**
* @Function: CreateMeshDecal
* @Description: ����ģ������Mesh����Meshֻ������һ��
*/
class CMap
{
public:
    CMap()
    {
    }
    ~CMap()
    {
    }
    void CreateMeshDecal();
    void    SetMeshDecal(Ogre::Real x, Ogre::Real z, Ogre::Real rad);
    Real GetTerrainHeight(Real x, Real z);
private:
    Ogre::ManualObject* m_pMeshDecal;
    Ogre::SceneManager* m_pSceneManager;
};
void CMap::CreateMeshDecal(void)
{
    m_pMeshDecal = new Ogre::ManualObject("MeshDecal");
    m_pSceneManager->getRootSceneNode()->attachObject(m_pMeshDecal);     // ���ڸ��ڵ�

    int x_size = 4;                      // ��X����Ķ��������
    int z_size = 4;                      // ��Z����Ķ��������

    // ��Mesh��ʹ�õĲ��ʣ��������δ�����Mesh
    m_pMeshDecal->begin("Crosshairs", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    for (int i = 0; i <= x_size; i++)
    {
        for (int j = 0; j <= z_size; j++)
        {
            // ���һ������
            m_pMeshDecal->position(Ogre::Vector3(i, 0, j));
            // Ϊ������㶨����������
            m_pMeshDecal->textureCoord((float)i / (float)x_size, (float)j / (float)z_size);
        }
    }

    for (int i = 0; i < x_size; i++)
    {
        for (int j = 0; j < z_size; j++)
        {
            // ͨ���������������ı���(4*4)��(x_size + 1)��һ���ϵĶ�����
            // quadֻ���������δ�
            m_pMeshDecal->quad(
                i * (x_size + 1) + j,                                  // ����
                i * (x_size + 1) + j + 1,                            // ����
                (i + 1) * (x_size + 1) + j + 1,                    // ����
                (i + 1) * (x_size + 1) + j                           // ����
                );
        }
    }
    m_pMeshDecal->end();
}

== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==


/**
* @Function: SetMeshDecal
* @Description: ����ģ��������ָ��λ��
* @Param x: ����
* @Param z:
* @Param rad: �뾶
*/
void CMap::SetMeshDecal(Ogre::Real x, Ogre::Real z, Ogre::Real rad)
{
    Ogre::Real x1 = x - rad;                // ���½�����
    Ogre::Real z1 = z - rad;

    int        x_size = 4;                          // ��X����Ķ��������
    int        z_size = 4;

    Ogre::Real x_step = (rad * 2) / x_size;                // ÿ���ı��εĿ���
    Ogre::Real z_step = (rad * 2) / z_size;

    // ���µ�������Mesh
    m_pMeshDecal->beginUpdate(0);
    for (int i = 0; i <= x_size; i++)
    {
        for (int j = 0; j <= z_size; j++)
        {
            // ���ݵ��θ߶ȸ��¶���λ��
            m_pMeshDecal->position(Ogre::Vector3(x1, GetTerrainHeight(x1, z1) + 1, z1));
            m_pMeshDecal->textureCoord((float)i / (float)x_size, (float)j / (float)z_size);
            z1 += z_step;                        // ����ÿ��
        }
        x1 += x_step;                          // ����ÿ��
        z1  = z - rad;
    }

    // �����ı���
    for (int i = 0; i < x_size; i++)
    {
        for (int j = 0; j < z_size; j++)
        {
            m_pMeshDecal->quad(
                i * (x_size + 1) + j,                                  // ����
                i * (x_size + 1) + j + 1,                            // ����
                (i + 1) * (x_size + 1) + j + 1,                   // ����
                (i + 1) * (x_size + 1) + j);                        // ����
        }
    }
    m_pMeshDecal->end();
}

== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
/**
* @Function: GetTerrainHeight
* @Description: ��õ��θ߶�
* @Param x: ����
* @Param z:
* @Return: �߶�
*/
Real CMap::GetTerrainHeight(Real x, Real z)
{
    // ���ù��ߵ���ʼλ��Ϊ�����Ϸ�5000�߶�
    m_UpdateRay.setOrigin(Vector3(x, 5000.0f, z));
    // ���ù��ߵķ���ΪY�Ḻ����
    m_UpdateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
    m_pRaySceneQuery->setRay(m_UpdateRay);                                        // �������ڹ���׷�ٵĹ���

    RaySceneQueryResult&          result = m_pRaySceneQuery->execute(); // ����������
    RaySceneQueryResult::iterator itr;

    for (itr = result.begin(); itr != result.end(); itr++)
    {
        if (itr->worldFragment)           // ����ǵ���
        {
            return itr->worldFragment->singleIntersection.y;
        }
    }
    return 0;
}
