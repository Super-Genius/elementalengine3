
///==========================================================================
/// \file	CParticleData.h
/// \brief	Data Structs for Particle Transport
/// \date	05/22/2008
/// \author	Brendan Geiger
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

#ifndef	_CParticleData_H_
#define _CParticleData_H_

namespace ElementalEngine
{

struct SParticleBase
{
	Vec3		pos;
	EulerAngle	rot;
};

} // namespace ElementalEngine

#endif // _CParticleData_H_