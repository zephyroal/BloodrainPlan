#pragma once
#include	"OgreShaderSubRenderState.h"
#include	"ogreshaderprogramset.h"


namespace Ogre
{
	namespace	RTShader
	{

		class	OutDepth : public SubRenderState
		{
		public:
			OutDepth();
			virtual	~OutDepth();

			virtual const String&	getType() const;

			virtual int		getExecutionOrder() const;
			virtual void	updateGpuProgramsParams( Renderable* rend, Pass* pass, const AutoParamDataSource* source, const LightList* pLightList );
			virtual void	copyFrom( const SubRenderState& rhs ){}

		protected:
			virtual bool	resolveParameters( ProgramSet* programSet );	
			virtual bool	resolveDependencies( ProgramSet* programSet );
			virtual bool	addFunctionInvocations( ProgramSet* programSet );

		public:
			static String		Type;
			
		private:
			UniformParameterPtr		m_matWorldView;
			ParameterPtr			m_inVsPosition;
			ParameterPtr			m_outVsViewPos;

			UniformParameterPtr		m_inPsFarClipDistance;
			ParameterPtr			m_inPsViewPos;
			ParameterPtr			m_outColor1;

		};

		class	OutDepthFactory : public SubRenderStateFactory
		{
		public:
			virtual const String&	getType() const;

		protected:
			virtual SubRenderState*	createInstanceImpl();

		};

	}
}