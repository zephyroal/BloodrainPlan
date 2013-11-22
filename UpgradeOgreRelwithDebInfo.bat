@echo on

rem Core
copy /y "%OGRE_SRC%\bin\relwithdebinfo\OgreMain.dll" ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\OgreVolume.dll" ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\OgreTerrain.dll" ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\OgreRTShaderSystem.dll" ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\OgrePaging.dll" ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\OgreOverlay.dll" ".\bin\relwithdebinfo\"

rem plugins
copy /y "%OGRE_SRC%\bin\relwithdebinfo\RenderSystem_Direct3D9.dll"  ".\bin\relwithdebinfo\"
rem copy /y "%OGRE_SRC%\bin\relwithdebinfo\RenderSystem_Direct3D10.dll"  ".\bin\relwithdebinfo\"
rem copy /y "%OGRE_SRC%\bin\relwithdebinfo\RenderSystem_Direct3D11.dll"  ".\bin\relwithdebinfo\"
rem copy /y "%OGRE_SRC%\bin\relwithdebinfo\RenderSystem_GL.dll"  ".\bin\relwithdebinfo\"
rem copy /y "%OGRE_SRC%\bin\relwithdebinfo\RenderSystem_GLES.dll"  ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\Plugin_ParticleFX.dll"  ".\bin\relwithdebinfo\"
rem copy /y "%OGRE_SRC%\bin\relwithdebinfo\Plugin_BSPSceneManager.dll"  ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\Plugin_CgProgramManager.dll"  ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\Plugin_PCZSceneManager.dll"  ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\Plugin_OctreeZone.dll"  ".\bin\relwithdebinfo\"
copy /y "%OGRE_SRC%\bin\relwithdebinfo\Plugin_OctreeSceneManager.dll"  ".\bin\relwithdebinfo\" 

echo Éý¼¶Íê±Ï...