///---------------------------------------------------------------------
/// \file		Register.h
/// \brief		Classes/Macros to register dynamically created objects
/// \created	2/01/04
/// \author		Kenneth L. Hurley
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
//---------------------------------------------------------------------
#ifndef REGISTER_H
#define REGISTER_H

#include "IComponent.hpp"

namespace ElementalEngine
{

///
/// structure to register components automagically
typedef struct COMPONENTREGISTERINFO
{
	const TCHAR *m_Name;				/// the name of the class
	COMPONENTCREATEFUNC *m_CreateFunc;	/// the creation function
} COMPONENTREGISTERINFO;

///
/// Class to register an component, i.e. a COM like class object
///
class CRegisterComponent
{
private:
	CHashString	m_ClassName;
public:
	CRegisterComponent(COMPONENTREGISTERINFO *componentInfo, bool isSingleton = false)
	{
		IToolBox *toolBox;
		toolBox = EngineGetToolBox();
		m_ClassName = componentInfo->m_Name;
		if (isSingleton)
		{
			toolBox->RegisterComponent(&m_ClassName,
				componentInfo->m_CreateFunc, componentInfo->m_CreateFunc);
		}
		else
		{
			toolBox->RegisterComponent(&m_ClassName,
				componentInfo->m_CreateFunc, NULL);
		}
	}

	~CRegisterComponent()
	{
		IToolBox *toolBox;
		toolBox = EngineGetToolBox();
		toolBox->UnRegisterComponent(&m_ClassName);
	}
};

///
/// how to automatically register the object inside the .exe, .dll
///
#define REGISTER_COMPONENT(className)								\
	static const TCHAR *className##_Name = reinterpret_cast<const TCHAR*>(&_T(#className));		\
	static COMPONENTREGISTERINFO									\
					className##ComponentRegisterInfo =				\
	{																\
		className##_Name,											\
		className::Create											\
	};																\
	extern "C" CRegisterComponent className##RO(&className##ComponentRegisterInfo);

///
/// how to automatically register the object inside the .exe, .dll
///
#define REGISTER_COMPONENT_SINGLETON(className)						\
	static const TCHAR * className##_Name = reinterpret_cast<const TCHAR*>(&_T(#className));      \
	static COMPONENTREGISTERINFO									\
					className##ComponentRegisterInfo =				\
	{																\
		className##_Name,											\
		className::Create											\
	};																\
	extern "C" CRegisterComponent className##RO(&className##ComponentRegisterInfo, true);


///
/// how to automatically register the object inside the .exe, .dll
/// and masquerade as another object
///
#define REGISTER_COMPONENT_AS(className, baseClassName)				\
	static const TCHAR *className##_Name = reinterpret_cast<const TCHAR*>(&_T(#baseClassName));	\
	static COMPONENTREGISTERINFO									\
					className##ComponentRegisterInfo =				\
	{																\
		className##_Name,											\
		className::Create											\
	};																\
	extern "C" CRegisterComponent className##RO(&className##ComponentRegisterInfo);

///
/// how to automatically register the object inside the .exe, .dll
/// as a singleton and masquerade as another singleton
///
#define REGISTER_COMPONENT_SINGLETON_AS(className, baseClassName)	\
	static const TCHAR * className##_Name = reinterpret_cast<const TCHAR*>(&_T(#baseClassName));	\
	static COMPONENTREGISTERINFO									\
					className##ComponentRegisterInfo =				\
	{																\
		className##_Name,											\
		className::Create											\
	};																\
	extern "C" CRegisterComponent className##RO(&className##ComponentRegisterInfo, true);

///
/// Message handler structure for registration
///
typedef struct MESSAGEHANDLERINFO
{
	const TCHAR *m_MsgName;			/// the name of the message
	MSGFUNCTION m_MsgFuncPtr;		/// the function handler pointer
	DWORD m_Priority;				/// the priority of the message
} MESSAGEHANDLERINFO;

//
// classes to auto-register messages
//
class CRegisterMessage
{
	CHashString m_MsgHashName;
	CHashString m_ClassHashName;
	DWORD m_MsgID;
public:
	CRegisterMessage(const TCHAR *className, MESSAGEHANDLERINFO *msgInfo)
	{
		IToolBox *toolBox;

		toolBox = EngineGetToolBox();
		m_MsgHashName = msgInfo->m_MsgName;
		m_ClassHashName = className;
		m_MsgID = toolBox->GetMessageID(&m_MsgHashName);
		toolBox->RegisterMessageHandler(m_MsgID,
			msgInfo->m_MsgFuncPtr, &m_ClassHashName, msgInfo->m_Priority);
	}

	~CRegisterMessage()
	{
		IToolBox *toolBox;

		toolBox = EngineGetToolBox();
		toolBox->UnRegisterMessageHandler(m_MsgID, &m_ClassHashName);
	}
};

#define REGISTER_MESSAGE_HANDLER(msg, handler, className)				\
static MESSAGEHANDLERINFO msg##MHI =									\
{																		\
	reinterpret_cast<const TCHAR*>(&_T(#msg)),							\
	static_cast< MSGFUNCTION >(&className::handler),					\
	0x80000000															\
};																		\
extern "C" CRegisterMessage msg##_##className##RM(_T(#className), &msg##MHI);

#define REGISTER_MESSAGE_HANDLER_PRIORITY(msg, handler, className, priority)		\
static MESSAGEHANDLERINFO msg##MHI =												\
{																					\
	reinterpret_cast<const TCHAR*>(&_T(#msg)),										\
	static_cast< MSGFUNCTION >(&className::handler),								\
	priority																		\
};																					\
extern "C" CRegisterMessage msg##_##className##RM(_T(#className), &msg##MHI);

#define REGISTER_MESSAGE_HANDLER_AS(msg, handler, className, baseClassName)	\
static MESSAGEHANDLERINFO msg##MHI =										\
{																			\
	reinterpret_cast<const TCHAR*>(&_T(#msg)),								\
	static_cast< MSGFUNCTION >(&className::handler),						\
	0x80000000																\
};																			\
extern "C" CRegisterMessage msg##_##className##RM(_T(#baseClassName), &msg##MHI);

/// Masquerading as macros
#define REGISTER_MESSAGE_HANDLER_PRIORITY_AS(msg, handler, className, baseClassName, priority)	\
static MESSAGEHANDLERINFO msg##MHI =															\
{																								\
	reinterpret_cast<const TCHAR*>(&_T(#msg)),													\
	static_cast< MSGFUNCTION >(&className::handler),											\
	priority																					\
};																								\
extern "C" CRegisterMessage msg##_##className##RM(_T(#baseClassName), &msg##MHI);

}	//namespace ElementalEngine

#endif		// #ifndef REGISTER_H
