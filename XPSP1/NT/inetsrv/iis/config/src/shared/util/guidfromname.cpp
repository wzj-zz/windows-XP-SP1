
// GuidFromName

// Algorithm from Internet Draft document "UUIDs and GUIDs"
// By Paul J. Leach and Rich Sals, February 4, 1998.

// This function has been adapted from the routines in the document
//	uuid_create_from_name and format_uuid_v3

// Changes from documented routines:
// 1. Changed all instances of uuid_t to GUID.
//		uuid_t field time_low is GUID field Data1.
//		uuid_t field time_mid is GUID field Data2.
//		uuid_t field time_hi_and_version is GUID field Data3.
//		uuid_t field clock_seq_hi_and_reserved is GUID field Data4[0].
//		uuid_t field clock_seq_low is GUID field Data4[1].
//		uuid_t field node[6] is GUID field Data4[2] through Data4[8].
//
// 2. Use a c++ implementation of the md5 cryptographic hash function.
//
// 3. Implemented the htonl, htons, ntohl, ntohs socket routines as inlines.
//
// 4. Renamed variables and types to suit my biases.

/*
** Copyright (c) 1990- 1993, 1996 Open Software Foundation, Inc.
** Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
** Digital Equipment Corporation, Maynard, Mass.
** Copyright (c) 1995-1999 Microsoft.  All rights reserved.
** To anyone who acknowledges that this file is provided "AS IS"
** without any express or implied warranty: permission to use, copy,
** modify, and distribute this file for any purpose is hereby
** granted without fee, provided that the above copyright notices and
** this notice appears in all source code copies, and that none of
** the names of Open Software Foundation, Inc., Hewlett-Packard
** Company, or Digital Equipment Corporation be used in advertising
** or publicity pertaining to distribution of the software without
** specific, written prior permission.  Neither Open Software
** Foundation, Inc., Hewlett-Packard Company, Microsoft, nor Digital Equipment
** Corporation makes any representations about the suitability of
** this software for any purpose.
*/

#include <windows.h>				// windows standard includes
#include <wtypes.h>
#include <md5.h>				// cryptographic hash function
#include <GuidFromName.h>		// verify our function signature

// Figure out if we're compiling for a big- or little-endian machine.
// This is evaluated at compile time in retail builds.

inline bool BigEndian()
{
	unsigned long n = 0xff000000L;

	return 0 != *reinterpret_cast<unsigned char *>(&n);
}

//=============================================================================
// htons, htonl, ntohs, ntohl equivalents copied and adapted from socket library.
//=============================================================================

// HostToNetworkLong converts a 32-bit long to network byte order

inline ULONG HostToNetworkLong(ULONG hostlong)
{
	if (BigEndian())
		return hostlong;
	else
		return	( (hostlong >> 24) & 0x000000FFL) |
				( (hostlong >>  8) & 0x0000FF00L) |
				( (hostlong <<  8) & 0x00FF0000L) |
				( (hostlong << 24) & 0xFF000000L);
}

// HostToNetworkLong converts a 16-bit short to network byte order

inline USHORT HostToNetworkShort(USHORT hostshort)
{
	if (BigEndian())
		return hostshort;
	else
		return ((hostshort >> 8) & 0x00FF) | ((hostshort << 8) & 0xFF00);
}

// NetworkToHostLong converts a 32-bit long to local host byte order

inline ULONG NetworkToHostLong(ULONG netlong)
{
	if (BigEndian())
		return netlong;
	else
		return	( (netlong >> 24) & 0x000000FFL) |
				( (netlong >>  8) & 0x0000FF00L) |
				( (netlong <<  8) & 0x00FF0000L) |
				( (netlong << 24) & 0xFF000000L);
}

// NetworkToHostShort converts a 16-bit short to local host byte order

inline USHORT NetworkToHostShort(USHORT netshort)
{
	if (BigEndian())
		return netshort;
	else
		return ((netshort >> 8) & 0x00FF) | ((netshort << 8) & 0xFF00);
}

//=============================================================================
// GuidFromName(GUID * pGuidResult, REFGUID refGuidNsid, 
//				const void * pvName, DWORD dwcbName);
//=============================================================================

void GuidFromName
(
	GUID *	pGuidResult,		// resulting GUID
	REFGUID	refGuidNsid,		// Name Space GUID, so identical names from
								// different name spaces generate different GUIDs
	const void * pvName,		// the name from which to generate a GUID
	DWORD dwcbName				// name length in bytes
)
{
	MD5			md5;			// Cryptographic hash class instance
	MD5HASHDATA	md5HashData;	// 128-bit hash result
	GUID		guidNsid;		// context NameSpace GUID in network byte order


	// put name space ID in network byte order so it hashes the same
	// no matter what endian machine we're on

	guidNsid = refGuidNsid;

	// The sample code in the IETF draft document discards the result of
	// htonl and htons.  I've implemented what I think is meant and I've
	// sent a note to the author asking for confirmation that this is
	// his intent.

	if (!BigEndian())	// evaluated at compile time in retail builds
	{
		guidNsid.Data1 = HostToNetworkLong (guidNsid.Data1);
		guidNsid.Data2 = HostToNetworkShort(guidNsid.Data2);
		guidNsid.Data3 = HostToNetworkShort(guidNsid.Data3);
	}

	md5.Init();
	md5.HashMore(&guidNsid, sizeof(GUID));
	md5.HashMore(pvName, dwcbName);
	md5.GetHashValue(&md5HashData);

	// the hash is in network byte order at this point

	memcpy(pGuidResult, &md5HashData, sizeof(GUID));

	// Remainder adapted from function "format_uuid_v3" in IETF draft document

	// Construct a version 3 uuid with the pseudo-random number plus a few constants.

	// convert GUID from network order to local byte order

	if (!BigEndian())	// evaluated at compile time in retail builds
	{
		pGuidResult->Data1 = NetworkToHostLong (pGuidResult->Data1);
		pGuidResult->Data2 = NetworkToHostShort(pGuidResult->Data2);
		pGuidResult->Data3 = NetworkToHostShort(pGuidResult->Data3);
	}

	// set version number 
	pGuidResult->Data3 &= 0x0FFF;	// clear version number nibble
	pGuidResult->Data3 |= (3 << 12);// set version 3 = name-based

	// set variant field
	pGuidResult->Data4[0] &= 0x3F;	// clear variant bits
	pGuidResult->Data4[0] |= 0x80;	// set variant = 100b
}
