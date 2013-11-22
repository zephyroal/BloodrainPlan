#include	"stdafx.h"
#include	"OutDepth.h"
#include	"DeferredOrder.h"

namespace Ogre
{
	namespace	RTShader
	{

		String	OutDepth::Type = "SubRenderState_OutDepth";

		#define Lib_OutLinerDepth		"OutLinerDepth"
		#define Func_OutLinerDepth		"OutLinerDepth"

		OutDepth::OutDepth()
		{
		}

		OutDepth::~OutDepth()
		{
		}

		const String&	OutDepth::getType() const
		{
			return Type;
		}

		int	OutDepth::getExecutionOrder() const
		{
			return DO_OutDepth;
		}

		void	OutDepth::updateGpuProgramsParams( Renderable* rend, Pass* pass, const AutoParamDataSource* source, const LightList* pLightList )
		{

		}

		bool	OutDepth::resolveParameters( ProgramSet* programSet )
		{
			Program* vsProgram = programSet->getCpuVertexProgram();
			Program* psProgram = programSet->getCpuFragmentProgram();
			Function* vsMain = vsProgram->getEntryPointFunction();
			Function* psMain = psProgram->getEntryPointFunction();

			//	-vs
			m_inVsPosition = vsMain->getParameterBySemantic( vsMain->getInputParameters(), Parameter::SPS_POSITION, 0 );
			if ( m_inVsPosition.get() == NULL )
			{
				return false;
			}

			m_outVsViewPos = vsMain->resolveOutputParameter( Parameter::SPS_TEXTURE_COORDINATES, -1, Parameter::SPC_POSTOLIGHT_VIEW_SPACE0, GCT_FLOAT3 );
			if ( m_outVsViewPos.get() == NULL )
			{
				return false;
			}

			
			if ( vsProgram->getParameterByAutoType( GpuProgramParameters::ACT_WORLDVIEW_MATRIX ).getPointer() == NULL )
			{
				m_matWorldView = vsProgram->resolveAutoParameterInt( GpuProgramParameters::ACT_WORLDVIEW_MATRIX, 0 );
				if ( m_matWorldView.get() == NULL )
				{
					return false;
				}
			}

			//	-ps
			m_inPsViewPos = psMain->resolveInputParameter( m_outVsViewPos->getSemantic(), m_outVsViewPos->getIndex(), m_outVsViewPos->getContent(), GCT_FLOAT3 );
			if ( m_inPsViewPos.get() == NULL )
			{
				return false;
			}

			if ( psProgram->getParameterByAutoType( GpuProgramParameters::ACT_FAR_CLIP_DISTANCE ).getPointer() == NULL )
			{
				m_inPsFarClipDistance = psProgram->resolveAutoParameterInt( GpuProgramParameters::ACT_FAR_CLIP_DISTANCE, 0 );
				if ( m_matWorldView.get() == NULL )
				{
					return false;
				}
			}

			m_outColor1 = psMain->resolveOutputParameter( Parameter::SPS_COLOR, 1, Parameter::SPC_UNKNOWN, GCT_FLOAT4 );
			if ( m_outColor1.get() == NULL )
			{
				return false;
			}

			return true;
		}

		bool	OutDepth::resolveDependencies( ProgramSet* programSet )
		{
			Program* vsProgram = programSet->getCpuVertexProgram();
			Program* psProgram = programSet->getCpuFragmentProgram();

			vsProgram->addDependency( FFP_LIB_COMMON );
			vsProgram->addDependency(FFP_LIB_TRANSFORM);

			psProgram->addDependency( FFP_LIB_COMMON );
			psProgram->addDependency( Lib_OutLinerDepth );

			return true;
		}

		bool	OutDepth::addFunctionInvocations( ProgramSet* programSet )
		{
			Function* vsMain = programSet->getCpuVertexProgram()->getEntryPointFunction();
			Function* psMain = programSet->getCpuFragmentProgram()->getEntryPointFunction();

			FunctionInvocation* curFuncInvocation = NULL;

			//	-vs
			curFuncInvocation = OGRE_NEW FunctionInvocation(FFP_FUNC_TRANSFORM,  FFP_VS_TRANSFORM, 0); 

			curFuncInvocation->pushOperand( m_matWorldView, Operand::OPS_IN);
			curFuncInvocation->pushOperand( m_inVsPosition, Operand::OPS_IN);
			curFuncInvocation->pushOperand( m_outVsViewPos, Operand::OPS_OUT );

			vsMain->addAtomInstance( curFuncInvocation );

			//	-ps
			curFuncInvocation = OGRE_NEW FunctionInvocation( FFP_FUNC_CONSTRUCT, DO_OutDepth, 0 );
			curFuncInvocation->pushOperand( ParameterFactory::createConstParamFloat( 0.0f ), Operand::OPS_IN );
			curFuncInvocation->pushOperand( ParameterFactory::createConstParamFloat( 0.0f ), Operand::OPS_IN );
			curFuncInvocation->pushOperand( ParameterFactory::createConstParamFloat( 0.0f ), Operand::OPS_IN );
			curFuncInvocation->pushOperand( ParameterFactory::createConstParamFloat( 0.0f ), Operand::OPS_IN );
			curFuncInvocation->pushOperand( m_outColor1, Operand::OPS_OUT );
			psMain->addAtomInstance( curFuncInvocation );

			curFuncInvocation = OGRE_NEW FunctionInvocation( Func_OutLinerDepth, DO_OutDepth, 1 );
			curFuncInvocation->pushOperand( m_inPsViewPos, Operand::OPS_IN );
			curFuncInvocation->pushOperand( m_inPsFarClipDistance, Operand::OPS_IN );
			curFuncInvocation->pushOperand( m_outColor1, Operand::OPS_OUT, Operand::OPM_X );
			psMain->addAtomInstance( curFuncInvocation );

			return true;
		}


		//	-Factory
		const String&	OutDepthFactory::getType() const
		{
			return OutDepth::Type;
		}

		SubRenderState*	OutDepthFactory::createInstanceImpl()
		{
			return new OutDepth; 
		}


	}
}