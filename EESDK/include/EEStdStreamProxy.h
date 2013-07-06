///============================================================================
/// \file		EEStdStreamProxy.h
/// \brief		Header file for proxy between std:streambuf and IEEStreams
/// \date		11-14-2006
/// \author		Kenneth Hurley
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

#ifndef _EESTDSTREAMPROXY_H_
#define _EESTDSTREAMPROXY_H_

#include <iostream>
using namespace std;

namespace ElementalEngine
{

class CEEStdStreamProxy : public std::iostream
{
public:
	CEEStdStreamProxy(IEEStream *eeStream);
	~CEEStdStreamProxy(void);
};

} // namespace ElementalEngine

#endif // #ifndef _EESTDSTREAMPROXY_H_
