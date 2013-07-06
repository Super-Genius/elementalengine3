///============================================================================
/// \file	EEMemory.h
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

#ifndef	_EEMEMORY_H_
#define _EEMEMORY_H_

// dlmalloc.h
#ifdef __cplusplus
extern "C" {
#endif
void* dlmalloc(size_t);
void  dlfree(void*);
void* dlrealloc(void*, size_t);
#ifdef __cplusplus
};  /* end of extern "C" */
#endif

namespace ElementalEngine 
{

class IMemoryAllocator : public IComponent
{
public:
	virtual void* Malloc( size_t bytes ) = 0;
	virtual void Free( void *p ) = 0;
};

// Memory allocation/deallocation functions
inline void *EE_Debug_malloc( size_t num_bytes )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent(&memAllocName);
	return pAllocator->Malloc( num_bytes );
}

inline void EE_Debug_free( void *p )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	pAllocator->Free( p );
}

inline void *EE_Debug_new( size_t num_bytes )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	return pAllocator->Malloc( num_bytes );
}

inline void EE_Debug_delete( void *p )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	pAllocator->Free( p );
}

inline void *EE_malloc( size_t num_bytes )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	return pAllocator->Malloc( num_bytes );
}

inline void EE_free( void *p )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	pAllocator->Free( p );
}

inline void *EE_new( size_t num_bytes )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	return pAllocator->Malloc( num_bytes );
}

inline void EE_delete( void *p )
{
	static CHashString memAllocName(_T("CMemoryAllocator"));
	static IMemoryAllocator *pAllocator = (IMemoryAllocator*)EngineGetToolBox()->GetComponent( &memAllocName );
	pAllocator->Free( p );
}

} //namespace ElementalEngine 

// Build specific overrides
#ifdef _DEBUG
#define EE_malloc EE_Debug_malloc
#define EE_free EE_Debug_free
#define EE_new EE_Debug_new
#define EE_delete EE_Debug_delete

// Custom global new/delete
inline void *operator new( size_t num_bytes, const char *file, int line, const char *fn )
{
	return ElementalEngine::EE_new(num_bytes);
}

inline void operator delete( void *p, const char *file, int line, const char *fn )
{
	ElementalEngine::EE_delete(p);
}

#endif

#ifdef EE_OVERLOAD_GLOBAL_NEW
inline void *operator new( size_t num_bytes )
{
	return ElementalEngine::EE_new(num_bytes);
}
inline void operator delete( void *p )
{
	ElementalEngine::EE_delete(p);
}
#endif

// Class specific overloaded new
#define EE_NEW_OVERLOAD_POOL \
void *operator new( size_t num_bytes ) { return EE_new(num_bytes); } \
void operator delete( void *p ) { EE_delete(p); }

// Global overloaded new
#define EE_GLOBAL_NEW_OVERLOAD \
	void *operator new( size_t num_bytes ) { return EE_new(num_bytes); } \
	void operator delete( void *p ) { EE_delete(p); }

#ifdef EE_NEW_PREPROCESS_REPLACE
// Preprocessor replacements
#define malloc EE_malloc
#define free EE_free
#define new new("??", 0, "??")
//#define delete
#endif

#endif // _EEMEMORY_H_