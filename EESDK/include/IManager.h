///=====================================================================
/// \file	IManager.h
/// \brief	Definition of IManager Interface
///         This class defines a IManager which is the interface
///			that all Manager's inherit
/// \date	02/16/2005
/// \author	Patrick Ghiocel
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
///=====================================================================

#ifndef	_IMANAGER__H__
#define _IMANAGER__H__

#include "IComponent.hpp"

namespace ElementalEngine
{
class IObject;
class IToolBox;

class IManager : public IComponent
{
public:
	/// make sure derived destructors are called and we can use RTTI
	virtual ~IManager() {};

	/// Function to add an object to the manager
	/// \param Object is the IObject derived class pointer to add
	/// \return true if success, otherwise false
	virtual bool AddObject( IObject *Object, IHashString *group) = 0;

	/// Deletes an AI Object and removes it to the AI System
	/// \param Object is the IObject derived class pointer to delete
	/// \return true if success, otherwise false
	virtual bool DeleteObject( IObject* Object ) = 0;
};

}	// namespace ElementalEngine

#endif