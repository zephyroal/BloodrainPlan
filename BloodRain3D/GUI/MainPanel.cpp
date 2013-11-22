/*!
        @file
        @author		Albert Semenov
        @date		10/2008
*/
#include "Precompiled.h"
#include "MainPanel.h"
// рутовая папк?всей меди?

MyGUI::UString gMediaBase;
typedef std::pair<std::wstring, common::FileInfo> PairFileInfo;



MainPanel::MainPanel() :
    BaseLayout("MainPanel.layout")
{
    // mMainWidget->setPosition(10, 10);

    assignWidget(mButtonNew, "New");
    assignWidget(mButtonLoad, "Load");
    assignWidget(mButtonQuit, "Quit");
    assignWidget(mComboCreate, "CreateObject");
    assignWidget(mTabControl, "MainControl");

    MyGUI::Canvas* canvas2;
    assignWidget(canvas2, "SceneAddView");
    // canvas2->setPointer("hand");

#ifdef _DEBUG
    mRenderBoxScene.setCanvas(canvas2);
    mRenderBoxScene.injectObject("Robot.mesh");
    mRenderBoxScene.setAutoRotation(true);
    mRenderBoxScene.setMouseRotation(true);
#endif


    mTabControl->eventTabChangeSelect += MyGUI::newDelegate(this, &MainPanel::notifyTabChangeSelect);

    mButtonNew->eventMouseButtonClick  += MyGUI::newDelegate(this, &MainPanel::notifyMouseButtonClick);
    mButtonLoad->eventMouseButtonClick += MyGUI::newDelegate(this, &MainPanel::notifyMouseButtonClick);
    mButtonQuit->eventMouseButtonClick += MyGUI::newDelegate(this, &MainPanel::notifyMouseButtonClick);
    mComboCreate->eventComboAccept     += MyGUI::newDelegate(this, &MainPanel::notifyComboAccept);

    /*#ifdef MYGUI_OGRE_PLATFORM
    Ogre::ArchiveManager::ArchiveMapIterator ArchiveIterator = Ogre::ArchiveManager::getSingleton().getArchiveIterator();
    while (ArchiveIterator.hasMoreElements())
    {
    Ogre::Archive* pArchive = ArchiveIterator.getNext();

    MyGUI::TreeControl::Node* pNode = new MyGUI::TreeControl::Node(pArchive->getName(), "Data");
    pNode->setData(pArchive);
    pRoot->add(pNode);
    }
    #else*/


    // #endif

    assignWidget(mAtlasView, "AtlasView");
    mTexture = (MyGUI::OgreTexture* )MyGUI::RenderManager::getInstance().createTexture("AtlasView");
    setAtlasView("");

    assignWidget(mpResourcesTree, "MyTree");
    mpResourcesTree->eventTreeNodePrepare += newDelegate(this, &MainPanel::notifyTreeNodePrepare);


    MyGUI::TreeControl::Node* pRoot = mpResourcesTree->getRoot();
    gMediaBase = std::wstring(L"E:/ZeusSVN/Trunk/BloodRain3D/media/TLBBMediaФ¤Бф");
    common::VectorFileInfo    result;
    common::getSystemFileList(result, gMediaBase, L"*.*");

    for (common::VectorFileInfo::iterator item = result.begin(); item != result.end(); ++item)
    {
        if ((*item).name == L".." || (*item).name == L".")
        {
            continue;
        }
        MyGUI::TreeControl::Node* pNode = new MyGUI::TreeControl::Node((*item).name, "Data");

        pNode->setData(PairFileInfo(gMediaBase, *item));
        pRoot->add(pNode);
    }
}

void MainPanel::setAtlasView(const MyGUI::UString& _name)
{
    /*
    TexturePtr ui = TextureManager::getSingletonPtr()->createManual(
                            "<TileImagesetTexture>", "General",TEX_TYPE_2D,
                            layerTextureWidth,layerTextureHeight, 1,3, PF_BYTE_RGBA , TU_WRITE_ONLY );*/
    Ogre::TexturePtr         layer0 = Ogre::TextureManager::getSingletonPtr()->getByName("<TileImagesetTexture>");

    mTexture->setOgreTexture(layer0);
    // mTexture->saveToFile("AtlasView.dds");
    MyGUI::IntCoord          coord(0, 0, 1024, 128);
    MyGUI::types::TSize<int> tileSize(64, 64);
    mAtlasView->setItemSelect(0);
    mAtlasView->setImageInfo("AtlasView", coord, tileSize);
    // mAtlasView->setCoord
    // mAtlasView->setRenderItemTexture(mTexture);
    // mAtlasView->setCoord(0,0,2048,256);
    // mAtlasView->_setUVSet(MyGUI::FloatRect(0, 0, 1, 1));
}
void MainPanel::SetAtlasViewIndex(int x)
{
    mAtlasView->setItemSelect(x);
}
void MainPanel::addObject(const MyGUI::UString& _name)
{
    mComboCreate->addItem(_name);
}
void MainPanel::notifyTabChangeSelect(MyGUI::TabControl* _sender, size_t _index)
{
    eventAction(EventTabChangeSelect, _index);
}
void MainPanel::notifyMouseButtonClick(MyGUI::Widget* _sender)
{
    if (_sender == mButtonNew)
    {
        eventAction(EventNew, 0);
    }
    else if (_sender == mButtonLoad)
    {
        eventAction(EventLoad, 0);
    }
    else if (_sender == mButtonQuit)
    {
        eventAction(EventQuit, 0);
    }
}

void MainPanel::notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index)
{
    eventAction(EventCreate, _index);

    mComboCreate->setIndexSelected(MyGUI::ITEM_NONE);
    mComboCreate->setCaption("Select Type");
}
void MainPanel::notifyTreeNodePrepare(MyGUI::TreeControl* pTreeControl, MyGUI::TreeControl::Node* pNode)
{
    if (pNode == pTreeControl->getRoot())
    {
        return;
    }

    pNode->removeAll();

    /*#ifdef MYGUI_OGRE_PLATFORM
    Ogre::Archive* pArchive = *(pNode->getData<Ogre::Archive*>());

    MyGUI::UString strPath(getPath(pNode));
    Ogre::StringVectorPtr Resources = pArchive->find(strPath + "*", false, true);
    for (Ogre::StringVector::iterator Iterator = Resources->begin(); Iterator != Resources->end(); ++Iterator)
    {
    MyGUI::TreeControl::Node* pChild = new MyGUI::TreeControl::Node(*Iterator, "Folder");
    pChild->setData(pArchive);
    pNode->add(pChild);
    }

    Resources = pArchive->find(strPath + "*", false, false);
    for (Ogre::StringVector::iterator Iterator = Resources->begin(); Iterator != Resources->end(); ++Iterator)
    {
    MyGUI::UString strName(*Iterator);
    MyGUI::UString strExtension;
    size_t nPosition = strName.rfind(".");
    if (nPosition != MyGUI::UString::npos)
    {
    strExtension = strName.substr(nPosition + 1);
    std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), tolower);
    }

    MyGUI::UString strImage;
    if (strExtension == "png" || strExtension == "tif" || strExtension == "tiff" || strExtension == "jpg" || strExtension == "jpeg")
    strImage = "Image";
    else
    if (strExtension == "mat" || strExtension == "material")
    strImage = "Material";
    else
    if (strExtension == "layout")
    strImage = "Layout";
    else
    if (strExtension == "ttf" || strExtension == "font" || strExtension == "fontdef")
    strImage = "Font";
    else
    if (strExtension == "txt" || strExtension == "text")
    strImage = "Text";
    else
    if (strExtension == "xml")
    strImage = "XML";
    else
    if (strExtension == "mesh")
    strImage = "Mesh";
    else
    if (strExtension == "htm" || strExtension == "html")
    strImage = "HTML";
    else
    strImage = "Unknown";

    MyGUI::TreeControl::Node* pChild = new MyGUI::TreeControl::Node(strName, strImage);
    pChild->setPrepared(true);
    pNode->add(pChild);
    }
    #else*/
    PairFileInfo info = *(pNode->getData<PairFileInfo>());
    // если папк? то добавляем дете?
    if (info.second.folder)
    {
        std::wstring           path = info.first + L"/" + info.second.name;
        common::VectorFileInfo result;
        common::getSystemFileList(result, path, L"*.*");

        for (common::VectorFileInfo::iterator item = result.begin(); item != result.end(); ++item)
        {
            if ((*item).name == L".." || (*item).name == L".")
            {
                continue;
            }
            if ((*item).folder)
            {
                MyGUI::TreeControl::Node* pChild = new MyGUI::TreeControl::Node((*item).name, "Folder");
                pChild->setData(PairFileInfo(path, *item));
                pNode->add(pChild);
            }
            else
            {
                MyGUI::UString strName((*item).name);
                MyGUI::UString strExtension;
                size_t         nPosition = strName.find(".");
                if (nPosition != MyGUI::UString::npos)
                {
                    strExtension = strName.substr(nPosition + 1);
                    std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), tolower);
                }

                MyGUI::UString strImage;
                if (strExtension == "png" || strExtension == "tif" || strExtension == "tiff" || strExtension == "jpg" || strExtension == "jpeg")
                {
                    strImage = "Image";
                }
                else if (strExtension == "mat" || strExtension == "material")
                {
                    strImage = "Material";
                }
                else if (strExtension == "layout")
                {
                    strImage = "Layout";
                }
                else if (strExtension == "ttf" || strExtension == "font" || strExtension == "fontdef")
                {
                    strImage = "Font";
                }
                else if (strExtension == "txt" || strExtension == "text")
                {
                    strImage = "Text";
                }
                else if (strExtension == "xml")
                {
                    strImage = "XML";
                }
                else if (strExtension == "mesh")
                {
                    strImage = "Mesh";
                    MyGUI::TreeControl::Node* pChild = new MyGUI::TreeControl::Node((*item).name, strImage);
                    pChild->setPrepared(true);
                    pNode->add(pChild);
                }
                else if (strExtension == "htm" || strExtension == "html")
                {
                    strImage = "HTML";
                }
                else
                {
                    strImage = "Unknown";
                }
            }
        }
    }

    // #endif
}

MyGUI::UString MainPanel::getPath(MyGUI::TreeControl::Node* pNode) const
{
    if (!pNode || pNode == mpResourcesTree->getRoot())
    {
        return MyGUI::UString();
    }

    MyGUI::UString strPath;
    while (pNode->getParent() != mpResourcesTree->getRoot())
    {
        strPath = pNode->getText() + "/" + strPath;
        pNode   = pNode->getParent();
    }

    return strPath;
}
// namespace demo
