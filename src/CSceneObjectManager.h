///============================================================================
/// \file		CSceneObjectManager.h
/// \brief		Header file for manager for scene objects
/// \date		09/19/2007
/// \author		Nick Tourte
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

#ifndef _C_SCENEOBJECTMANAGER_H_
#define _C_SCENEOBJECTMANAGER_H_

namespace ElementalEngine
{

typedef map<DWORD, ISceneObject*> ATTACHMENTOBJECTMAP;
typedef list<ISceneObject*> ATTACHMENTLIST;
typedef map<DWORD, ATTACHMENTLIST> BASEATTACHMENTMAP;

class CSceneObjectManager : public CManager
{
public:
	// default dtor
	virtual ~CSceneObjectManager();

	/// Function to get this component.  This is a singleton
	/// \param	void
	/// \return	IComponent*: This Manager
	static IComponent* Create( int nArgs, va_list argptr );

	/// \fn virtual void Update()
	/// \brief Function call back when system is being updated
	/// \param tickCount = system tick counter
	virtual void Update(DWORD tickCount);
	
	/// Deletes an Object and removes it from the System
	/// \param Object is the IObject derived class pointer to delete
	/// \return true if success, otherwise false
	virtual bool DeleteObject( IObject* Object );

	/// MESSAGES	
	
	/// Registers an attachment in the maps
	/// \param size = sizeof(ATTACHSCENEOBJECTPARAMS)
	/// \param param = ATTACHSCENEOBJECTPARAMS with attaching objects
	/// \return MSG_HANDLED_STOP
	DWORD OnRegisterSceneAttachObjects(DWORD size, void *param);

	/// Detaches an attachment object from it's parent
	/// \param size = sizeof(IHashString*)
	/// \param param = IHashString with name of attachment to detach
	/// \return MSG_HANDLED_STOP
	DWORD OnDetachSceneObject(DWORD size, void *param);

protected:
	SINGLETONCONSTRUCTOROVERRIDE(CSceneObjectManager);
	// default ctor
	CSceneObjectManager();

private:
	void RemoveAttachmentFromBaseMap(ISceneObject *attachment, ATTACHMENTLIST *attachList);

	/// named scene object type
	CHashString m_hsSceneObjectType;
	
	ATTACHMENTOBJECTMAP m_mAttachmentObjectMap;	
	BASEATTACHMENTMAP m_mBaseAttachmentMap;
};

}	//namespace ElementalEngine

#endif