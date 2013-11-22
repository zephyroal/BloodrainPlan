/*!
        @file
        @author		Albert Semenov
        @date		10/2008
*/
#ifndef __MAIN_PANEL_H__
#define __MAIN_PANEL_H__

#include <MyGUI.h>
#include "Base/Main.h"
#include "BaseLayout/BaseLayout.h"

#include "TreeControl.h"
#include "TreeControlItem.h"

#include "FileSystemInfo/FileSystemInfo.h"
#include "Ogre/RenderBox/RenderBoxScene.h"
class MainPanel :
    public wraps::BaseLayout
{
public:
    enum TypeEvents
    {
        EventNew,
        EventLoad,
        EventQuit,
        EventCreate,
        EventTabChangeSelect
    };

public:
    MainPanel();
    void setAtlasView(const MyGUI::UString& _name);
    void addObject(const MyGUI::UString& _name);
    void notifyTreeNodePrepare(MyGUI::TreeControl* pTreeControl, MyGUI::TreeControl::Node* pNode);
    MyGUI::UString getPath(MyGUI::TreeControl::Node* pNode) const;
private:
    void notifyMouseButtonClick(MyGUI::Widget* _sender);
    void notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index);
    void notifyTabChangeSelect(MyGUI::TabControl* _sender, size_t _index);

public:
    void SetAtlasViewIndex(int x);
    wraps::RenderBoxScene                            mRenderBoxScene;
    MyGUI::TreeControl*                              mpResourcesTree;
    MyGUI::delegates::CDelegate2<TypeEvents, size_t> eventAction;

    MyGUI::Button*                                   mButtonNew;
    MyGUI::Button*                                   mButtonLoad;
    MyGUI::Button*                                   mButtonQuit;
    MyGUI::TabControl*                               mTabControl;

    MyGUI::ComboBox*                                 mComboCreate;
    MyGUI::ImageBox*                                 mAtlasView;
    MyGUI::OgreTexture*                              mTexture;
};
#endif // __MAIN_PANEL_H__
