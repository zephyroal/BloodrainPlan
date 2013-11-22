#pragma once
#include	"OgreShaderFFPRenderState.h"

enum	DeferredOrder
{
	DO_OutDiff_Shininess = Ogre::RTShader::FFP_COLOUR,
	DO_OutDepth = Ogre::RTShader::FFP_COLOUR + 1,
	DO_OutNormal_ViewDistance = Ogre::RTShader::FFP_COLOUR + 2,
};