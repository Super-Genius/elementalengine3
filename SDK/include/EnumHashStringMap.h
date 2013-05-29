

#ifndef _ENUMHASSTRINGMAP_H_
#define _ENUMHASSTRINGMAP_H_

#include <map>

template <typename Type>
class EnumHashStringMap : public std::map<const DWORD, Type>
{
public:
    EnumHashStringMap(const DWORD key, const Type& val)
    {
        (*this)[key] = val;
    }

    EnumHashStringMap& operator()(const DWORD key, const Type& val)
    {
        (*this)[key] = val;
        return *this;
    }
};

#endif	// _ENUMHASSTRINGMAP_H_