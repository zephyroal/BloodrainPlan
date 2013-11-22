@echo on

for /R %%i IN (*.mesh) DO %OGRE_SRC%/bin/relwithdebinfo/OgreMeshUpgrader.exe %%i

echo Éý¼¶Íê±Ï...