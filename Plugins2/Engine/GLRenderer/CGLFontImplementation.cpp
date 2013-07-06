///============================================================================
/// \note   Elemental Engine
///         Copyright (c)  2005-2008 Signature Devices, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Signature Devices, Inc.,
///			3200 Bridge Parkway Suite 102, Redwood City, CA 94086 USA.
///============================================================================

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IFontImplementation * CGLFontImplementation::s_instance = NULL;

IFontObject * CGLFontImplementation::CreateFont()
{
	IFontObject * obj = NULL;
	try
	{
		obj = new EE_CGLFont();
	}
	catch( ... )
	{
		EngineGetToolBox()->Log( LOGERROR, _T("Out of memory, could not create font object\n"));
		return NULL;
	}
	return obj;
}

void CGLFontImplementation::DestroyFont( IFontObject * obj )
{
	if( obj )
	{
		delete obj;
	}
}

int CGLFontImplementation::GetFontPixWidth(IHashString *fontFace, UINT fontsize, const TCHAR* text)
{
	return EE_CGLFont::GetFontPixWidth(CHashString(fontFace), fontsize, text);
}

IFontImplementation * CGLFontImplementation::Instance()
{
	if ( s_instance == NULL )
	{
		s_instance = (IFontImplementation*)new CGLFontImplementation();
	}
	return s_instance;
}

void CGLFontImplementation::Destroy()
{
	if ( s_instance != NULL )
	{
		delete s_instance;
	}
	s_instance = NULL;
}