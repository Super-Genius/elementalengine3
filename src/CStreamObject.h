///============================================================================
/// \file	CStreamObject.h
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

#ifndef CSTREAMOBJECT_H
#define CSTREAMOBJECT_H

#include "IStreamObject.h"

class CStreamObject : public IStreamObject
{
private:
	void *	m_StreamData;
	int		m_Stride;
	int		m_NumElements;
public:
	CStreamObject()
	{
		m_StreamData = NULL;
		m_Stride = NULL;
		m_NumElements = NULL;
	}
	~CStreamObject()
	{
		Free();
	}
	virtual int GetStride()
	{
		return m_Stride;
	}
	virtual int GetNumElements()
	{
		return m_NumElements;
	}
	virtual void Alloc( const int in_elements, const int in_stride )
	{
		int buffersize = in_elements*in_stride;
		if( m_NumElements*m_Stride != buffersize )
		{
			Free();
		}
		//Create if our buffer was inadequate or is new
		if( !m_StreamData )
		{
			m_StreamData = malloc( buffersize );
		}
		m_NumElements = in_elements;
		m_Stride = in_stride;
	}
	virtual void Free()
	{
		free( m_StreamData );
		m_StreamData = NULL;
		m_NumElements = 0;
		m_Stride = 0;

	}
	virtual void * GetData()
	{
		return m_StreamData;
	}
};

#endif

