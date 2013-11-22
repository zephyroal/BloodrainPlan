#include "StdAfx.h"
#include "WXScene.h"

WXScene::WXScene(void)
{
}

WXScene::~WXScene(void)
{
    int i = 0;
}

void WXScene::Draw(CDC& dc, int width, int height)
{
    // m_strInfomaiton=TEXT("scene parse not ready!");

    __super::Draw(dc, width, height);
}

void WXScene::LoadTTLBScene(const char* sceneName)
{
    m_strInfomaiton = sceneName;
    TiXmlDocument sceneDoc;


    bool          bLoadSuccess = sceneDoc.LoadFile(sceneName);

    if ( !bLoadSuccess )
    {
        throw("load scene file failed");
        return;
    }

    const char*   strTemp;
    const char*   sValue;
    // Scene
    TiXmlElement* root = sceneDoc.FirstChildElement();

    // terrain
    TiXmlElement* element = root->FirstChildElement("Terrain");

    strTemp = element->Attribute("filename");

    // m_strInfomaiton.AppendFormat(TEXT("%s\n") , 0000 );
    m_strInfomaiton.AppendFormat(TEXT("\nterrain:%s\n"), strTemp);

    // load the terrain and insert into scene.
    // ETM::TerrainManager will do the insertion

    while ( 1 )
    {
        element = element->NextSiblingElement();
        strTemp = element->Attribute("type");
        if ( IsStrEqual(strTemp, "StaticEntity"))
        {
            break;
        }

        TiXmlElement* propriety = element->FirstChildElement("Property");
        const char*   objName   = element->Attribute("name");
        m_strInfomaiton.AppendFormat(TEXT("type:%s,name:%s\n"), strTemp, objName);

        while ( propriety )
        {
            strTemp = propriety->Attribute("name");
            sValue  = propriety->Attribute("value");

            m_strInfomaiton.AppendFormat(TEXT("(name:%s,value:%s )"), strTemp, sValue);

            propriety = propriety->NextSiblingElement();
        }

        m_strInfomaiton.AppendFormat(TEXT("\n"));
    }



    static int StaticEntityIndex = 0;
    while ( element )
    {
        strTemp = element->Attribute("type");

        if ( IsStrEqual("StaticEntity", strTemp))
        {
            float         w, x, y, z;

            TiXmlElement* propriety = element->FirstChildElement("Property");

            int           statics = 0;
            while ( propriety )
            {
                ++statics;
                strTemp = propriety->Attribute("name");
                sValue  = propriety->Attribute("value");
                if ( IsStrEqual("mesh name", strTemp))
                {
                }
                else if ( IsStrEqual("position", strTemp))
                {
                    sscanf(sValue, "%f %f %f", &x, &y, &z);
                }
                else if ( IsStrEqual("orientation", strTemp))
                {
                    sscanf(sValue, "%f %f %f %f", &w, &x, &y, &z);
                }
                else if ( IsStrEqual("scale", strTemp))
                {
                    sscanf(sValue, "%f %f %f", &x, &y, &z);
                }
                else if ( IsStrEqual("receive shadows", strTemp))
                {
                }
                else if ( IsStrEqual("receive decals", strTemp))
                {
                }
                else
                {
                    assert(0);
                }


                propriety = propriety->NextSiblingElement();

                // insert the static entity into the scene
            }
        }
        else if ( IsStrEqual("Model", strTemp))
        {
        }
        else if (  IsStrEqual("ParticleSystem", strTemp))
        {
        }
        else if ( IsStrEqual("TerrainLiquid", strTemp))
        {
        }
        else if ( IsStrEqual("Effect", strTemp))
        {
        }
        else if (IsStrEqual("Actor", strTemp))
        {
            // Actor
        }
        else
        {
            OutputDebugString("\nÎÞ·¨¼ÓÔØ:");
            OutputDebugString(strTemp);
            OutputDebugString("\n");
        }

        ++StaticEntityIndex;
        element = element->NextSiblingElement();
    }

    m_strInfomaiton.AppendFormat(TEXT("\ntotal static entity counts:%d\n"), StaticEntityIndex);
}
