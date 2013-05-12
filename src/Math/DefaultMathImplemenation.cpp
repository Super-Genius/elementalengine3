

#include "stdafx.h"

#include "DefaultVec3Implementation.h"
#include "DefaultVec4Implementation.h"
#include "DefaultQuaternionImpl.h"
#include "DefaultMatrix3x3Impl.h"
#include "DefaultMatrix4x4Impl.h"
#include "DefaultMathUtilImpl.h"
#include "DefaultPlaneImpl.h"
#include "DefaultRandomImpl.h"
#include "DefaultRayImplementation.h"
#include "DefaultAABBImplementation.h"
#include "DefaultEulerImplementation.h"

#define MATH_PRIORITY 1000

namespace ElementalEngine
{
extern int InitializeMathImplementations();

void InitDefaultMathImplementation()
{
	static MATHIMPLEMENTATIONMAP mapImplementation;
	mapImplementation[CHashString(_T("Vec3DefaultImplementation")).GetUniqueID()] = CDefaultVec3Implementation::Instance();
	mapImplementation[CHashString(_T("Vec4DefaultImplementation")).GetUniqueID()] = CDefaultVec4Implementation::Instance();
	mapImplementation[CHashString(_T("QuaternionDefaultImplementation")).GetUniqueID()] = CDefaultQuaternionImplementation::Instance();
	mapImplementation[CHashString(_T("Matrix3x3DefaultImplementation")).GetUniqueID()] = CDefaultMatrix3x3Implementation::Instance();
	mapImplementation[CHashString(_T("Matrix4x4DefaultImplementation")).GetUniqueID()] = CDefaultMatrix4x4Implementation::Instance();
	mapImplementation[CHashString(_T("MathUtilDefaultImplementation")).GetUniqueID()] = CDefaultMathUtilImplementation::Instance();
	mapImplementation[CHashString(_T("PlaneDefaultImplementation")).GetUniqueID()] = CDefaultPlaneImplementation::Instance();
	mapImplementation[CHashString(_T("RandomDefaultImplementation")).GetUniqueID()] = CDefaultRandomImplementation::Instance();
	mapImplementation[CHashString(_T("AABBDefaultImplementation")).GetUniqueID()] = CDefaultAABBImplementation::Instance();
	mapImplementation[CHashString(_T("RayDefaultImplementation")).GetUniqueID()] = CDefaultRayImplementation::Instance();
	mapImplementation[CHashString(_T("EulerDefaultImplementation")).GetUniqueID()] = CDefaultEulerImplementation::Instance();

	REGISTERMATHIMPLEMENTATION msg;
	msg.iPriority = MATH_PRIORITY;
	msg.dwLibraryName = CHashString(_T("DefaultMath")).GetUniqueID();
	msg.pImplementationMap = &mapImplementation;
	static DWORD msgHash_RegisterMathImplementation = CHashString(_T("RegisterMathImplementation")).GetUniqueID();
	EngineGetToolBox()->SendMessage(msgHash_RegisterMathImplementation, sizeof(msg), &msg );

	// Reinitialize the local implementation pointers for this module.
	InitializeMathImplementations();
}

}
