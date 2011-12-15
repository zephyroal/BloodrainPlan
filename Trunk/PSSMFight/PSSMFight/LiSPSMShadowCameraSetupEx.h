#pragma once
#include	"stdafx.h"

namespace Ogre
{
	class	LiSPSMShadowCameraSetupEx : public LiSPSMShadowCameraSetup
	{
	public:
		LiSPSMShadowCameraSetupEx();

		virtual void	getShadowCamera( const SceneManager *sm, const Camera *cam, const Viewport *vp, const Light *light, Camera *texCam, size_t iteration) const;

	private:
		Ogre::GpuProgramParametersSharedPtr		m_DeferredShadowFPParamter;
	};
}