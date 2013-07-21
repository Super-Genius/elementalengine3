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

#ifndef _LOGVIEWERCOMPONENT_HPP_
#define _LOGVIEWERCOMPONENT_HPP_

/// class so we can hook into messaging system of Elemental Engine
class CLogViewerComponent : public IComponent
{
private:
	SINGLETONCONSTRUCTOROVERRIDE(CLogViewerComponent);
	CLogViewerComponent();

public:
	/// \brief	Default Destructor
	virtual ~CLogViewerComponent();

	static IComponent *Create(int nArgs, va_list argptr);
	void Serialize(IArchive &ar);
	IHashString *GetComponentType();
	bool IsKindOf(IHashString *compType);

	// Messages
	DWORD OnLogMessage( DWORD size, void *param );

	DWORD OnCreate(DWORD size, void *param);

	//void SetParent(LogViewerDialog *parent);

	// distributing messages
	DWORD OnCmdMsg(DWORD size, void *param);

private:
	IToolBox* m_ToolBox;

};

#endif // #ifndef _LOGVIEWERCOMPONENT_HPP_
