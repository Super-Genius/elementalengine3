///============================================================================
/// \file	ITextureManager.h
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

#ifndef ITEXTUREMANAGER_H
#define ITEXTUREMANAGER_H

#include "ITextureObject.h"

namespace ElementalEngine
{

class ITextureManager
{	
public:
	virtual ~ITextureManager() {};
	virtual DWORD OnAddTexture( DWORD size, void *params) = 0;
	virtual DWORD OnRemoveTexture( DWORD size, void *params) = 0;
	virtual DWORD OnGetTexture( DWORD size, void *params) = 0;
	virtual ITextureObject * GetTexture(IHashString *name ) = 0;
};
}	//namespace ElementalEngine

#endif