///============================================================================
/// \file	IStreamObject.h
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

#ifndef ISTREAMOBJECT_H
#define ISTREAMOBJECT_H

class IStreamObject
{
public:
	virtual ~IStreamObject() {};
	virtual int GetStride() = 0;
	virtual int GetNumElements() = 0;
	virtual void Alloc( const int in_elements, const int in_stride )= 0;
	virtual void Free() = 0;
	virtual void * GetData() = 0;
};

#endif