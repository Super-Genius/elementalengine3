///============================================================================
/// \file	CBatchGeometry.h
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

#ifndef CBATCHGEOMETRY_H
#define CBATCHGEOMETRY_H
#include <set>
using namespace std;

class CGeometry;
class MergedBuffer
{
public:
	void AddVertices( void * vb, int size );
};
//a collection of static geometry merged together, composite the textures/texcoords
//
class CBatchGeometry
{
public:

	set< CGeometry* > m_Geometry;
	void AddGeom( IGeometry * geom )
	{
		//check if it's already in there?
		/*if( m_Geometry.find( geom ) == m_Geometry.end() )
		{
			m_Geometry.insert( geom );
			//merge into VB?
			MergeVertexBuffers();
			AddTexture();
		}*/
	}
	void RemoveGeom( IGeometry * geom );
	void	Render();
	void MergeVertexBuffers()
	{
		//grab 
	}
};
#endif