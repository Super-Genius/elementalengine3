///==========================================================================
/// \file	DefaultAABBImplementation.h
/// \brief	
/// \date	08/10/2005
/// \author	Marvin Gouw
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

#ifndef DEFAULTAABBIMPLEMENTATION_H__
#define DEFAULTAABBIMPLEMENTATION_H__

#include "IAABBImplementation.h"
#include "AABB.h"

namespace ElementalEngine
{

class CDefaultAABBImplementation : public IAABBImplementation
{
private:
	void Init( void );
	void CleanUp( void );
	CDefaultAABBImplementation()
	{
		Init();
	}

	virtual ~CDefaultAABBImplementation()
	{
		CleanUp();
	}

public:
	static IAABBImplementation* Instance( void );    
	static void Destroy( void );
	
	virtual bool IntersectRay( AABB &bb, const Ray &r, float &t0, float &t1);
	virtual bool IntersectTri( AABB &bb, Vec3& a_V0, Vec3& a_V1, Vec3& a_V2 );
	virtual INTERSECTION_RESULT IntersectAABB( AABB &bb, const Vec3 &vMin, const Vec3 &vMax );
	virtual INTERSECTION_RESULT IntersectSphere( AABB &bb, const Vec3 &vCenter, const float &fRadius );
	
private:
	static IAABBImplementation* s_instance;
};

} //namespace ElementalEngine

#endif // DEFAULTAABBIMPLEMENTATION_H__