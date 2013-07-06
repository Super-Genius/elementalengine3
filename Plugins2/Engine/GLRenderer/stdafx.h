///============================================================================
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

#pragma once

#include "Platform.h"

#include <assert.h>
#include <math.h>

#ifdef _DEBUG
// This define must occur before any headers are included.
//#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif

#include <OpenGL/gl.h>
#include <stack>
#include "ElementalEngine.hpp"
#include "Register.h"
#include "ShaderConstants.h"
#include "Config.h"
#include "CGLVertexBufferObject.h"
#include "CGLShaderCommon.h"
#include "CGLPixelShader.h"
#include "CGLVertexShader.h"
#include "CGLRenderer.h"
#include "CGLRenderContext.h"
#include "GLPlatform.h"
