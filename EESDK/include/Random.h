///==========================================================================
/// \file	Random.h
/// \brief	
/// \date	09/01/2006
/// \author	Kyle Swaim
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

#ifndef RANDOM_H__
#define RANDOM_H__

#include "IRandomImplementation.h"

namespace ElementalEngine
{

int SetRandomImplementation();

class Random
{
private:
	friend int SetRandomImplementation();
	static IRandomImplementation *impl;

public:
	static void Init(IRandomImplementation* randomImplementation)
	{
		impl = randomImplementation;
	}

	/// Default Constructor
	Random()		
	{
		if (impl == NULL)
		{
			SetRandomImplementation();
		}
	}

	/// Constructor
	/// \param seed - Seed value to generate random numbers
	Random(unsigned long seed)		
	{
		if (impl == NULL)
		{
			SetRandomImplementation();
		}
	}

	/// Constructor
	/// \param seedArray - Seed array to generate random numbers
	/// \param size - The size of seedArray
	Random(const unsigned long* seedArray, int size)		
	{
		if (impl == NULL)
		{
			SetRandomImplementation();
		}
	}

	/// Destructor
	~Random() {}
	//...

	/// Set the seed for the Random number generator to use
	/// \param seed - The seed tp generate random numbers
	void SetSeed(unsigned long seed)
	{
		impl->SetSeed( seed );
	}
	/// Set the seed for the Random number generator to use
	/// \param seedArray - An array of seed values
	/// \param size - The size of seedArray
	void SetSeed(const unsigned long* seedArray, int size)
	{
		impl->SetSeed( seedArray, size );
	}
	//...

	/// Generate 32 bit random integer
	/// \return unsigned long - Generated number
	unsigned long GetRandomNumber()
	{
		return impl->GetRandomNumber();
	}
	/// Generates a random number between 0 and 1
	/// \return double - Generated number between 0 and 1
	double GetRandom()
	{
		return impl->GetRandom();
	}

private:
	// copy constructor not defined
	Random(const Random&);
	// assignment operator not defined
	void operator=(const Random&);
};

} //namespace ElementalEngine

#ifndef EE_SDK_EXPORTS
#pragma comment(lib, "EESDK.lib")
#endif

#endif // RANDOM_H__