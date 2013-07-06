///============================================================================
/// \file		CJoystickAxis.h
/// \brief		Header file for JoystickAxis class
/// \date		03-31-2008
/// \author		Richard Nguyen
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

#ifndef CJOYSTICKAXIS_H__
#define CJOYSTICKAXIS_H__

class CJoystickAxis
{
public:
	CJoystickAxis();
	void ResetFaults( float fTime );

	/// reset calibration data
	void ResetAxisCalibration( int iMinValue, int iMaxValue );

	/// smooth amount is the time in seconds that history will affect the input
	inline void SetSmoothAmount( float fAmount )
	{
		m_fSmoothAmount = fAmount;
	}

	inline bool HasChanged() const
	{
		return m_bChanged;
	}
	inline float GetValue() const
	{
		return m_fValue;
	}
	inline float GetTimeAxisLastChanged() const
	{
		return m_fAxisLastChanged;
	}

	/// Takes in raw (uncalibrated, unfiltered) data and stores normalized filtered data in the range [0,1]
	void ProcessRawInput( int iValue, float fTime, bool bUseSmoothing );

	struct AXISHISTORYENTRY
	{
		float fValue;
		float fTime;
	};
	bool m_bChanged;
	float m_fAxisLastChanged;
	float m_fRawValue;
	float m_fValue;
	int m_iMin, m_iMax;
	std::vector<AXISHISTORYENTRY> m_AxisHistory;
	float m_fSmoothAmount; // amount of time allowed for history to influence the input on this axis
	bool m_bFaultFlag;
	bool m_bNoChangeFault;
	bool m_bCheckFault;
};

#endif //#ifndef CJOYSTICKAXIS_H__