///============================================================================
/// \file	IRayImplementation.h
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

#ifndef RAYIMPLEMENTATION_H
#define RAYIMPLEMENTATION_H

namespace ElementalEngine
{

class Ray;

class IRayImplementation
{
public:

    /// public virtual destructor
    virtual ~IRayImplementation() {}
    
	/// Return true if this ray intersects a triangle
	/// \param ray - the ray to compare with
	/// \param vert0, vert1, vert2 - the 3 vertices of the triangle
	/// \param t - the distance between the ray and the point of intersection
	/// \param u - the intersection point along the axis of the v0-v1 edge where v0 = 0.0 and v1 = 1.0
	/// \param v - the intersection point along the axis of the v0-v2 edge where v0 = 0.0 and v2 = 1.0
	virtual bool IntersectTriangle( Ray &ray, Vec3 &vert0, Vec3 &vert1, Vec3 &vert2,
									double *t, double *u, double *v) = 0;

};

} //namespace EE

#endif // RAYIMPLEMENTATION_H