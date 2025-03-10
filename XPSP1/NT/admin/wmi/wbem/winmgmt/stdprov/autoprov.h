/*++

Copyright (C) 1997-2001 Microsoft Corporation

Module Name:

    AUTOPROV.H

Abstract:

    Defines the guids for automation provider.

History:

	a-davj  04-Mar-97   Created.

--*/

#ifndef _autoprov_H_
#define _autoprov_H_

DEFINE_GUID(LIBID_autoprov,0xDAC651D0L,0x7CC7,0x11CF,0xA5,0xB6,0x00,0xAA,0x00,0x68,0x0C,0x3F);

DEFINE_GUID(CLSID_AutoProvider,0xDAC651D1L,0x7CC7,0x11CF,0xA5,0xB6,0x00,0xAA,0x00,0x68,0x0C,0x3F);

// {72967902-68EC-11d0-B729-00AA0062CBB7}
DEFINE_GUID(CLSID_AutoPropProv, 
0x72967902, 0x68ec, 0x11d0, 0xb7, 0x29, 0x0, 0xaa, 0x0, 0x62, 0xcb, 0xb7);

#ifdef __cplusplus
class AutoProvider;
#endif

#endif
