#include "Procedure.h"
#undef  OIS_WITH_MULTITOUCH

CProcedure::CProcedure(RenderWindow*                  win,
                       Camera*                        cam,
                       SceneManager*                  sceneManager,
                       ET::TerrainManager*            terrainMgr,
                       ET::SplattingManager*          splatMgr,
                       BloodRainBites::MyTrayManager* trayMgr)
    : MyFrameListener(win, cam, true, true), mTerrainMgr(terrainMgr), mTerrainInfo(&terrainMgr->getTerrainInfo()), mSplatMgr(splatMgr)
    , mTrayMgr(0), mDetailsPanel(0)
{
    // Setup default variables
    mPointer             = NULL;
    mLMouseDown          = false;
    mRMouseDown          = false;
    mLShiftDown          = false;
    mLCtrlDown           = false;
    mLAltDown            = false;
    mLCaptial            = false;
    mMMouseDown          = false;
    mSelecting           = false;
    m_pSelectedSceneNode = NULL;
    m_pSelectedEntity    = NULL;
    mSceneMgr            = sceneManager;
    mTrayMgr             = trayMgr;

    mInputContext.mMouse    = mMouse;
    mInputContext.mKeyboard = mKeyboard;
    mTrayMgr->SetInputContext(mInputContext);
    // Reduce move speed
    mMoveSpeed    = 320.0f;
    mRotateSpeed *= 0.008f;
    // Register this so that we get events.
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
    // Create RaySceneQuery
    mRaySceneQuery = mSceneMgr->createRayQuery(Ray());
    // Create a "pointer" for use on the terrain
    Entity* pointer  = mSceneMgr->createEntity("PointerDeform", "ogrehead.mesh");
    Entity* pointer2 = mSceneMgr->createEntity("PointerPaint", "razor.mesh");
    Entity* pointer3 = mSceneMgr->createEntity("PointerSceneAdd", "robot.mesh");
    Entity* pointer5 = mSceneMgr->createEntity("PointerSceneEdit", "knot.mesh");

    mPointer = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mPointer->attachObject(pointer);
    mPointer->setScale(0.1f, 0.1f, 0.1f);
    createEditBrush();
    m_OldEditType   = m_EditType = ET_Deform;
    m_iFrame        = m_iLastFrame = 0;
    m_iUpadteCounts = 40;
    mChosenTexture  = 0;
    mDirection      = Vector3::ZERO;
    mCamNode        = mCamera->getParentSceneNode();
    m_bQuit         = false;
    m_iCount        = 0;
    m_bShowHelp     = true;
    mRect           = new SelectionRectangle("Selection SelectionRectangle");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mRect);
    mVolQuery      = mSceneMgr->createPlaneBoundedVolumeQuery(PlaneBoundedVolumeList());
    mNewEntityName = "robot.mesh";
    CreateTray();
    showDebugOverlay(true);
    setPointerVisible(true);
}

CProcedure::~CProcedure( )
{
    destroyMyGUIScene();
    if (mRaySceneQuery)
    {
        delete mRaySceneQuery;
    }
}

void CProcedure::CreateTray()
{
    // m_NewMaterialVec=Ogre::MaterialManager::getSingleton().create("SelectedEntityNew","General");
    m_iMtrNum = 0;
    // 创建UI系统，包括TrayManager的显示帧率和MyGUI控制界面
    CreateMyGUI();
    setupMyGUIResources();
    size_t handle = 0;
    mWindow->getCustomAttribute("WINDOW", &handle);
    createPointerManager(handle);

    setPointerVisible(true);
    createMyGUIScene();
    if (mWindow->isActive() == false)
    {
        mWindow->setActive(true);
#if MYGUI_PLATFORM == MYGUI_PLATFORM_WIN32
        ::Sleep(50);
#endif
    }

    mTrayMgr->showFrameStats(BloodRainBites::TL_BOTTOMLEFT);
    mTrayMgr->showCursor();
    // create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("");
    items.push_back("Filtering");
    items.push_back("Poly Mode");
    items.push_back("DargLB-MoveCamera");
    mDetailsPanel = mTrayMgr->createParamsPanel(BloodRainBites::TL_NONE, "DetailsPanel", 200, items);
    mDetailsPanel->setParamValue(9, "Bilinear");
    mDetailsPanel->setParamValue(10, "Solid");
    mDetailsPanel->hide();
    mTrayMgr->toggleAdvancedFrameStats();
    mHelpText = mTrayMgr->createTextBox(BloodRainBites::TL_CENTER, "HelpText", L"Help", 200, 200);
    // mHelpText->setCaption(L"帮助说明：");
    mHelpText->setText(L"Just Look at those helps");
    mTrayMgr->removeWidgetFromTray(mHelpText);
    mHelpText->hide();
    // mButton=mTrayMgr->createButton(BloodRainBites::TL_BOTTOMRIGHT, "Button", "PushMe",60);
}

void CProcedure::createEditBrush()
{
    // load the edit brush for terrain editing
    Image image;
    image.load("brush.png", "ET");
    image.resize(16, 16);
    mEditBrush = ET::loadBrushFromImage(image);

    // load the edit brush for terrain editing

    mBrush.brushType = ET::TileBrush::POINT;
    mBrush.op        = 0;
    mBrush.imageID   = 0;
    mBrush.layer     = 0;
    m_iLayer         = 0;
}

void CProcedure::RenderQuad(const FrameEvent& evt)
{
    mTrayMgr->frameRenderingQueued(evt);
    if (!mTrayMgr->isDialogVisible())
    {
        if (mDetailsPanel->isVisible())           // if details panel is visible, then update its contents
        {
            mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
            mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
            mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
            mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
            mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
            mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
            mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
        }
    }
}

bool CProcedure::frameStarted(const FrameEvent& evt)
{
    if (m_bQuit)
    {
        return false;
    }
    if (!MyFrameListener::frameStarted(evt))
    {
        return false;
    }
    m_iFrame++;
    // if(m_iCount>0)
    // mAnimationState->addTime(evt.timeSinceLastFrame);
    RenderQuad(evt);
    // handle movement
    // Frame动画，暂时未启用
/*
        for(iter=m_AnimationStateVec.begin();iter<m_AnimationStateVec.end();iter++)
        {
                //if(iter)
                //牢记，iteraor是指向元素的指针
                AnimationState *anim=(*iter);
                anim->addTime(evt.timeSinceLastFrame/5.0f);
        }*/

    mCamNode->translate(mCamera->getOrientation() * mDirection * mMoveSpeed * evt.timeSinceLastFrame);

    // Ensure we stay above terrain (somehow buggy still, doesn't work reliably)
    Vector3 camPos = mCamNode->getPosition( );
    // get terrain height at camPos
    float   terrainHeight = mTerrainInfo->getHeightAt(camPos.x, camPos.z);
    if ((terrainHeight + 30.0f) > camPos.y)
    {
        mCamNode->setPosition(camPos.x, terrainHeight + 30.0f, camPos.z);
    }
    if (mLMouseDown || mRMouseDown)
    {
        // deform or paint terrain on mouse down
        // left button raises, right button lowers
        if (m_EditType == ET_Deform)
        {
            // choose a brush intensity, this determines
            // how extreme our brush works on the terrain
            float   brushIntensity = evt.timeSinceLastFrame * 0.4 * (mLMouseDown ? 1 : -1);
            // translate our cursor position to vertex indexes
            Vector3 deformPos = mPointer->getPosition();
            int     x         = mTerrainInfo->posToVertexX(deformPos.x);
            int     z         = mTerrainInfo->posToVertexZ(deformPos.z);
            // now tell the ETM to deform the terrain
            mTerrainMgr->deform(x, z, mEditBrush, brushIntensity);
        }
        else if (m_EditType == ET_Paint)
        {
            /*
            // need to set our brush intensity larger for painting.
            // for painting, all texture channels are only 1 byte
            // large, so with a small intensity you won't get any
            // effect at all.
            float brushIntensity = evt.timeSinceLastFrame * 5.0 * (mLMouseDown? 1 : -1);
            // retrieve edit points
            Vector3 paintPos = mPointer->getPosition();
            int x = mTerrainInfo->posToVertexX(paintPos.x);
            int z = mTerrainInfo->posToVertexZ(paintPos.z);
            // now use the splatting manager to update the coverage maps
            mSplatMgr->paint(mChosenTexture, x, z, mEditBrush, brushIntensity);*/
            // need to set our brush intensity larger for painting.
            // for painting, all texture channels are only 1 byte
            // large, so with a small intensity you won't get any
            // effect at all.
            float        brushIntensity = evt.timeSinceLastFrame * 15.0 * (mLMouseDown ? 1 : -1);
            // retrieve edit points
            Vector3      paintPos = mPointer->getPosition();
            int          x        = mTerrainInfo->posToVertexX(paintPos.x);
            int          z        = mTerrainInfo->posToVertexZ(paintPos.z);
            // now use the splatting manager to update the coverage maps
            // mSplatMgr->paint(mChosenTexture, x, z, mEditBrush, brushIntensity);

            // mBrush.brushType =ET::TileBrush::POINT;
            // 一定要坚持，不是不报，功夫不到！
            // mBrush.op=0; //你手贱不贱啊！！！！！！
            // mBrush.imageID=mChosenTexture;
            // mBrush.layer=m_iLayer;
            static float fLastPaintTime;
            fLastPaintTime += evt.timeSinceLastFrame;
            if (fLastPaintTime > 0.3)
            {
                mTerrainMgr->tileSplat(x, z, mBrush);
                fLastPaintTime = 0;
            }
        }
        else if (m_EditType == ET_SceneAdd)
        {
        }
    }
    return true;
}

MyGUI::MouseButton    OISButton2MyGUI(OIS::MouseButtonID id)
{
    MyGUI::MouseButton MyGUIid;
    switch (id)
    {
    case OIS::MouseButtonID::MB_Left:
        MyGUIid = MyGUI::MouseButton::Left;
        break;
    case OIS::MouseButtonID::MB_Right:
        MyGUIid = MyGUI::MouseButton::Right;
        break;
    case OIS::MouseButtonID::MB_Middle:
        MyGUIid = MyGUI::MouseButton::Middle;
        break;
    default:
        MyGUIid = MyGUI::MouseButton::None;
    }
    return MyGUIid;
}

bool CProcedure::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    mCursorX = arg.state.X.abs;
    mCursorY = arg.state.Y.abs;
    if (injectMousePress(mCursorX, mCursorY, MyGUI::MouseButton::Enum(id)))
    {
        return true;
    }
    // Left mouse button down
    if ( id == OIS::MB_Left )
    {
        mLMouseDown = true;
        if (m_EditType == ET_SceneEdit)
        {
            Entity*        rayResult = NULL;
            Vector3        hitPoint;
            SceneNode*     pSn = NULL;
            MaterialPtr    mtr;
            MaterialPtr    newMtr;
            Ray            mouseRay = mCamera->getCameraToViewportRay(mCursorX / (float)arg.state.width, mCursorY / (float)arg.state.height);
            RaySceneQuery* rayQuery = mSceneMgr->createRayQuery(Ray());
            if (PickEntity(rayQuery, mouseRay, &rayResult, hitPoint))
            {
                pSn = rayResult->getParentSceneNode();               // find a entit
                Pass* newPass;
                if (m_pSelectedEntity == rayResult)
                {
                    return true;
                }
                if (m_pSelectedEntity != NULL)
                {
                    for (unsigned int i = 0; i < m_pSelectedEntity->getNumSubEntities(); ++i)
                    {
                        // 将先前选择的物体效果移除
                        m_pSelectedEntity->getSubEntity(i)->setMaterialName(m_OldMaterialVec[i]);
                    }
                }
                /*
                //法线预备
                if (rayResult->getSubEntity(i)->)
                {
                }*/
                m_OldMaterialVec.clear();
                m_NewMaterialVec.clear();
                for (unsigned int i = 0; i < rayResult->getNumSubEntities(); ++i)
                {
                    mtr = rayResult->getSubEntity(i)->getMaterial();

                    newMtr = mtr->clone(mtr->getName() + Ogre::StringConverter::toString(m_iMtrNum++));
                    m_NewMaterialVec.push_back(newMtr);
                    m_OldMaterialVec.push_back(mtr->getName());
                    // m_NewMaterialVec=m_pOldMaterial;
                    newPass = m_NewMaterialVec[i]->getTechnique(0)->createPass();
                    // m_NewMaterialVec[i]->getTechnique(0)->movePass(0,1);
                    // newPass=material->getTechnique(0)->getPass(0);
                    // newPass->setDepthWriteEnabled(false);//这里不写入Zbuffer，使原先的材质可以将主体覆盖，只显示边缘
                    newPass->setCullingMode(CULL_NONE);
                    newPass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
                    newPass->setVertexProgram("EdgeHighLightVS");
                    newPass->setFragmentProgram("EdgeHighLightPS");
                    // 为被选中的物体单独创建一个新材质
                    rayResult->getSubEntity(i)->setMaterial(m_NewMaterialVec[i]);
                    // newPass->setVertexProgram("SelectRectVS");
                    // newPass->setFragmentProgram("SelectRectPS");
                }
                m_pSelectedEntity = rayResult;
                if (pSn)
                {
                    if (m_pSelectedSceneNode)
                    {
                        m_pSelectedSceneNode->showBoundingBox(false);
                    }
                    pSn->showBoundingBox(true);
                    m_pSelectedSceneNode = pSn;
                }
            }
            /*
            //框选精度不够，暂时放弃
            deselectObjects();
            mStart.x =mCursorX/ (float)arg.state.width;
            mStart.y =mCursorY/ (float)arg.state.height;
            mStop = mStart;
            mSelecting = true;
            mRect->clear();
            mRect->setVisible(true);*/
        }
    }
    // Middle mouse button down
    else if ( id == OIS::MB_Middle )
    {
        // CEGUI::MouseCursor::getSingleton().hide( );
        mMMouseDown = true;
    }

    else if (id == OIS::MB_Right)
    {
        // if(CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id)))
        // return true;
        mRMouseDown = true;
    }

    return true;
}

bool CProcedure::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    mCursorX = arg.state.X.abs;
    mCursorY = arg.state.Y.abs;
    if ( id == OIS::MB_Left )
    {
        if (mSelecting && m_EditType == ET_SceneEdit)
        {
            performSelection(mStart, mStop);
            mSelecting = false;
            mRect->setVisible(false);
        }
    }
    // injectMouseRelease(mCursorX, mCursorY, MyGUI::MouseButton::Enum(id));
    if (injectMouseRelease(mCursorX, mCursorY, MyGUI::MouseButton::Enum(id)))
    {
        return true;
    }
    // Left mouse button up
    if ( id == OIS::MB_Left )
    {
        // if(CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id)))
        // return true;
        mLMouseDown = false;
        // after completed deformation steps, recalculate the lightmap
        if ((m_EditType == ET_Deform) && (m_iFrame >= m_iLastFrame + m_iUpadteCounts))
        {
            m_iLastFrame = m_iFrame;
            updateLightmap();
        }
        else if (m_EditType == ET_SceneAdd)
        {
            Ray                      mouseRay =
                mCamera->getCameraToViewportRay(arg.state.X.abs / float(arg.state.width), arg.state.Y.abs / float(arg.state.height));
            std::pair<bool, Vector3> result = mTerrainInfo->rayIntersects(mouseRay);
            if (result.first)
            {
                // 加入新的物体
                std::string name;
                name = mNewEntityName + StringConverter::toString(m_iCount++);
                Entity*     ent = mSceneMgr->createEntity(name, mNewEntityName);

                /*Ogre::AnimationStateSet* allset =ent->getAllAnimationStates();
                if(allset)
                {
                        Ogre::AnimationStateIterator iter=allset->getAnimationStateIterator();
                        Ogre::AnimationState* Anim=iter.getNext();
                        //if(Anim.mAnimationName)
                        {
                                Anim->setEnabled(true);
                                Anim->setLoop(true);
                                m_AnimationStateVec.push_back(Anim);
                        }
                }*/
                mCurrentObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(std::string(name) + "Node",
                                                                                     result.second);
                mCurrentObject->attachObject(ent);

                // mCurrentObject->setScale(0.03f,0.03f,0.03f);
                ent->setQueryFlags(Objiect_MASK);
                // ent->setCastShadows(false);
                // mAnimationState = ent->getAnimationState("Walk");
                // mAnimationState->setLoop(true);
                // mAnimationState->setEnabled(true);
            }
        }
    }

    // Middle mouse button up
    else if ( id == OIS::MB_Middle )
    {
        // CEGUI::MouseCursor::getSingleton().show( );
        mMMouseDown = false;
    }

    else if (id == OIS::MB_Right)
    {
        mRMouseDown = false;
        // after completed deformation steps, recalculate the lightmap
        if ((m_EditType == ET_Deform) && (m_iFrame >= m_iLastFrame + m_iUpadteCounts))
        {
            m_iLastFrame = m_iFrame;
            updateLightmap();
        }
    }
    // CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
    return true;
}

void CProcedure::checkPostion()
{
    const OIS::MouseState& ms = mMouse->getMouseState();

    if (mCursorX < 0)
    {
        mCursorX = 0;
    }
    else if (mCursorX >= ms.width)
    {
        mCursorX = ms.width - 1;
    }

    if (mCursorY < 0)
    {
        mCursorY = 0;
    }
    else if (mCursorY >= ms.height)
    {
        mCursorY = ms.height - 1;
    }
}

bool CProcedure::mouseMoved(const OIS::MouseEvent& arg)
{
    mCursorX = arg.state.X.abs;
    mCursorY = arg.state.Y.abs;
    // checkPostion();
    injectMouseMove(mCursorX, mCursorY, arg.state.Z.abs);
    // if(injectMouseMove(mCursorX, mCursorY, arg.state.Z.abs))
    //	return true;
    // terrain to which the mouse is currently pointing
    Ray                      mouseRay =
        mCamera->getCameraToViewportRay(arg.state.X.abs / float(arg.state.width), arg.state.Y.abs / float(arg.state.height));
    // since ETM is no longer a scene manager, we don't use a ray scene query,
    // but instead query the terrain info directly
    std::pair<bool, Vector3> result = mTerrainInfo->rayIntersects(mouseRay);
    if (result.first)
    {
        // update pointer's position
        mPointer->setPosition(result.second);
    }
    // If we are dragging the middle mouse button.
    if ( mMMouseDown )
    {
        mCamera->yaw(Degree(-arg.state.X.rel * mRotateSpeed));
        mCamera->pitch(Degree(-arg.state.Y.rel * mRotateSpeed));
    }
    if (m_EditType == ET_Paint)
    {
        if (arg.state.Z.rel > 0)
        {
            mBrush.imageID++;
            if (mBrush.imageID > mTerrainMgr->getImgNum())
            {
                mBrush.imageID = 0;
            }
        }
        else if (arg.state.Z.rel < 0)
        {
            mBrush.imageID--;
            if (mBrush.imageID < 0)
            {
                mBrush.imageID = mTerrainMgr->getImgNum();
            }
        }
        // 在界面中显示所选择的响应Atlas图片
        // 20110520 ~不容易啊，走到这一步
        mMainPanel->SetAtlasViewIndex(mBrush.imageID);
    }
    if (m_EditType == ET_SceneEdit)
    {
        if (m_pSelectedSceneNode != NULL)
        {
            // 拖放
            if (mLMouseDown)
            {
                float   m_fMoveSpeed = 1.0f;
                Vector3 pos          = m_pSelectedSceneNode->getPosition();
                if (mLShiftDown)
                {
                    pos.y += arg.state.X.rel * m_fMoveSpeed - arg.state.Y.rel * m_fMoveSpeed;
                }
                else if (mLAltDown)
                {
                    pos.x += arg.state.X.rel * m_fMoveSpeed + arg.state.Y.rel * m_fMoveSpeed;
                }
                else if (mLCtrlDown)
                {
                    pos.z += arg.state.X.rel * m_fMoveSpeed + arg.state.Y.rel * m_fMoveSpeed;
                }
                else if (mLCaptial)
                {
                    pos = mPointer->getPosition();
                }
                m_pSelectedSceneNode->setPosition(pos);
            }
            // 旋转
            else if (mRMouseDown)
            {
                if (mLShiftDown)
                {
                    m_pSelectedSceneNode->roll(Degree(-arg.state.X.rel * mRotateSpeed) + Degree(-arg.state.Y.rel * mRotateSpeed));
                }
                else if (mLCtrlDown)
                {
                    m_pSelectedSceneNode->pitch(Degree(-arg.state.X.rel * mRotateSpeed) + Degree(-arg.state.Y.rel * mRotateSpeed));
                }
                else if (mLAltDown)
                {
                    m_pSelectedSceneNode->yaw(Degree(-arg.state.X.rel * mRotateSpeed) + Degree(-arg.state.Y.rel * mRotateSpeed));
                }
            }
            // 缩放
            if (arg.state.Z.rel > 0)
            {
                m_pSelectedSceneNode->scale(0.9f, 0.9f, 0.9f);
            }
            else if (arg.state.Z.rel < 0)
            {
                m_pSelectedSceneNode->scale(1.1f, 1.1f, 1.1f);
            }
        }
        if (mSelecting)
        {
            // deselectObjects();
            mStop.x = mCursorX / (float)arg.state.width;
            mStop.y = mCursorY / (float)arg.state.height;

            mRect->setCorners(mStart, mStop);
            /*Vector2 mFrst=mStart;
            Vector2 mSecond=mStop;
            if (mFrst.x>mSecond.x)
                    swap(mFrst.x,mSecond.x) ;
            if (mFrst.y> mSecond.y)
                    swap(mFrst.y, mSecond.y);
            //performSelection(mStart, mStop);//实时显示框选结果
            mRect->setCorners(mFrst, mSecond);*/
        }
    }
    return true;
}

void CProcedure::performSelection(const Vector2& first, const Vector2& second)
{
    float left = first.x, right = second.x,
          top  = first.y, bottom = second.y;
    if (left > right)
    {
        swap(left, right);
    }
    if (top > bottom)
    {
        swap(top, bottom);
    }
    if ((right - left) * (bottom - top) < 0.0001)
    {
        return;
    }
    LogManager::getSingleton().logMessage("当前起始点：【" + Ogre::StringConverter::toString(mStart.x) + "," +
                                          Ogre::StringConverter::toString(mStart.y) +
                                          "】结束点：【" + Ogre::StringConverter::toString(mStop.x) + "," + Ogre::StringConverter::toString(
                                              mStop.y) + "】");
    Ray                                   topLeft     = mCamera->getCameraToViewportRay(left, top);
    Ray                                   topRight    = mCamera->getCameraToViewportRay(right, top);
    Ray                                   bottomLeft  = mCamera->getCameraToViewportRay(left, bottom);
    Ray                                   bottomRight = mCamera->getCameraToViewportRay(right, bottom);
    PlaneBoundedVolume                    vol;
    vol.planes.push_back(Plane(topLeft.getPoint(3), topRight.getPoint(3), bottomRight.getPoint(3)));             // 前平面
    vol.planes.push_back(Plane(topLeft.getOrigin(), topLeft.getPoint(10), topRight.getPoint(10)));             // 顶平面
    vol.planes.push_back(Plane(topLeft.getOrigin(), bottomLeft.getPoint(10), topLeft.getPoint(10)));           // 左平面
    vol.planes.push_back(Plane(topLeft.getOrigin(), bottomRight.getPoint(10), bottomLeft.getPoint(10)));       // 底平面
    vol.planes.push_back(Plane(topLeft.getOrigin(), topRight.getPoint(10), bottomRight.getPoint(10)));         // 右平面

    PlaneBoundedVolumeList                volList;
    volList.push_back(vol);

    mVolQuery->setVolumes(volList);
    // mVolQuery->setQueryMask(Objiect_MASK);
    mVolQuery->setQueryTypeMask(SceneManager::ENTITY_TYPE_MASK);
    SceneQueryResult                      result = mVolQuery->execute();
    SceneQueryResultMovableList::iterator itr;
    for (itr = result.movables.begin(); itr != result.movables.end(); ++itr)
    {
        selectObject(*itr);
    }
}

void CProcedure::deselectObjects()
{
    std::list<MovableObject*>::iterator itr;
    for (itr = mSelected.begin(); itr != mSelected.end(); ++itr)
    {
        (*itr)->getParentSceneNode()->showBoundingBox(false);
    }
    mSelected.clear();
}

void CProcedure::selectObject(MovableObject* obj)
{
    obj->getParentSceneNode()->showBoundingBox(true);
    mSelected.push_back(obj);
}

void CProcedure::SwithMode(EditType newType, bool bChangedFromKeyBoard)
{
    m_OldEditType = m_EditType;
    if (m_OldEditType == ET_SceneEdit)
    {
        deselectObjects();
    }
    m_EditType = newType;
    mPointer->detachAllObjects();
    if (m_EditType == ET_Paint)
    {
        mPointer->attachObject(mSceneMgr->getEntity("PointerPaint"));
    }
    else if (m_EditType == ET_Deform)
    {
        mPointer->attachObject(mSceneMgr->getEntity("PointerDeform"));
    }
    else if (m_EditType == ET_SceneAdd)
    {
        mPointer->attachObject(mSceneMgr->getEntity("PointerSceneAdd"));
    }
    else if (m_EditType == ET_SceneEdit)
    {
        // mPointer->attachObject(mSceneMgr->getEntity("PointerSceneEdit"));
    }
    if (bChangedFromKeyBoard)
    {
        mMainPanel->mTabControl->setIndexSelected(m_EditType);
        /*
        using namespace CEGUI;
        WindowManager& winMgr = WindowManager::getSingleton ();
        TabControl *tc = NULL;
        if (winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
        {
        tc = static_cast<TabControl *>(winMgr.getWindow (
        "TabControlDemo/Frame/TabControl"));
        //                        int iItem=tc->getSelectedTabIndex();
        //                        if(iItem!=(int)m_EditType)
        tc->setSelectedTabAtIndex(m_EditType);
        }*/
    }
}

void CProcedure::OnGUITreeItemSelected(MyGUI::TreeControl* pTreeControl, MyGUI::TreeControl::Node* pNode)
{
    if (m_EditType != ET_SceneAdd)
    {
        return;
    }
    if (pNode->getChildren().size() > 0)
    {
        return;
    }
    // PairFileInfo info = *(pNode->getData<PairFileInfo>());
    if (pNode->getText().size() > 0)
    {
        MyGUI::UString path = ((MyGUI::TreeControl::Node*)(pNode->getParent()))->getText() + MyGUI::UString("/") + pNode->getText();
        SetSceneAddType(UTF8ToANSI(path.asUTF8_c_str()));
    }
}

void CProcedure::SetSceneAddType(std::string ObjectName)
{
    if (m_EditType != ET_SceneAdd)
    {
        return;
    }
    mPointer->detachAllObjects();
    Entity*     newPoint;
    std::string EntityName = "PointerSceneEdit" + ObjectName;
    if (mSceneMgr->hasEntity(EntityName))
    {
        newPoint = mSceneMgr->getEntity(EntityName);
    }
    else
    {
        newPoint = mSceneMgr->createEntity(EntityName, ObjectName);
    }
    mPointer = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mPointer->attachObject(newPoint);
    // mPointer->setScale(0.03f,0.03f,0.03f);
    mNewEntityName = ObjectName;
#ifndef _DEBUG
    mMainPanel->mRenderBoxScene.injectObject(ObjectName);
#endif
}

void CProcedure::OnGUISwitchMode()
{
    /*
    using namespace CEGUI;
    WindowManager& winMgr = WindowManager::getSingleton ();
    TabControl *tc = NULL;
    if (winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
    {
    tc = static_cast<TabControl *>(winMgr.getWindow (
    "TabControlDemo/Frame/TabControl"));
    int iItem=tc->getSelectedTabIndex();
    if(iItem!=(int)m_OldEditType)
    SwithMode((EditType)iItem);
    }*/
}

#if MYGUI_PLATFORM == MYGUI_PLATFORM_WIN32
MyGUI::Char translateWin32Text(MyGUI::KeyCode kc)
{
    static WCHAR deadKey = 0;

    BYTE         keyState[256];
    HKL          layout = GetKeyboardLayout(0);
    if ( GetKeyboardState(keyState) == 0 )
    {
        return 0;
    }

    int          code = *((int*)&kc);
    unsigned int vk   = MapVirtualKeyEx((UINT)code, 3, layout);
    if ( vk == 0 )
    {
        return 0;
    }

    WCHAR        buff[3] = { 0, 0, 0 };
    int          ascii   = ToUnicodeEx(vk, (UINT)code, keyState, buff, 3, 0, layout);
    if (ascii == 1 && deadKey != '\0' )
    {
        // A dead key is stored and we have just converted a character key
        // Combine the two into a single character
        WCHAR wcBuff[3] = { buff[0], deadKey, '\0' };
        WCHAR out[3];

        deadKey = '\0';
        if (FoldStringW(MAP_PRECOMPOSED, (LPWSTR)wcBuff, 3, (LPWSTR)out, 3))
        {
            return out[0];
        }
    }
    else if (ascii == 1)
    {
        // We have a single character
        deadKey = '\0';
        return buff[0];
    }
    else if (ascii == 2)
    {
        // Convert a non-combining diacritical mark into a combining diacritical mark
        // Combining versions range from 0x300 to 0x36F; only 5 (for French) have been mapped below
        // http://www.fileformat.info/info/unicode/block/combining_diacritical_marks/images.htm
        switch (buff[0])
        {
        case 0x5E:         // Circumflex accent: ?
            deadKey = 0x302;
            break;
        case 0x60:         // Grave accent: ?
            deadKey = 0x300;
            break;
        case 0xA8:         // Diaeresis: ?
            deadKey = 0x308;
            break;
        case 0xB4:         // Acute accent: ?
            deadKey = 0x301;
            break;
        case 0xB8:         // Cedilla: ?
            deadKey = 0x327;
            break;
        default:
            deadKey = buff[0];
            break;
        }
    }

    return 0;
}
#endif

bool CProcedure::keyPressed(const OIS::KeyEvent& arg)
{
    MyGUI::Char    text      = (MyGUI::Char)arg.text;
    MyGUI::KeyCode key       = MyGUI::KeyCode::Enum(arg.key);
    int            scan_code = key.getValue();
    if (scan_code > 70 && scan_code < 84)
    {
        static MyGUI::Char nums[13] = { 55, 56, 57, 45, 52, 53, 54, 43, 49, 50, 51, 48, 46 };
        text = nums[scan_code - 71];
    }
    else if (key == MyGUI::KeyCode::Divide)
    {
        text = '/';
    }
    else
    {
#if MYGUI_PLATFORM == MYGUI_PLATFORM_WIN32
        text = translateWin32Text(key);
#endif
    }
    injectKeyPress(key, text);
    // return;

    // we'll use the keys 1, 2, 3, 4 and E to switch between
    // edit modes and select a paint texture
    // WSAD is used for movement
    // O will save the changes to disk
    // key down
    /*
    if(CEGUI::System::getSingleton().injectKeyDown(arg.key))
    return true;
    if(CEGUI::System::getSingleton().injectChar(arg.key))
    return true;*/

    // now character
    if (m_EditType == ET_Paint)
    {
        switch (arg.key)
        {
        case OIS::KC_TAB:
            mBrush.layer++;
            if (mBrush.layer > 1)
            {
                mBrush.layer = 0;
            }
            break;
        case OIS::KC_1:
            mBrush.imageID = 0;
            break;
        case OIS::KC_2:
            mBrush.imageID = 1;
            break;
        case OIS::KC_3:
            mBrush.imageID = 2;
            break;
        case OIS::KC_4:
            mBrush.imageID = 3;
            break;
        case OIS::KC_5:
            mBrush.imageID = 4;
            break;
        case OIS::KC_6:
            mBrush.imageID = 5;
            break;
        default:
            break;
        }
    }
    switch (arg.key)
    {
    case OIS::KC_LSHIFT:
        mLShiftDown = true;
        return true;
    case OIS::KC_LCONTROL:
        mLCtrlDown = true;
        return true;
    case OIS::KC_LMENU:
        mLAltDown = true;
        return true;
    case OIS::KC_CAPITAL:
        mLCaptial = true;
        return true;
    case OIS::KC_W:
        mDirection.z += -1;
        return true;
    case OIS::KC_S:
        mDirection.z += 1;
        return true;
    case OIS::KC_A:
        mDirection.x += -1;
        return true;
    case OIS::KC_D:
        mDirection.x += 1;
        return true;
    case OIS::KC_P:
        saveTerrain();
        return true;
    case OIS::KC_O:
        loadTerrain();
        return true;
    case OIS::KC_ESCAPE:
        m_bQuit = true;
        return true;
    case OIS::KC_F1:
        SwithMode(ET_Deform, true);
        return true;
    case OIS::KC_F2:
        SwithMode(ET_Paint, true);
        return true;
    case OIS::KC_F3:
        SwithMode(ET_SceneAdd, true);
        return true;
    case OIS::KC_F4:
        SwithMode(ET_SceneEdit, true);
        return true;

    case OIS::KC_F5:     // 显示程序参数
        mTrayMgr->toggleAdvancedFrameStats();
        return true;
    case OIS::KC_F6:     // 显示帮助
        if (mHelpText->getTrayLocation() == BloodRainBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mHelpText, BloodRainBites::TL_TOPRIGHT, 0);
            mHelpText->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mHelpText);
            mHelpText->hide();
        }
        break;
    case OIS::KC_F7:     // 显示详细信息
        if (mDetailsPanel->getTrayLocation() == BloodRainBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, BloodRainBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
        return true;
    case OIS::KC_F8:     // 显示线框模式
        {
            std::string       newVal;
            Ogre::PolygonMode pm;
            switch (mCamera->getPolygonMode())
            {
            case Ogre::PM_SOLID:
                newVal = "Wireframe";
                pm     = Ogre::PM_WIREFRAME;
                mSceneMgr->getSkyDomeNode()->setVisible(false);
                // mSceneMgr->setSkyDome(false, "Examples/CloudySky", 5, 8);
                break;
            case Ogre::PM_WIREFRAME:
                newVal = "Points";
                pm     = Ogre::PM_POINTS;
                mSceneMgr->getSkyDomeNode()->setVisible(false);
                // mSceneMgr->setSkyDome(false, "Examples/CloudySky", 5, 8);
                break;
            default:
                newVal = "Solid";
                pm     = Ogre::PM_SOLID;
                mSceneMgr->getSkyDomeNode()->setVisible(true);
                // mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
            }
            mCamera->setPolygonMode(pm);
            mDetailsPanel->setParamValue(10, newVal);
        }
        return true;
    case OIS::KC_F9:       // refresh all textures
        {
            Ogre::TextureManager::getSingleton().reloadAll();
        }
        return true;
    case OIS::KC_F10:
        {
            // 0x13 Funny
            int shadowMode = mSceneMgr->getShadowTechnique();
            if ( ++shadowMode > SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED )
            {
                shadowMode = 0;
            }
            mSceneMgr->setShadowTechnique(ShadowTechnique(shadowMode));
        }
        return true;

    case OIS::KC_PAUSE:           // take a screenshot
        {
            mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
        }
        return true;
    default:
        return false;
    }
}

bool CProcedure::keyReleased(const OIS::KeyEvent& arg)
{
    injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
    // if(CEGUI::System::getSingleton().injectKeyUp(arg.key))
    //	return true;
    // not interested in this
    switch (arg.key)
    {
    case OIS::KC_W:
        mDirection.z -= -1;
        return true;
    case OIS::KC_S:
        mDirection.z -= 1;
        return true;
    case OIS::KC_A:
        mDirection.x -= -1;
        return true;
    case OIS::KC_D:
        mDirection.x -= 1;
        return true;
    case OIS::KC_LSHIFT:
        mLShiftDown = false;
        return true;
    case OIS::KC_LCONTROL:
        mLCtrlDown = false;
        return true;
    case OIS::KC_LMENU:
        mLAltDown = false;
        return true;
    case OIS::KC_CAPITAL:
        mLCaptial = false;
        return true;
    default:
        return false;
    }
}

void CProcedure::saveTerrain()
{
    // just for demonstration, we'll save anything we can
    // start with the terrain
    Image         image;
    ET::saveHeightmapToImage(*mTerrainInfo, image);
    image.save("../../Media/ET/ETterrain.png");

    // now save the splatting maps
    for (uint i = 0; i < mSplatMgr->getNumMaps(); ++i)
    {
        mSplatMgr->saveMapToImage(i, image);
        image.save("../../Media/ET/ETcoverage." + StringConverter::toString(i) + ".png");
    }

    // now the lightmap
    Image         lightmap;
    ET::createTerrainLightmap(*mTerrainInfo, lightmap, 512, 512, Vector3(1, -1, 1), ColourValue(1, 1, 1),
                              ColourValue(0.3f, 0.3f,  0.3f));
    lightmap.save("../../Media/ET/ETlightmap.png");

    // generate a base texture for this terrain (could be used for older hardware instead of splatting)
    // create an ImageList of our splatting textures.
    ET::ImageList textures(6);
    for (int i = 0; i < 6; ++i)
    {
        textures[i].load("splatting" + StringConverter::toString(i) + ".png", "ET");
    }
    // create the base texture
    Image         baseTexture;
    mSplatMgr->createBaseTexture(baseTexture, 512, 512, textures, 20, 20);
    baseTexture.save("../../Media/ET/ETbase.png");

    // finally create a minimap using the lightmap and the generated base texture
    Image         minimap = ET::createMinimap(baseTexture, lightmap);
    minimap.save("../../Media/ET/ETminimap.png");
}

void CProcedure::loadTerrain()
{
    // now we revert the process and load the data back from disk
    // recreate terrain...
    mTerrainMgr->destroyTerrain();
    Image               image;
    image.load("ETterrain.png", "ET");
    ET::TerrainInfo     info;
    ET::loadHeightmapFromImage(info, image);

    // --------------------------------------------------------------------
    // todo
    ET::TileTerrainInfo tileTerrainInfo;

    info.setExtents(AxisAlignedBox(0, 0, 0, 1500, 300, 1500));
    mTerrainMgr->createTerrain(info, tileTerrainInfo);
    mTerrainInfo = &mTerrainMgr->getTerrainInfo();

    // now load the splatting maps
    for (uint i = 0; i < mSplatMgr->getNumMaps(); ++i)
    {
        image.load("ETcoverage." + StringConverter::toString(i) + ".png", "ET");
        mSplatMgr->loadMapFromImage(i, image);
    }

    // update the lightmap
    updateLightmap();
}

void CProcedure::updateLightmap()
{
    Image      lightmap;
    ET::createTerrainLightmap(*mTerrainInfo, lightmap, 128, 128, Vector3(1, -1, 1), ColourValue(1, 1, 1),
                              ColourValue(0.3f, 0.3f,  0.3f));

    // get our dynamic texture and update its contents
    TexturePtr tex = TextureManager::getSingleton().getByName("ETLightmap");
    tex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
}

void CProcedure::shutdown()
{
    m_bQuit = true;
}

void CProcedure::CreateMyGUI()
{
    mPluginCfgName    = std::string("plugins.cfg");
    mResourceXMLName  = std::string("resources.xml");
    mResourceFileName = std::string("MyGUI_Core.xml");

    mPlatform = new MyGUI::OgrePlatform();
    mPlatform->initialise(mWindow, mSceneMgr);
    mGUI = new MyGUI::Gui();
    mGUI->initialise(mResourceFileName);

    mInfo      = new diagnostic::StatisticInfo();
    mFocusInfo = new diagnostic::InputFocusInfo();
}

const std::string& CProcedure::getRootMedia()
{
    return mRootMedia;
}

void CProcedure::setupMyGUIResources()
{
    MyGUI::xml::Document          doc;

    if (!doc.open(mResourceXMLName))
    {
        doc.getLastError();
    }

    MyGUI::xml::ElementPtr        root = doc.getRoot();
    if (root == nullptr || root->getName() != "Paths")
    {
        return;
    }

    MyGUI::xml::ElementEnumerator node = root->getElementEnumerator();
    while (node.next())
    {
        if (node->getName() == "Path")
        {
            bool rootAttr = false;
            if (node->findAttribute("root") != "")
            {
                rootAttr = MyGUI::utility::parseBool(node->findAttribute("root"));
                if (rootAttr)
                {
                    mRootMedia = node->getContent();
                }
            }
            addResourceLocation(node->getContent());
        }
    }
    addResourceLocation(getRootMedia() + "/Common/Base");

    addResourceLocation("../../media/MyGUI");
    addResourceLocation(getRootMedia() + "/Common/Tools");
}

void CProcedure::addResourceLocation(const std::string& _name, const std::string& _group, const std::string& _type, bool _recursive)
{
#if MYGUI_PLATFORM == MYGUI_PLATFORM_APPLE
    // OS X does not set the working directory relative to the app, In order to make things portable on OS X we need to provide the loading with it's own bundle path location
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(std::string(macBundlePath() + "/" + _name), _type, _group, _recursive);
#else
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(_name, _type, _group, _recursive);
#endif
}

void CProcedure::addResourceLocation(const std::string& _name, bool _recursive)
{
    addResourceLocation(_name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "FileSystem", _recursive);
}

void CProcedure::destroyGui()
{
    shutdownMyGUI();
    if (mGUI)
    {
        if (mInfo)
        {
            delete mInfo;
            mInfo = nullptr;
        }
        if (mFocusInfo)
        {
            delete mFocusInfo;
            mFocusInfo = nullptr;
        }
        mGUI->shutdown();
        delete mGUI;
        mGUI = nullptr;
    }
    if (mPlatform)
    {
        mPlatform->shutdown();
        delete mPlatform;
        mPlatform = nullptr;
    }
}

void CProcedure::shutdownMyGUI()
{
    // 箐嚯�屐 怦?觌囫覃
    for (VectorBasePtr::reverse_iterator iter = mListBase.rbegin(); iter != mListBase.rend(); ++iter)
    {
        delete (*iter);
    }
    mListBase.clear();

    // 箐嚯�屐 怦?痼蝾恹?忤滏弪?
    MyGUI::LayoutManager::getInstance().unloadLayout(mListWindowRoot);
    mListWindowRoot.clear();
}

void CProcedure::createMyGUIScene()
{
    std::string            mLocale = ::setlocale(LC_ALL, "chs");
    if (!mLocale.empty())
    {
        MyGUI::LanguageManager::getInstance().setCurrentLanguage(mLocale);
    }

    MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
    factory.registerFactory<MyGUI::TreeControl>("Widget");
    factory.registerFactory<MyGUI::TreeControlItem>("Widget");

    MyGUI::ResourceManager::getInstance().load("FrameworkFonts.xml");
    MyGUI::ResourceManager::getInstance().load("FrameworkSkin.xml");
    MyGUI::ResourceManager::getInstance().load("TreeControlSkin.xml");

    mMainPanel                                         = new MainPanel();
    mMainPanel->mpResourcesTree->eventTreeNodeSelected = MyGUI::newDelegate(this, &CProcedure::OnGUITreeItemSelected);
    mMainPanel->eventAction                            = MyGUI::newDelegate(this, &CProcedure::notifyEventAction);
    mMainPanel->addObject("FrameWindow");
    mMainPanel->addObject("Horizontal Scrollbar");
    mMainPanel->addObject("Vertical Scrollbar");
    mMainPanel->addObject("TextBox");
    mMainPanel->addObject("ImageBox");
    mMainPanel->addObject("Render to Texture");
}

void CProcedure::destroyMyGUIScene()
{
    shutdown();
    delete mMainPanel;
    mMainPanel = nullptr;

    MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
    factory.unregisterFactory<MyGUI::TreeControl>("Widget");
    factory.unregisterFactory<MyGUI::TreeControlItem>("Widget");
}

bool CProcedure::injectMouseMove(int _absx, int _absy, int _absz)
{
    if (!mGUI)
    {
        return false;
    }

    return MyGUI::InputManager::getInstance().injectMouseMove(_absx, _absy, _absz);
}

bool CProcedure::injectMousePress(int _absx, int _absy, MyGUI::MouseButton _id)
{
    if (!mGUI)
    {
        return false;
    }

    return MyGUI::InputManager::getInstance().injectMousePress(_absx, _absy, _id);
}

bool CProcedure::injectMouseRelease(int _absx, int _absy, MyGUI::MouseButton _id)
{
    if (!mGUI)
    {
        return false;
    }

    return MyGUI::InputManager::getInstance().injectMouseRelease(_absx, _absy, _id);
}

bool CProcedure::injectKeyPress(MyGUI::KeyCode _key, MyGUI::Char _text)
{
    if (!mGUI)
    {
        return false;
    }
    if (_key == MyGUI::KeyCode::Escape)
    {
        mExit = true;
        return false;
    }
    /*else if (_key == MyGUI::KeyCode::SysRq)
    {
        std::ifstream stream;
        std::string   file;
        do
        {
            stream.close();
            static size_t num      = 0;
            const size_t  max_shot = 100;
            if (num == max_shot)
            {
                MYGUI_LOG(Info, "The limit of screenshots is exceeded : " << max_shot);
                return true;
            }
            file = MyGUI::utility::toString("screenshot_", ++num, ".png");
            stream.open(file.c_str());
        }
        while (stream.is_open());
        mWindow->writeContentsToFile(file);
        return true;
    }*/
    /*else if (_key == MyGUI::KeyCode::F12)
    {
        bool visible = mFocusInfo->getFocusVisible();
        mFocusInfo->setFocusVisible(!visible);
    }*/
    return MyGUI::InputManager::getInstance().injectKeyPress(_key, _text);
}

bool CProcedure::injectKeyRelease(MyGUI::KeyCode _key)
{
    if (!mGUI)
    {
        return false;
    }

    return MyGUI::InputManager::getInstance().injectKeyRelease(_key);
}

void CProcedure::notifyEventAction(MainPanel::TypeEvents _action, size_t _index)
{
    if (_action == MainPanel::EventQuit)
    {
        shutdown();
    }
    else if (_action == MainPanel::EventTabChangeSelect)
    {
        if (_index != (int)m_OldEditType)
        {
            SwithMode((EditType)_index);
        }
    }
}

bool CProcedure::processUnbufferedKeyInput(const FrameEvent& evt)
{
    Real moveScale = mMoveScale;
    if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
    {
        moveScale *= 10;
    }

    if (mKeyboard->isKeyDown(OIS::KC_A))
    {
        mTranslateVector.x = -moveScale;                            // Move camera left
    }
    if (mKeyboard->isKeyDown(OIS::KC_D))
    {
        mTranslateVector.x = moveScale;                     // Move camera RIGHT
    }
    if (mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W))
    {
        mTranslateVector.z = -moveScale;                            // Move camera forward
    }
    if (mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S))
    {
        mTranslateVector.z = moveScale;                     // Move camera backward
    }
    if (mKeyboard->isKeyDown(OIS::KC_PGUP))
    {
        mTranslateVector.y = moveScale;                     // Move camera up
    }
    if (mKeyboard->isKeyDown(OIS::KC_PGDOWN))
    {
        mTranslateVector.y = -moveScale;                            // Move camera down
    }
    if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
    {
        mCamera->yaw(-mRotScale);
    }

    if (mKeyboard->isKeyDown(OIS::KC_LEFT))
    {
        mCamera->yaw(mRotScale);
    }

    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q))
    {
        return false;
    }

    if (mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0)
    {
        mStatsOn = !mStatsOn;
        showDebugOverlay(mStatsOn);
        mTimeUntilNextToggle = 1;
    }

    if (mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0)
    {
        switch (mFiltering)
        {
        case TFO_BILINEAR:
            mFiltering = TFO_TRILINEAR;
            mAniso     = 1;
            break;
        case TFO_TRILINEAR:
            mFiltering = TFO_ANISOTROPIC;
            mAniso     = 8;
            break;
        case TFO_ANISOTROPIC:
            mFiltering = TFO_BILINEAR;
            mAniso     = 1;
            break;
        default: break;
        }
        MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
        MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);

        showDebugOverlay(mStatsOn);
        mTimeUntilNextToggle = 1;
    }

    if (mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
    {
        std::ostringstream ss;
        ss << "screenshot_" << ++mNumScreenShots << ".png";
        mWindow->writeContentsToFile(ss.str());
        mTimeUntilNextToggle = 0.5;
        mDebugText           = "Saved: " + ss.str();
    }

    if (mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <= 0)
    {
        mSceneDetailIndex = (mSceneDetailIndex + 1) % 3;
        switch (mSceneDetailIndex)
        {
        case 0: mCamera->setPolygonMode(PM_SOLID); break;
        case 1: mCamera->setPolygonMode(PM_WIREFRAME); break;
        case 2: mCamera->setPolygonMode(PM_POINTS); break;
        }
        mTimeUntilNextToggle = 0.5;
    }

    static bool displayCameraDetails = false;
    if (mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
    {
        displayCameraDetails = !displayCameraDetails;
        mTimeUntilNextToggle = 0.5;
        if (!displayCameraDetails)
        {
            mDebugText = "";
        }
    }

    // Print camera details
    if (displayCameraDetails)
    {
        mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
                     " " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());
    }

    // Return true to continue rendering
    return true;
}

bool CProcedure::processUnbufferedMouseInput(const FrameEvent& evt)
{
    // Rotation factors, may not be used if the second mouse button is pressed
    // 2nd mouse button - slide, otherwise rotate
    const OIS::MouseState& ms = mMouse->getMouseState();
    if (ms.buttonDown(OIS::MB_Right))
    {
        mTranslateVector.x += ms.X.rel * 0.13;
        mTranslateVector.y -= ms.Y.rel * 0.13;
    }
    else
    {
        mRotX = Degree(-ms.X.rel * 0.13);
        mRotY = Degree(-ms.Y.rel * 0.13);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        // Adjust the input depending upon viewport orientation
        Radian origRotY, origRotX;
        switch (mCamera->getViewport()->getOrientation())
        {
        case Viewport::OR_LANDSCAPELEFT:
            origRotY = mRotY;
            origRotX = mRotX;
            mRotX    = origRotY;
            mRotY    = -origRotX;
            break;
        case Viewport::OR_LANDSCAPERIGHT:
            origRotY = mRotY;
            origRotX = mRotX;
            mRotX    = -origRotY;
            mRotY    = origRotX;
            break;

        // Portrait doesn't need any change
        case Viewport::OR_PORTRAIT:
        default:
            break;
        }
#endif
    }

    return true;
}

void CProcedure::moveCamera()
{
    // Make all the changes to the camera
    // Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
    // (e.g. airplane)
    mCamera->yaw(mRotX);
    mCamera->pitch(mRotY);
    mCamera->moveRelative(mTranslateVector);
}

void CProcedure::showDebugOverlay(bool show)
{
    if ( mTrayMgr )
    {
        if (show)
        {
            mTrayMgr->showTrays();
        }
        else
        {
            mTrayMgr->hideTrays();
        }
    }
}

// Override frameRenderingQueued event to process that (don't care about frameEnded)
bool CProcedure::frameRenderingQueued(const FrameEvent& evt)
{
    if (mWindow->isClosed())
    {
        return false;
    }

    mSpeedLimit = mMoveScale * evt.timeSinceLastFrame;

    // Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
    if (mJoy)
    {
        mJoy->capture();
    }

    bool          buffJ = (mJoy) ? mJoy->buffered() : true;

    Ogre::Vector3 lastMotion = mTranslateVector;

    // Check if one of the devices is not buffered
    if (!mMouse->buffered() || !mKeyboard->buffered() || !buffJ)
    {
        // one of the input modes is immediate, so setup what is needed for immediate movement
        if (mTimeUntilNextToggle >= 0)
        {
            mTimeUntilNextToggle -= evt.timeSinceLastFrame;
        }

        // Move about 100 units per second
        mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
        // Take about 10 seconds for full rotation
        mRotScale = mRotateSpeed * evt.timeSinceLastFrame;

        mRotX            = 0;
        mRotY            = 0;
        mTranslateVector = Ogre::Vector3::ZERO;
    }

    // Check to see which device is not buffered, and handle it
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    if (!mKeyboard->buffered())
    {
        if (processUnbufferedKeyInput(evt) == false)
        {
            return false;
        }
    }

#ifdef INCLUDE_RTSHADER_SYSTEM
    processShaderGeneratorInput();
#endif

#endif
    if (!mMouse->buffered())
    {
        if (processUnbufferedMouseInput(evt) == false)
        {
            return false;
        }
    }

    // ramp up / ramp down speed
    if (mTranslateVector == Ogre::Vector3::ZERO)
    {
        // decay (one third speed)
        mCurrentSpeed   -= evt.timeSinceLastFrame * 0.3;
        mTranslateVector = lastMotion;
    }
    else
    {
        // ramp up
        mCurrentSpeed += evt.timeSinceLastFrame;
    }
    // Limit motion speed
    if (mCurrentSpeed > 1.0)
    {
        mCurrentSpeed = 1.0;
    }
    if (mCurrentSpeed < 0.0)
    {
        mCurrentSpeed = 0.0;
    }

    mTranslateVector *= mCurrentSpeed;


    if (!mMouse->buffered() || !mKeyboard->buffered() || !buffJ)
    {
        moveCamera();
    }

    return true;
}

bool CProcedure::frameEnded(const FrameEvent& evt)
{
    updateStats();
    return true;
}
