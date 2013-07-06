///==========================================================================
/// \file	IScene.h
/// \brief	Scene Interface
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
///==========================================================================

#ifndef	ISCENE_H
#define	ISCENE_H

namespace ElementalEngine
{

class IScene : public IObject
{
public:
	virtual ~IScene() { }

	virtual void UpdateObject( ISceneObject *pObject ) = 0;
	virtual void RemoveGlobalObject( ISceneObject* object ) = 0;
};

} //namespace ElementalEngine

#endif	// ISCENE_H