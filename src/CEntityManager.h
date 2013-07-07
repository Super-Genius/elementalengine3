///==========================================================================
/// \file	CEntityManager.h
/// \brief	Manager for CEntity objects
///			This file defines CEntityManager which manages the creation
///			and manipulation of CEntity objects. 
///			Inherits from CManager
/// \date	2-22-2005
/// \author	Kyle Swaim
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

#ifndef	CENTITYMANAGER_H
#define	CENTITYMANAGER_H

#include "CManager.h"

///==========================================================================
/// \class	CEntityManager
/// \brief	Manager class for CEntity objects
///==========================================================================
class CEntityManager : public CManager
{
protected:
	SINGLETONCONSTRUCTOROVERRIDE( CEntityManager );

	/// \brief	Default Destructor
	///			Since CPhysManager is implemented as a singleton the
	///			constructor is private.
	CEntityManager();

public:

	/// \brief	Default Destructor
	~CEntityManager( );

	/// \brief	Function to get this component.
	///			Returns the singleton instance in this case
	static IComponent *Create(int nArgs, va_list argptr);

	/// \brief	Initializes the AI Manager and AI System
	/// \return	True on sucess, False on failure
	virtual bool Init();
	
	/// \brief	Compares two names and determines if one is a part of another
	///	\param	searchString - StdString searching for
	/// \param	compString - StdString to compare agains
	/// \return true if seachString is same as compString else false
	bool CompareNames(StdString searchString, StdString compString);

	/// \brief	Visits the entity objects
	///	Searches through all entities calling the visitor pattern on
	///	any entities that match the searchName
	///	\param	*searchString - IHashString ptr to the search string
	///	\param	*visitPattern - the Visitor Pattern to call on the entity
	///	\return	true if no errors, else false
	bool VisitNode(IHashString *searchString, IVisitor* visitorPattern);

	/// \brief	Message to visit the entity objects
	///	\param	size - size of VISITHIERARCHYDATA
	///	\param	*params - ptr to VISITHIERARCHYDATA struct
	///	\return MSG_HANDLED_PROCEED, MSG_ERROR
	DWORD OnVisitEntities(DWORD size, void *params);

	/// \brief	Start running the manager
	DWORD OnStart(DWORD size, void *params);
	/// \brief	Stop running the manager
	DWORD OnStop(DWORD size, void *params);


private:
	int	dummy;
	/// Boolean controling if the simulation is active or not
	bool m_IsRunning;
};

#endif	// CENTITYMANAGER_H
