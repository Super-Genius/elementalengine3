///============================================================================
/// \file	CPerfTimer.h
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

#ifndef CPerfTimeManager_H
#define CPerfTimeManager_H

#include "CManager.h"
/*
struct DELTATIMERSCALE
{
	float m_fTimeScale;
	bool m_bPaused;
	DELTATIMERSCALE()
	{
		m_fTimeScale = 1.0f;
		m_bPaused = false;
	}
};

typedef map<DWORD, DELTATIMERSCALE> DELTATIMEMAP;
*/
class CPerfTimeManager : public CManager
{

public:

	/// \brief The Default Destructor for CPerfTimeManager
	/// \return void
	~CPerfTimeManager( );

	/// \brief	Function to get this component.  This is a singleton
	/// \param	void
	/// \return	IComponent*: This Manager
	static IComponent* Create( int nArgs, va_list argptr );

	/// \brief	Initializes the Particle Manager
	/// \param	void
	/// \return	bool: Success
	virtual bool Init( void );

	/// \brief	Update function for the Particle Manager
	/// \return	void
	virtual void Update( DWORD tickCount );
	
	/// \brief	DeInitialization
	/// \return	bool: Success
	virtual bool DeInit( void );
	
	DWORD OnGetRealTime(DWORD size, void *params);
	DWORD OnGetTime(DWORD size, void *params);	
	DWORD OnGetTimeDelta(DWORD size, void *param);
	DWORD OnUpdateTime(DWORD size, void *param);
	DWORD OnResetTime(DWORD size, void *param);
	DWORD OnPauseTime(DWORD size, void *param);
	DWORD OnUnpauseTime(DWORD size, void *param);
	DWORD OnGetCurrentFPS(DWORD size, void *params);
	DWORD OnGetCurrentFrame(DWORD size, void *params);

	/// Sets the scale of the tick coming back, so we can slow down time for debugging or effects
	/// \param size = sizeof(float)
	/// \param param = float with scale data
	/// \return MSG_HANDLED_STOP
	DWORD OnSetTimeScale(DWORD size, void *param);

	/// Sets the maximum delta t, so if we're debugging, the next tick won't be crazy big
	/// Also good for putting a cap on the frame rate so anims don't skip frames like crazy
	/// \param size = sizeof(float)
	/// \param param = float with tick data
	/// \return MSG_HANDLED_STOP
	DWORD OnSetMaxTick(DWORD size, void *param);

	virtual IHashString* GetComponentType();
	
private:

	SINGLETONCONSTRUCTOROVERRIDE( CPerfTimeManager );
	/// \brief The Default Constructor for CPerfTimeManager
	/// \return void
	CPerfTimeManager();

	DELTATIMEMAP m_mDeltaTimers;

	ITimer* m_Timer;

};

#endif