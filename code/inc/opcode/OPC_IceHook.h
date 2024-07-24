
// Should be included by Opcode.h if needed

	#define ICE_DONT_CHECK_COMPILER_OPTIONS

	// From Windows...
	typedef int                 BOOL;
	#ifndef FALSE
	#define FALSE               0
	#endif

	#ifndef TRUE
	#define TRUE                1
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>
	#include <string.h>
	#include <float.h>
	#include <math.h>

	#ifndef ASSERT
		#define	ASSERT(exp)	{}
	#endif
	#define ICE_COMPILE_TIME_ASSERT(exp)	extern char ICE_Dummy[ (exp) ? 1 : -1 ]

	#define	Log				{}
#ifdef __GNUC__
	#define	SetIceError(...)		false
#else
	#define	SetIceError(a,b)	false
#endif
	#define	EC_OUTOFMEMORY	"Out of memory"

	#include "opcode/OPC_Preprocessor.h"

	#undef ICECORE_API
	#define ICECORE_API	OPCODE_API

	#include "opcode/OPC_Types.h"
	#include "opcode/OPC_FPU.h"
	#include "opcode/OPC_MemoryMacros.h"

	namespace IceCore
	{
		#include "opcode/OPC_Utils.h"
		#include "opcode/OPC_Container.h"
		#include "opcode/OPC_Pairs.h"
		#include "opcode/OPC_RevisitedRadix.h"
		#include "opcode/OPC_Random.h"
	}
	using namespace IceCore;

	#define ICEMATHS_API	OPCODE_API
	namespace IceMaths
	{
		#include "opcode/OPC_Axes.h"
		#include "opcode/OPC_Point.h"
		#include "opcode/OPC_HPoint.h"
		#include "opcode/OPC_Matrix3x3.h"
		#include "opcode/OPC_Matrix4x4.h"
		#include "opcode/OPC_Plane.h"
		#include "opcode/OPC_Ray.h"
		#include "opcode/OPC_IndexedTriangle.h"
		#include "opcode/OPC_Triangle.h"
		#include "opcode/OPC_TriList.h"
		#include "opcode/OPC_AABB.h"
		#include "opcode/OPC_OBB.h"
		#include "opcode/OPC_BoundingSphere.h"
		#include "opcode/OPC_Segment.h"
		#include "opcode/OPC_LSS.h"
	}
	using namespace IceMaths;
