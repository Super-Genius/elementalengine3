///=====================================================================
/// \file	IListenerFactory.h
/// \brief	Definition of IListenerFactory Interface
///         This class defines the interface for Listener factories
/// \date	02/16/2005
/// \author	Kenneth Hurley
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

#ifndef _ILISTENERFACTORY__H__
#define _ILISTENERFACTORY__H__

#include "IComponent.hpp"

namespace ElementalEngine
{
class IListener;

/// \interface IListenerFactory
/// \brief this interface is for listener factories.
class IListenerFactory :  public IComponent
{
public:
	virtual ~IListenerFactory() {};
	/// Register a IListener with the system
	/// \param sourceListener - Listener that gets callbacks from system
	/// \param priority - priority listener is called
	virtual void Register(IListener *sourceListener, DWORD priority) = 0;
	/// UnRegister a IListener with the system
	/// \param sourceListener - Listener that gets callbacks from system
	/// \return true if found and unregistered
	virtual bool UnRegister(IListener *sourceListener) = 0;
};

}	//namespace ElementalEngine

#endif