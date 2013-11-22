#ifndef HydraxListenerHead
#define HydraxListenerHead
#pragma  once
// ************************************
// 函数名称: BloodRain3D
// 函数说明：千里之行始于足下
// 作 成 者：何牧
// 加入日期：2011/05/07
// ************************************
// 你看你，都搁置多久了？！

/*
--------------------------------------------------------------------------------
This source file is part of Hydrax.
Visit ---

Copyright (C) 2009 Xavier Vergu韓 Gonz醠ez <xavierverguin@hotmail.com>
<xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------
*/

// ----------------------------------------------------------------------------
// Include the main OGRE header files
// Ogre.h just expands to including lots of individual OGRE header files
// ----------------------------------------------------------------------------
#include <Ogre.h>
#include <OgreTextAreaOverlayElement.h>

// ----------------------------------------------------------------------------
// Include the OGRE example framework
// This includes the classes defined to make getting an OGRE application running
// a lot easier. It automatically sets up all the main objects and allows you to
// just override the bits you want to instead of writing it all from scratch.
// ----------------------------------------------------------------------------
#include <MyAppFrame.h>

// ----------------------------------------------------------------------------
// Include the Hydrax plugin headers
// Main base headers (Hydrax.h) and especific headers (Noise/Water modules)
// ----------------------------------------------------------------------------
#include "Hydrax.h"
#include "Perlin.h"
#include "ProjectedGrid.h"

#define _def_SkyBoxNum 3



// ----------------------------------------------------------------------------
// Define the application object
// This is derived from MyApplication which is the class OGRE provides to
// make it easier to set up OGRE without rewriting the same code all the time.
// You can override extra methods of MyApplication if you want to further
// specialise the setup routine, otherwise the only mandatory override is the
// 'createScene' method which is where you set up your own personal scene.
// ----------------------------------------------------------------------------

class CHydraxListener : public MyFrameListener, public OIS::KeyListener
{
private:
    SceneManager* mSceneMgr;
    Real          mKeyBuffer;
    Camera*       mCamera;
    RenderWindow* mWindow;
    bool          m_bPause;              // 是否暂停渲染HydraX
public:
    CHydraxListener(RenderWindow* win, Camera* cam, SceneManager* sm);
    // Just override the mandatory create scene method
    void createScene(void);
    // Create text area to show skyboxes information
    void createTextArea();
    virtual bool mouseMoved(const OIS::MouseEvent& arg);
    // bool frameRenderingQueued(const FrameEvent& evt);
    bool frameStarted(const FrameEvent& e);
    virtual bool keyPressed(const OIS::KeyEvent& arg);
    virtual bool keyReleased(const OIS::KeyEvent& arg);
    void changeSkyBox();
};




#endif
