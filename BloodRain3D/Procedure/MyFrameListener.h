/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2013 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    MyFrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
                         R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
             P:        Toggle on/off display of camera position / orientation
                         F2:	   Set the main viewport material scheme to default material manager scheme.
                         F3:	   Set the main viewport material scheme to shader generator default scheme.
                         F4:	   Toggle default shader generator lighting model from per vertex to per pixel.
-----------------------------------------------------------------------------
*/

#ifndef __MyFrameListener_H__
#define __MyFrameListener_H__



#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreOverlaySystem.h"


// Use this define to signify OIS will be used as a DLL
// (so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS.h>
#include "MyInputContext.h"

using namespace Ogre;

#ifdef INCLUDE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"
#endif

class MyFrameListener : public FrameListener, public WindowEventListener
{
public:
    // Constructor takes a RenderWindow because it uses that to determine input context
    MyFrameListener(RenderWindow* win, Camera* cam, bool bufferedKeys = false, bool bufferedMouse = false,
                    bool bufferedJoy = false) :
        mCamera(cam), mTranslateVector(Vector3::ZERO), mCurrentSpeed(0), mWindow(win), mStatsOn(true), mNumScreenShots(0),
        mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
        mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36),
        mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0)
    {
        LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
        OIS::ParamList     pl;
        size_t             windowHnd = 0;
        std::ostringstream windowHndStr;

        win->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        mInputManager = OIS::InputManager::createInputSystem(pl);

        // Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, bufferedKeys));
        mMouse    = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, bufferedMouse));
        try {
            mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, bufferedJoy));
        }
        catch (...) {
            mJoy = 0;
        }

        // Set initial mouse clipping size
        windowResized(mWindow);
        // Register as a Window listener
        WindowEventUtilities::addWindowEventListener(mWindow, this);
    }

#ifdef INCLUDE_RTSHADER_SYSTEM
    virtual void processShaderGeneratorInput()
    {
        // Switch to default scheme.
        if (mKeyboard->isKeyDown(OIS::KC_F2))
        {
            mCamera->getViewport()->setMaterialScheme(MaterialManager::DEFAULT_SCHEME_NAME);
            mDebugText  = "Active Viewport Scheme: ";
            mDebugText += MaterialManager::DEFAULT_SCHEME_NAME;
        }

        // Switch to shader generator scheme.
        if (mKeyboard->isKeyDown(OIS::KC_F3))
        {
            mCamera->getViewport()->setMaterialScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
            mDebugText  = "Active Viewport Scheme: ";
            mDebugText += RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME;
        }

        // Toggles per pixel per light model.
        if (mKeyboard->isKeyDown(OIS::KC_F4) && mTimeUntilNextToggle <= 0)
        {
            mTimeUntilNextToggle = 1.0;

            static bool                userPerPixelLightModel = true;
            RTShader::ShaderGenerator* shaderGenerator        = RTShader::ShaderGenerator::getSingletonPtr();
            RTShader::RenderState*     renderState            = shaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

            // Remove all global sub render states.
            renderState->reset();

            // Add per pixel lighting sub render state to the global scheme render state.
            // It will override the default FFP lighting sub render state.
            if (userPerPixelLightModel)
            {
                RTShader::SubRenderState* perPixelLightModel = shaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
                renderState->addTemplateSubRenderState(perPixelLightModel);

                mDebugText = "Per pixel lighting model applied to shader generator default scheme";
            }
            else
            {
                mDebugText = "Per vertex lighting model applied to shader generator default scheme";
            }

            // Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
            shaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

            userPerPixelLightModel = !userPerPixelLightModel;
        }
    }

#endif

    // Adjust mouse clipping area
    virtual void windowResized(RenderWindow* rw)
    {
        unsigned int           width, height, depth;
        int                    left, top;
        rw->getMetrics(width, height, depth, left, top);

        const OIS::MouseState& ms = mMouse->getMouseState();
        ms.width  = width;
        ms.height = height;
    }

    // Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(RenderWindow* rw)
    {
        // Only close for window that created OIS (the main window in these demos)
        if ( rw == mWindow )
        {
            if ( mInputManager )
            {
                mInputManager->destroyInputObject(mMouse);
                mInputManager->destroyInputObject(mKeyboard);
                mInputManager->destroyInputObject(mJoy);

                OIS::InputManager::destroyInputSystem(mInputManager);
                mInputManager = 0;
            }
        }
    }

    virtual ~MyFrameListener()
    {
        // Remove ourself as a Window listener
        WindowEventUtilities::removeWindowEventListener(mWindow, this);
        windowClosed(mWindow);
    }

    virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
    {
        return true;
    }

    virtual bool processUnbufferedMouseInput(const FrameEvent& evt)
    {
        return true;
    }

    // Override frameRenderingQueued event to process that (don't care about frameEnded)
    virtual bool frameRenderingQueued(const FrameEvent& evt)
    {
        return true;
    }

    virtual bool frameEnded(const FrameEvent& evt)
    {
        return true;
    }
    virtual void showDebugOverlay(bool show)
    {
    }

protected:
    Camera*                      mCamera;

    Vector3                      mTranslateVector;
    Real                         mCurrentSpeed;
    RenderWindow*                mWindow;
    bool                         mStatsOn;

    std::string                  mDebugText;

    unsigned int                 mNumScreenShots;
    float                        mMoveScale;
    float                        mSpeedLimit;
    Degree                       mRotScale;
    // just to stop toggles flipping too fast
    Real                         mTimeUntilNextToggle;
    Radian                       mRotX, mRotY;
    TextureFilterOptions         mFiltering;
    int                          mAniso;

    int                          mSceneDetailIndex;
    Real                         mMoveSpeed;
    Degree                       mRotateSpeed;

    // OIS Input devices
    OIS::InputManager*           mInputManager;
    OIS::Mouse*                  mMouse;
    OIS::Keyboard*               mKeyboard;
    OIS::JoyStick*               mJoy;
    BloodRainBites::InputContext mInputContext;
};

#endif
