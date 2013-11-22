/**
* @Function: CreateMeshDecal
* @Description: 创建模型贴花Mesh，此Mesh只被创建一次
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
    m_pSceneManager->getRootSceneNode()->attachObject(m_pMeshDecal);     // 挂在根节点

    int x_size = 4;                      // 在X方向的多边形数量
    int z_size = 4;                      // 在Z方向的多边形数量

    // 此Mesh所使用的材质，用三角形带创建Mesh
    m_pMeshDecal->begin("Crosshairs", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    for (int i = 0; i <= x_size; i++)
    {
        for (int j = 0; j <= z_size; j++)
        {
            // 添加一个顶点
            m_pMeshDecal->position(Ogre::Vector3(i, 0, j));
            // 为这个顶点定义纹理坐标
            m_pMeshDecal->textureCoord((float)i / (float)x_size, (float)j / (float)z_size);
        }
    }

    for (int i = 0; i < x_size; i++)
    {
        for (int j = 0; j < z_size; j++)
        {
            // 通过顶点索引建立四边形(4*4)，(x_size + 1)是一行上的顶点数
            // quad只用于三角形带
            m_pMeshDecal->quad(
                i * (x_size + 1) + j,                                  // 左上
                i * (x_size + 1) + j + 1,                            // 右上
                (i + 1) * (x_size + 1) + j + 1,                    // 右下
                (i + 1) * (x_size + 1) + j                           // 左下
                );
        }
    }
    m_pMeshDecal->end();
}

== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==


/**
* @Function: SetMeshDecal
* @Description: 设置模型贴花到指定位置
* @Param x: 坐标
* @Param z:
* @Param rad: 半径
*/
void CMap::SetMeshDecal(Ogre::Real x, Ogre::Real z, Ogre::Real rad)
{
    Ogre::Real x1 = x - rad;                // 左下角坐标
    Ogre::Real z1 = z - rad;

    int        x_size = 4;                          // 在X方向的多边形数量
    int        z_size = 4;

    Ogre::Real x_step = (rad * 2) / x_size;                // 每个四边形的宽、高
    Ogre::Real z_step = (rad * 2) / z_size;

    // 更新地形贴花Mesh
    m_pMeshDecal->beginUpdate(0);
    for (int i = 0; i <= x_size; i++)
    {
        for (int j = 0; j <= z_size; j++)
        {
            // 根据地形高度更新顶点位置
            m_pMeshDecal->position(Ogre::Vector3(x1, GetTerrainHeight(x1, z1) + 1, z1));
            m_pMeshDecal->textureCoord((float)i / (float)x_size, (float)j / (float)z_size);
            z1 += z_step;                        // 列中每行
        }
        x1 += x_step;                          // 行中每列
        z1  = z - rad;
    }

    // 更新四边形
    for (int i = 0; i < x_size; i++)
    {
        for (int j = 0; j < z_size; j++)
        {
            m_pMeshDecal->quad(
                i * (x_size + 1) + j,                                  // 左下
                i * (x_size + 1) + j + 1,                            // 左上
                (i + 1) * (x_size + 1) + j + 1,                   // 右上
                (i + 1) * (x_size + 1) + j);                        // 右下
        }
    }
    m_pMeshDecal->end();
}

== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
/**
* @Function: GetTerrainHeight
* @Description: 获得地形高度
* @Param x: 坐标
* @Param z:
* @Return: 高度
*/
Real CMap::GetTerrainHeight(Real x, Real z)
{
    // 设置光线的起始位置为人物上方5000高度
    m_UpdateRay.setOrigin(Vector3(x, 5000.0f, z));
    // 设置光线的方向为Y轴负方向
    m_UpdateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
    m_pRaySceneQuery->setRay(m_UpdateRay);                                        // 设置用于光线追踪的光线

    RaySceneQueryResult&          result = m_pRaySceneQuery->execute(); // 获得搜索结果
    RaySceneQueryResult::iterator itr;

    for (itr = result.begin(); itr != result.end(); itr++)
    {
        if (itr->worldFragment)           // 如果是地形
        {
            return itr->worldFragment->singleIntersection.y;
        }
    }
    return 0;
}
