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


#include "StdAfx.h"
#include "EEOSXLogWindow.h"

// registers ourselves with Elemental Engine, so we get messages
// it's not really a singleton to us, but to EE it is.
REGISTER_COMPONENT_SINGLETON(CLogViewerComponent);

// Register EE Messages
REGISTER_MESSAGE_HANDLER_PRIORITY(LogMessage, OnLogMessage, CLogViewerComponent, 0x70000000);
REGISTER_MESSAGE_HANDLER(WindowsMainCreate, OnCreate, CLogViewerComponent);
REGISTER_MESSAGE_HANDLER(WindowsCmdMsg, OnCmdMsg, CLogViewerComponent);

// get the singleton for this
IComponent* CLogViewerComponent::Create(int nArgs, va_list argptr)
{
	return SINGLETONINSTANCE(CLogViewerComponent);
}

CLogViewerComponent::CLogViewerComponent()
{
	// init the pointers
	//m_LogFile = NULL;
	m_ToolBox = EngineGetToolBox();
    
    OSXCreateLogWindow();
}

CLogViewerComponent::~CLogViewerComponent()
{
	//if (m_LogFile != NULL)
	//{
	//	fclose(m_LogFile);
	//}

    OSXDestroyLogWindow();

	//if ( m_Parent != NULL )
	//{
	//	delete m_Parent;
	//	m_Parent = NULL;
	//}
}

// serialize function for this object
void CLogViewerComponent::Serialize(IArchive &ar)
{
}

// get component name for this object
IHashString *CLogViewerComponent::GetComponentType()
{
	static CHashString compName(_T("CLogViewerComponent"));
	return &compName;
}

bool CLogViewerComponent::IsKindOf(IHashString *compType)
{
	return (compType->GetUniqueID() == GetComponentType()->GetUniqueID());
}

DWORD CLogViewerComponent::OnLogMessage(DWORD size, void *param)
{
	LOGMESSAGESTRUCT *logMessage;
	// can't do a verify message size, as that might cause recursion
	if (size != sizeof(LOGMESSAGESTRUCT))
	{
		return MSG_ERROR;
	}

	logMessage = (LOGMESSAGESTRUCT *)param;

	//if ( m_Parent != NULL )
	//{
	//	m_Parent->LogLine(logMessage->severity, logMessage->msg );
	//}
	
    OSXLogWindowMessage(logMessage->msg);

	if (logMessage->severity == LOGFATALERROR)
	{
		static DWORD msgHash_Exit = CHashString(_T("Exit")).GetUniqueID();
		m_ToolBox->SendMessage(msgHash_Exit, 0, NULL);
	}
    
	return MSG_HANDLED_PROCEED;
}

// message to handle WindowsCreate function
DWORD CLogViewerComponent::OnCreate(DWORD size, void *param)
{
    // here we should receive a message.
	//VERIFY_MESSAGE_SIZE(size, sizeof(CMDIFrameWnd *));

	return MSG_HANDLED_PROCEED;
}

//---------------------------------------------------------------------
/// Function:	OnCmdMsg
/// Description:Called from the main windows OnCmdMsg function.
///				Routes commands through doc/view architecture
/// Returns:	TRUE if handled
//---------------------------------------------------------------------
DWORD CLogViewerComponent::OnCmdMsg(DWORD size, void *param)
{
	return MSG_HANDLED_PROCEED;
}