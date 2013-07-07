

#ifndef _ENUMMAP_H_
#define _ENUMMAP_H_

#include <map>

template <typename Type>
class EnumMap : public std::map<const DWORD, Type>
{
public:

    
    //! Construct and initialize.
    EnumMap(const DWORD key, const Type& val)
    {
        (*this)[key] = val;
    }
    
    EnumMap& operator()(const DWORD key, const Type& val)
    {
        (*this)[key] = val;
        return *this;
    }
};

#endif	// _ENUMMAP_H_