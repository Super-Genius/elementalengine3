///============================================================================
/// \note   Elemental Engine
///         Copyright (C) 2013 Social Systems Technology, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Social Systems Technology, Inc.,
///			109 East 17th Street Suite 4210 Cheyenne, WY 82001 USA
///============================================================================

#include "stdafx.h"
#if(0)
#include "CGLShader.h"
#include "Vec3.h"
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CGLShader::CGLShader()
{
	m_NumStages = 0;
	m_FrameCallback = NULL;
	m_ObjectCallback = NULL;
	

	m_StageTextures = NULL;
	m_StageTextureNames = NULL;

	if (m_Renderer != NULL)
	{
		IConfig *maxStageConfig;
		CHashString maxStageName(_T("MAX_TEXTURE_STAGES"));

		maxStageConfig = m_Renderer->GetConfiguration(&maxStageName);
		// make sure it came back to us
		if (maxStageConfig == NULL)
		{
			assert(0);
			m_MaxTextureStages = 1;
		}
		else
		{
			m_MaxTextureStages = (UINT)(*maxStageConfig);
		}

		m_StageTextures = new (IBaseTextureObject *[m_MaxTextureStages]);
		memset( m_StageTextures, 0, sizeof(IBaseTextureObject *) * m_MaxTextureStages);
		m_StageTextureNames = new CHashString[m_MaxTextureStages];
		m_SamplerStates = new D3D9SamplerStates[m_MaxTextureStages];
		for(UINT i = 0; i < m_MaxTextureStages;i++)
		{
			m_StageTextureNames[ i ].Init( _T("") );
		}
	}
	
}

CGLShader::~CGLShader()
{
	SAFE_DELETE_ARRAY( m_StageTextures );
	SAFE_DELETE_ARRAY( m_StageTextureNames );
	SAFE_DELETE_ARRAY( m_SamplerStates );

}

IHashString * CGLShader::GetTextureStageName( UINT stage )
{
	if( stage >= 0 && stage < m_MaxTextureStages )
	{
		return &m_StageTextureNames[ stage ];
	}
	else
	{
		return NULL;
	}
}

void CGLShader::SetTextureStage( UINT stage, IHashString * stagename )
{
	if( stage >= 0 && stage < m_MaxTextureStages )
	{
		m_StageTextureNames[ stage ].Init( stagename->GetString() );
		int numstages = 0;
		for (UINT i = 0; i < m_MaxTextureStages; i++ )
		{
			if( _tcslen( m_StageTextureNames[ i ].GetString() ) <= 0 )
			{
				break;
			}
			numstages++;
		}
		m_NumStages = numstages;
	}
}

void CGLShader::SetTextureStageFlags( UINT stage, DWORD filter, DWORD umode, DWORD vmode )
{
	static CHashString wrap(_T("wrap"));
	static CHashString clamp(_T("clamp"));
	static CHashString point(_T("point"));
	static CHashString bilinear(_T("bilinear"));
	static CHashString mirror(_T("mirror"));

	if( stage >= 0 && stage < m_MaxTextureStages )
	{
		//filter
		if( filter == point.GetUniqueID() ) 
			m_SamplerStates[ stage ].m_Filter = D3DTEXF_POINT;
		else if( filter == bilinear.GetUniqueID() )
			m_SamplerStates[ stage ].m_Filter =D3DTEXF_LINEAR;

		//u mode
		if( umode == wrap.GetUniqueID() ) 
			m_SamplerStates[ stage ].m_UMode = D3DTADDRESS_WRAP;
		else if( umode == clamp.GetUniqueID() )
			m_SamplerStates[ stage ].m_UMode =D3DTADDRESS_CLAMP;
		else if( umode == mirror.GetUniqueID() )
			m_SamplerStates[ stage ].m_UMode = D3DTADDRESS_MIRROR;

		//v mode
		if( vmode == wrap.GetUniqueID() ) 
			m_SamplerStates[ stage ].m_VMode = D3DTADDRESS_WRAP;
		else if( vmode == clamp.GetUniqueID() )
			m_SamplerStates[ stage ].m_VMode =D3DTADDRESS_CLAMP;
		else if( vmode == mirror.GetUniqueID() )
			m_SamplerStates[ stage ].m_VMode = D3DTADDRESS_MIRROR;
		
	}
}

bool CGLShader::IsStageCustom( UINT stage )
{
	if( stage >= 0 && stage < m_MaxTextureStages )
	{
		if( m_StageTextures[ stage ] != NULL )
		{
			return true;
		}
	}
	return false;
}


//GL specific
void CGLShader::ApplyShader( LPDIRECT3DDEVICE9 pDevice )
{
	SHADERVARFUNCMESSAGE msg;
	IToolBox * toolbox = EngineGetToolBox();
	int i = 0;
	for( ; i < (int)m_NumStages; i++ )
	{
		pDevice->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pDevice->SetSamplerState( i, D3DSAMP_MINFILTER, m_SamplerStates[ i ].m_Filter );
		pDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, m_SamplerStates[ i ].m_Filter );
		pDevice->SetSamplerState( i, D3DSAMP_ADDRESSU, m_SamplerStates[ i ].m_UMode );
		pDevice->SetSamplerState( i, D3DSAMP_ADDRESSV, m_SamplerStates[ i ].m_VMode );
	}
	pDevice->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pDevice->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	//Set some constants
	if( m_VertexShaderVarMap.size() > 0 )
	{
		SHADERVAR_MAP::iterator iter = m_VertexShaderVarMap.begin();
		for(;iter != m_VertexShaderVarMap.end(); iter++ )
		{
			ShaderVariable &r = (*iter).second;
			msg.uid = r.uid; 
			static DWORD msgHash_GetShaderVarFunc = CHashString(_T("GetShaderVarFunc")).GetUniqueID();
			toolbox->SendMessage(msgHash_GetShaderVarFunc, sizeof(SHADERVARFUNCMESSAGE), &msg );
			if( msg.m_ShaderVarFunc )
			{
				msg.m_ShaderVarFunc->GetValue( r.curvalue[0], r.curvalue[1], r.curvalue[2], r.curvalue[3] );
			}
			SetVertexShaderConstant( r.uregister, r.curvalue[0], r.curvalue[1], r.curvalue[2], r.curvalue[3] );		
		}
	}
	if( m_PixelShaderVarMap.size() > 0 )
	{
		SHADERVAR_MAP::iterator iter = m_PixelShaderVarMap.begin();
		for(;iter != m_PixelShaderVarMap.end(); iter++ )
		{
			ShaderVariable &r = (*iter).second;
			msg.uid = r.uid; 
			static DWORD msgHash_GetShaderVarFunc = CHashString(_T("GetShaderVarFunc")).GetUniqueID();
			toolbox->SendMessage(msgHash_GetShaderVarFunc, sizeof(SHADERVARFUNCMESSAGE), &msg );
			if( msg.m_ShaderVarFunc )
			{
				msg.m_ShaderVarFunc->GetValue( r.curvalue[0], r.curvalue[1], r.curvalue[2], r.curvalue[3] );
			}
			SetVertexShaderConstant( r.uregister, r.curvalue[0], r.curvalue[1], r.curvalue[2], r.curvalue[3]);		
		}
	}
}

bool CGLShader::Reload()
{
	static CHashString hlsllanguage( _T("hlsl") );
	bool retval = true;
	if( !LoadVShader( m_VShaderFile, &hlsllanguage) )
	{
		retval = false;
	}
	if( !LoadPShader( m_PShaderFile, &hlsllanguage) )
	{
		retval = false;
	}
	return retval;
}

void CGLShader::ReleasePixelShader()
{
	SAFE_RELEASE(  m_PixelShader );
}

void CGLShader::ReleaseVertexShader()
{
	SAFE_RELEASE( m_VertexShaderConstTable );
	SAFE_RELEASE( m_VertexShader );
}


#endif