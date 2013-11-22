@echo on

rem Core
copy /y "%OGRE_SRC%\bin\debug\OgreMain_d.dll" ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\OgreVolume_d.dll" ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\OgreTerrain_d.dll" ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\OgreRTShaderSystem_d.dll" ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\OgrePaging_d.dll" ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\OgreOverlay_d.dll" ".\bin\debug\"

rem plugins
copy /y "%OGRE_SRC%\bin\debug\RenderSystem_Direct3D9_d.dll"  ".\bin\debug\"
rem copy /y "%OGRE_SRC%\bin\debug\RenderSystem_Direct3D10_d.dll"  ".\bin\debug\"
rem copy /y "%OGRE_SRC%\bin\debug\RenderSystem_Direct3D11_d.dll"  ".\bin\debug\"
rem copy /y "%OGRE_SRC%\bin\debug\RenderSystem_GL_d.dll"  ".\bin\debug\"
rem copy /y "%OGRE_SRC%\bin\debug\RenderSystem_GLES_d.dll"  ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\Plugin_ParticleFX_d.dll"  ".\bin\debug\"
rem copy /y "%OGRE_SRC%\bin\debug\Plugin_BSPSceneManager_d.dll"  ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\Plugin_CgProgramManager_d.dll"  ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\Plugin_PCZSceneManager_d.dll"  ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\Plugin_OctreeZone_d.dll"  ".\bin\debug\"
copy /y "%OGRE_SRC%\bin\debug\Plugin_OctreeSceneManager_d.dll"  ".\bin\debug\" 

echo Éý¼¶Íê±Ï...