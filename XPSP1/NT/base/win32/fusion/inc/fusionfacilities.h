/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    fusionfacilities.h

Abstract:

    Include file that is included MULTIPLE TIMES to get varying effects
    of defining global variables vs. references to those global variables
    vs. symbols that you can use when writing output to scope it to
    a facility.

Author:

    Michael J. Grier (MGrier) 7/28/2000

Revision History:

--*/

//
//  NO INCLUSION PROTECTION!!! BY DESIGN!!!
//
//  NO PRAGMA ONCE PLEASE!!
//

#if !defined(FUSION_FACILITY_MASK_NAME)
#define FUSION_FACILITY_MASK_NAME(_x) g_FUSION_ ## _x ## _MASK
#endif

#if defined(FUSION_DEFINE_FACILITY_MASK_VARIABLE_DEFS)

#if !defined(FUSION_FACILITY_VARIABLE_DEFS_DEFINED)
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) DWORD FUSION_FACILITY_MASK_NAME(_facname);
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#define FUSION_FACILITY_VARIABLE_DEFS_DEFINED 1
#else
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#endif

#elif defined(FUSION_DEFINE_FACILITY_MASK_VARIABLE_ARRAY)

#if !defined(FUSION_FACILITY_MASK_VARIABLE_ARRAY_DEFINED)
#define FUSION_FACILITY_BEFORE_FACILITIES() static const DWORD *s_rgFusionFacilityMasks[] = {
#define FUSION_FACILITY(_facname) &FUSION_FACILITY_MASK_NAME(_facname),
#define FUSION_FACILITY_AFTER_FACILITIES() };
#define FUSION_FACILITY_MASK_VARIABLE_ARRAY_DEFINED 1
#else
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#endif

#elif defined(FUSION_DEFINE_FACILITY_STRUCTURE)

#if !defined(FUSION_FACILITY_STRUCTURE_DEFINED)
#define FUSION_FACILITY_BEFORE_FACILITIES() class CFusionFacilityList { public:
#define FUSION_FACILITY(_facname) char _facname;
#define FUSION_FACILITY_AFTER_FACILITIES() };
#define FUSION_FACILITY_STRUCTURE_DEFINED 1
#else
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#endif

#elif defined(FUSION_DEFINE_FACILITY_MASK_VARIABLE_REFS)

#if !defined(FUSION_FACILITY_VARIABLE_REFS_DEFINED)
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) EXTERN_C DWORD FUSION_FACILITY_MASK_NAME(_facname);
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#define FUSION_FACILITY_VARIABLE_REFS_DEFINED 1
#else
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#endif

#elif defined(FUSION_DEFINE_FACILITY_CONSTANTS)

#if !defined(FUSION_FACILITY_CONSTANTS_DEFINED)
#define FUSION_DEFINE_FACILITY_STRUCTURE
#include "fusionfacilities.h"
#undef FUSION_DEFINE_FACILITY_STRUCTURE
#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

#define FUSION_DEFINE_FACILITY_MASK_VARIABLE_REFS
#include "fusionfacilities.h"
#undef FUSION_DEFINE_FACILITY_MASK_VARIABLE_REFS
#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) const ULONG FUSION_ ## _facname ## _ID = ((ULONG) FIELD_OFFSET(CFusionFacilityList, _facname));
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */

#define FUSION_FACILITY_CONSTANTS_DEFINED 1
#else
#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */
#endif

#elif defined(FUSION_DEFINE_FACILITY_DLLMAIN_DEFS)

#if !defined(FUSION_FACILITY_DLLMAIN_DEFS_DEFINED)

#define FUSION_DEFINE_FACILITY_CONSTANTS
#include "fusionfacilities.h"
#undef FUSION_DEFINE_FACILITY_CONSTANTS
#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

#define FUSION_DEFINE_FACILITY_MASK_VARIABLE_DEFS
#include "fusionfacilities.h"
#undef FUSION_DEFINE_FACILITY_MASK_VARIABLE_DEFS
#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

#define FUSION_DEFINE_FACILITY_MASK_VARIABLE_ARRAY
#include "fusionfacilities.h"
#undef FUSION_DEFINE_FACILITY_MASK_VARIABLE_ARRAY
#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

#define FUSION_FACILITY_DLLMAIN_DEFS_DEFINED 1
#endif

#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */

#else

#define FUSION_DEFINE_FACILITY_CONSTANTS
#include "fusionfacilities.h"
#undef FUSION_DEFINE_FACILITY_CONSTANTS

#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

#define FUSION_FACILITY_BEFORE_FACILITIES() /* nothing */
#define FUSION_FACILITY(_facname) /* nothing */
#define FUSION_FACILITY_AFTER_FACILITIES() /* nothing */

#endif

//
//  Add fusion facility definitions here:
//

FUSION_FACILITY_BEFORE_FACILITIES()

FUSION_FACILITY(HASH_TABLE)
FUSION_FACILITY(DEQUE)
FUSION_FACILITY(STRING_BUFFER)
FUSION_FACILITY(BINDING)
FUSION_FACILITY(POLICY)

FUSION_FACILITY_AFTER_FACILITIES()

#undef FUSION_FACILITY_BEFORE_FACILITIES
#undef FUSION_FACILITY
#undef FUSION_FACILITY_AFTER_FACILITIES

