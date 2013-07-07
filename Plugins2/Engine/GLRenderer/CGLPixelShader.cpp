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
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

REGISTER_COMPONENT(CPixelShader);

CPixelShader::~CPixelShader()
{
	DeInit();
}

bool CPixelShader::Apply( IShaderDefines *pDefines )
{
	GLuint pShader = m_PixelShader;
	if (pDefines)
	{
		SHADERVARIANTSMAP::iterator itr = m_ShaderVariants.find( pDefines->GetValuesEncoding().iEncoding );
		if (itr != m_ShaderVariants.end())
		{
			pShader = itr->second;
		}
		else
		{
			if (!m_bVariantMissingOneTimeWarning)
			{
				EngineGetToolBox()->Log( LOGWARNING, _T("Compiled pixel shader variant not found for shader %s.\n"), GetName()->GetString() );
				m_bVariantMissingOneTimeWarning = true;
			}

			// attempt to compile variant at runtime
			LoadVariant( pDefines, true );
			SHADERVARIANTSMAP::iterator itr = m_ShaderVariants.find( pDefines->GetValuesEncoding().iEncoding );
			if (itr != m_ShaderVariants.end())
			{
				pShader = itr->second;
			}
		}
	}
	CGLRenderer * CGLRend = (CGLRenderer *)m_Renderer;
	CGLRend->SetPixelShader( pShader );
	return true;
}

bool CPixelShader::Reload( IShaderDefines *pDefines )
{
	IHashString * name = GetName();
	if( name )
	{
		// store variants before they are destroyed to recompile later
		SHADERVARIANTSLIST variantList;
		for (SHADERVARIANTSMAP::iterator itr=m_ShaderVariants.begin(); itr != m_ShaderVariants.end(); itr++)
		{
			variantList.push_back( itr->first );
		}

		// release all shaders and compile the default
		Init();

		if (pDefines)
		{
			// recompile variants
			for (SHADERVARIANTSLIST::iterator itr=variantList.begin(); itr != variantList.end(); itr++)
			{
				SHADERDEFINESENCODING encoding;
				encoding.iEncoding = *itr;
				pDefines->SetValuesFromEncoding( encoding );
				LoadVariant( pDefines, true );
			}
		}
	}

	return true;
}

bool CPixelShader::SetConstant( DWORD hash, float * data, int num )
{
	DWORD shaderconstant = hash;
	CGLRenderer * CGLRend = (CGLRenderer *)m_Renderer;
	return CGLRend->SetPixelShaderConstantF( shaderconstant, data, num );
}

bool CPixelShader::SetConstant( DWORD hash, float x, float y, float z, float w )
{
	//TODO: hash to constant number
	DWORD shaderconstant = hash;
	CGLRenderer * CGLRend = (CGLRenderer *)m_Renderer;
	float data[4];
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
	return CGLRend->SetPixelShaderConstantF( shaderconstant, data, 1 );
}

CPixelShader::CPixelShader(IHashString *parentName, IHashString *name) : 
	CObjectTemplate<IShader>( _T("CShaderManager"), _T("CPixelShader"), parentName, name )
{
	m_PixelShader = NULL;
	// grab the render interface
	GETRENDERERINTERFACEPARAMS renderInterface;

	static DWORD msgHash_GetRendererInterface = CHashString(_T("GetRendererInterface")).GetUniqueID();
	EngineGetToolBox()->SendMessage(msgHash_GetRendererInterface, 
						sizeof(GETRENDERERINTERFACEPARAMS), 
						&renderInterface, 
						NULL, 
						NULL);
	m_Renderer = renderInterface.m_RendererInterface;

	DeInit();
}



/// IObject  functions
/// \brief	Create the a instance of IObject
/// \return	IComponent pointer
IComponent * CPixelShader::Create(int nArgs, va_list argptr)
{
	IHashString *name;
	IHashString *parentName;
	IObject *self;

	name = va_arg(argptr, IHashString *);
	parentName = va_arg(argptr, IHashString *);
	// create the object however you must create it
	try
	{
		// try and create an instance
		self = (IObject*)new CPixelShader(parentName, name);
	}
	catch(...)
	{
		return NULL;
	}

	return self;
}

/// \brief	The Initialization for the Object
/// \param	IManager* pManager: Parent Manager
void  CPixelShader::Init()
{
	StdString infile = GetName()->GetString();
	GetStringFromFile( infile, m_Code );

	ReleaseShader();

	LoadVariant( NULL, true );
}

/// \brief	The Update Function for the Object
/// \return	True on sucess, False on faulure
bool  CPixelShader::Update()
{
	return true;
}

/// \brief	The DeInit Function for the Object
void CPixelShader::DeInit()
{
	ReleaseShader();
}

/// \brief either reads or writes the entity information to a file
void  CPixelShader::Serialize(IArchive &ar)
{
}

void CPixelShader::ReleaseShader()
{
//	SAFE_RELEASE(  m_PixelShader );
	// release shader variants
	for (SHADERVARIANTSMAP::iterator itr=m_ShaderVariants.begin(); itr != m_ShaderVariants.end(); itr++)
	{
//		SAFE_RELEASE( itr->second );
	}
	m_ShaderVariants.clear();
	m_bVariantMissingOneTimeWarning = false;
}

bool CPixelShader::LoadVariant( IShaderDefines *pDefines, bool bCompile )
{
    return true;
#if 0
	// check for already compiled default version of the shader
	if (pDefines == NULL && m_PixelShader != 0)
		return true;
	// check for already compiled variant of the shader
	if (pDefines != NULL)
	{
		unsigned long iEncoding = pDefines->GetValuesEncoding().iEncoding;
		SHADERVARIANTSMAP::iterator itr = m_ShaderVariants.find( iEncoding );
		if (itr != m_ShaderVariants.end())
			return true;
	}
    
#if 0

	D3DXMACRO *pMacros = NULL;
	if (pDefines)
	{
		const int iMaxShaderDefineCount = 32;
		const int iValueCharSize = 64;
		static char szValues[iMaxShaderDefineCount][iValueCharSize];
		static D3DXMACRO vMacros[iMaxShaderDefineCount + 1];
		if (iMaxShaderDefineCount < pDefines->GetDefineCount())
		{
			m_ToolBox->Log( LOGFATALERROR, _T("Shader Define Count exceeds the internal buffer!\n") );
			return false;
		}

		DWORD i;
		for (i=0; i < pDefines->GetDefineCount(); i++)
		{
			vMacros[i].Name = pDefines->GetDefineName(i);
			vMacros[i].Definition = _itoa(pDefines->GetDefineValue(i), szValues[i], 10);
		}
		// null terminate macro list
		vMacros[i].Name = NULL;
		vMacros[i].Definition = NULL;
		pMacros = vMacros;
	}

#endif
    
	LPDIRECT3DPIXELSHADER9 pShader = NULL;
	LPD3DXBUFFER shaderBuf = NULL;
	LPD3DXBUFFER pErrorMsgs = NULL;
	CGLIncludeManager includeInterface;
	LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)m_Renderer->GetAPIDevice();
	if( pDevice )
	{
		int len = _tcslen( (const TCHAR*)m_Code );
		LPCSTR profile = D3DXGetPixelShaderProfile( pDevice );

		bool bLoadedCompiled = false;
		HRESULT hr = E_FAIL;

		const TCHAR *szFile = GetName()->GetString();
		TCHAR drive[MAX_PATH];
		TCHAR directory[MAX_PATH];
		TCHAR filename[MAX_PATH];
		TCHAR fileext[MAX_PATH];
		TCHAR szDefinesTemp[65] = { '\0' };
		_tsplitpath( szFile, drive, directory, filename, fileext );

		StdString szCompiledFile;
		szCompiledFile += drive;
		szCompiledFile += directory;
		szCompiledFile += _T("Compiled/");
		szCompiledFile += filename;
		if (pDefines)
		{
			szCompiledFile += _T("_enc");
			szCompiledFile += _itot(pDefines->GetValuesEncoding().iEncoding, szDefinesTemp, 10);
		}
		szCompiledFile += fileext;

		LPVOID pShaderFileData = NULL;
		UINT shaderLen = 0;
		struct _stat shaderFilestat;

		// checking if compiled version exists, if we can load it into a buffer and if the file stats of the shader (not compiled) are readable
		if (CheckFileExists(szCompiledFile) &&	( _tstat( szFile, &shaderFilestat ) == 0) && LoadFileIntoBuffer( szCompiledFile, pShaderFileData, shaderLen, true ))
		{
			m_ToolBox->Log( LOGINFORMATION, _T("Reading compiled shader file: %s\n"), szCompiledFile.c_str() );
			// create a shader buffer to store the compiled shader
			hr = D3DXCreateBuffer( shaderLen, &shaderBuf );
			if (SUCCEEDED(hr))
			{
				time_t storedMTime = 0;
				// get the compiled date out of the file
				memcpy( &storedMTime, pShaderFileData, sizeof(time_t) );
			
				// if the stored modified time in the compiled shader file is the same as the current
				// modified time of the shader file
				if( storedMTime == shaderFilestat.st_mtime )
				{
					// reduce the buffer size by the preamble (mod time)
					shaderLen -= (int)sizeof(time_t);
						
					// copy the compiled shader into the shader buffer
					memcpy( shaderBuf->GetBufferPointer(), ((TCHAR *) pShaderFileData)+ sizeof(time_t), shaderLen);
					bLoadedCompiled = true;
				}
			}
			SAFE_DELETE_ARRAY( pShaderFileData );
		}

		if (!bLoadedCompiled && bCompile)
		{
			if (pDefines)
				EngineGetToolBox()->Log( LOGINFORMATION, _T("Compiling shader %s:%d\n"), GetName()->GetString(), pDefines->GetValuesEncoding().iEncoding );
			else
				EngineGetToolBox()->Log( LOGINFORMATION, _T("Compiling shader %s\n"), GetName()->GetString() );

			hr = D3DXCompileShader( m_Code,
										len,//length of string in bytes
										pMacros, //can add that matrix of macros here
										&includeInterface, //for include directories
										"main",//? temp
										profile, //vs_1_1 for example
										0, //compiling options?
										&shaderBuf,
										&pErrorMsgs,
										NULL );
		}
		//now actually create the shader
		if( hr == D3D_OK &&
			shaderBuf )
		{
			if (!bLoadedCompiled)
			{
				struct _stat shaderFilestat;

				// get the shader file's modified time
				if (_tstat( szFile, &shaderFilestat ) == 0)
				{
					m_ToolBox->Log( LOGINFORMATION, _T("Writing compiled shader file: %s\n"), szCompiledFile.c_str() );
					// open a compiled shader file for writing
					FILE *fp = fopen( szCompiledFile, "wb" );
					if (fp)
					{
						// write shader file's modified time
						fwrite( &shaderFilestat.st_mtime, sizeof(time_t), 1, fp );
						// write compiled shader data
						fwrite( shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), 1, fp );
						fclose(fp);
					}
					else
					{
						m_ToolBox->Log( LOGWARNING, _T("Failed to write compiled shader file: %s\n"), szCompiledFile.c_str() );
					}
				}
			}

			hr = pDevice->CreatePixelShader( (DWORD *) shaderBuf->GetBufferPointer(), &pShader );
			assert( SUCCEEDED(hr) );
			if (!SUCCEEDED(hr))
			{
				m_ToolBox->Log( LOGWARNING, _T("Failed to create shader : %s\n"), szCompiledFile.c_str() );
			}

			SAFE_RELEASE( shaderBuf );
			SAFE_RELEASE( pErrorMsgs );

			if (pDefines == NULL) // we are compiling the default shader with no macro defines
			{
				assert( m_PixelShader == NULL ); // the default shader should only be compiled on Init when this is NULL
				m_PixelShader = pShader;
			}
			else if (pDefines != NULL) // we are compiling a variant of the shader
			{
				unsigned long iEncoding = pDefines->GetValuesEncoding().iEncoding;
				m_ShaderVariants[iEncoding] = pShader;
			}
			return true;
		}
	}	
	if( pErrorMsgs )
	{
		IHashString * name = GetName();
		TCHAR* debug_errors = (TCHAR*)pErrorMsgs->GetBufferPointer();
		m_ToolBox->Log( LOGERROR, _T("Could not create Pixel shader %s\nError message: %s\n"),
			name->GetString(), debug_errors );				
		SAFE_RELEASE( pErrorMsgs );				
	}
	SAFE_RELEASE( shaderBuf );

	return false;
#endif
}
