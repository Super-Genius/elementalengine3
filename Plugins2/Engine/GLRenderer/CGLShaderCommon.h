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

#ifndef CGLSHADERCOMMON_H
#define CGLSHADERCOMMON_H

struct D3D9SamplerStates
{
	DWORD	m_Filter;
	DWORD	m_UMode;
	DWORD	m_VMode;
	D3D9SamplerStates()
	{
		m_Filter = GL_NEAREST;
		m_UMode = GL_REPEAT;
		m_VMode = GL_REPEAT;
	}
};

bool GetStringFromFile( StdString &filename, StdString &outstring );
BOOL LoadFileIntoBuffer( const TCHAR* pFileName, void *& pFileData, UINT& iFileDataSize, bool bBinary );

#endif