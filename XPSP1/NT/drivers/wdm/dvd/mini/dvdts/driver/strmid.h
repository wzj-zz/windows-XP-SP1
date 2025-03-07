/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    strmid.h

Abstract:

    sream descriptors and other definitions for DVDTS    

Environment:

    Kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1998 Microsoft Corporation.  All Rights Reserved.

  Some portions adapted with permission from code Copyright (c) 1997-1998 Toshiba Corporation

Revision History:

	5/1/98: created

--*/

#include "ksmedia.h"	// Definition ?



GUID g_S3Guid = {DDVPTYPE_E_HREFL_VREFL};
GUID g_ATIGuid = {0x1352A560L,0xDA61,0x11CF,0x9B,0x06,0x00,0xA0,0xC9,0x03,0xA3,0xB8};	// DDVPTYPE_BROOKTREE

// define this macro to facilitate giving the pixel format
#define MKFOURCC(ch0, ch1, ch2, ch3)    ((DWORD)(BYTE)(ch0) |           \
					((DWORD)(BYTE)(ch1) << 8) |     \
					((DWORD)(BYTE)(ch2) << 16) |    \
					((DWORD)(BYTE)(ch3) << 24 ))


/*****************************************************************************

                define the data formats used by the pins in this minidriver

*****************************************************************************/

/*

 Define the Mpeg2Video format that the minidriver supports

 */

KSDATAFORMAT hwfmtiMpeg2Vid
     = {

	sizeof (KSDATAFORMAT),
	0,
	0,
	0,

	//
	// specify media type, subtype, and format from mpeg2 video
	//

    STATIC_KSDATAFORMAT_TYPE_DVD_ENCRYPTED_PACK,
	STATIC_KSDATAFORMAT_SUBTYPE_MPEG2_VIDEO,
	STATIC_KSDATAFORMAT_SPECIFIER_MPEG2_VIDEO

	};

/*

 Define the supported AC3 audio format

 */

KSDATAFORMAT_WAVEFORMATEX Mpeg2AudioFormat = {
   {
      sizeof (KSDATAFORMAT_WAVEFORMATEX),
      0,
      1,
      0,
      STATIC_KSDATAFORMAT_TYPE_DVD_ENCRYPTED_PACK,
      STATIC_KSDATAFORMAT_SUBTYPE_AC3_AUDIO,
      STATIC_KSDATAFORMAT_SPECIFIER_WAVEFORMATEX
   },
   {
      0,	// WAVE_FORMAT_UNKNOWN
      6,	// channels
      48000,  // sampling rate
      0,  // byte rate
      768,	// align
      0,	// resolution
      0	// extra
   }
};

/*

Define the supported LPCM audio format.

*/

KSDATAFORMAT_WAVEFORMATEX LPCMAudioFormat = {
   {
      sizeof (KSDATAFORMAT_WAVEFORMATEX),
      0,
      1,
      0,
      STATIC_KSDATAFORMAT_TYPE_DVD_ENCRYPTED_PACK,
      STATIC_KSDATAFORMAT_SUBTYPE_LPCM_AUDIO,
      STATIC_KSDATAFORMAT_SPECIFIER_WAVEFORMATEX
   },
   {
      WAVE_FORMAT_PCM,
      2,	// channels
      48000,  // sampling rate
      192000,  // byte rate
      4,	// alignment
      16,	// bit resolution
      0	// extra
   }
};


/*

 Define the supported Sub picture format

 */

KSDATAFORMAT hwfmtiMpeg2Subpic = {
	sizeof (KSDATAFORMAT),
	0,
	0,
	0,

	//
	// specify media type, subtype
	//

    STATIC_KSDATAFORMAT_TYPE_DVD_ENCRYPTED_PACK,
//	STATIC_KSDATAFORMAT_TYPE_MPEG2_PES,
	STATIC_KSDATAFORMAT_SUBTYPE_SUBPICTURE,
//	STATIC_KSDATAFORMAT_FORMAT_NONE
//	STATIC_KSDATAFORMAT_SPECIFIER_MPEG2_VIDEO
//	STATIC_KSDATAFORMAT_FORMAT_MPEG2_VIDEO
	STATIC_GUID_NULL
};


KSDATAFORMAT hwfmtiVPEOut
    = {
	sizeof (KSDATAFORMAT),
    0,
	0,
	0,
    STATIC_KSDATAFORMAT_TYPE_VIDEO,
    STATIC_KSDATAFORMAT_SUBTYPE_VPVideo,
//  STATIC_KSDATAFORMAT_TYPE_VIDEO,
    STATIC_KSDATAFORMAT_SPECIFIER_NONE
//  STATIC_KSDATAFORMAT_TYPE_VIDEO
    };

KSDATAFORMAT hwfmtiCCOut
    = {
	sizeof(KSDATAFORMAT),
    0,
	200,
	0,
    STATIC_KSDATAFORMAT_TYPE_AUXLine21Data,
    STATIC_KSDATAFORMAT_SUBTYPE_Line21_GOPPacket,
//    STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    STATIC_KSDATAFORMAT_SPECIFIER_NONE
    };


//
// this array indicates that stream 0 (as constructed in sscmd.c) supports
// the MPEG 2 video format.  If stream zero supported more formats, the 
// addresses of these formats would be additional elements in this array.
//

PKSDATAFORMAT Mpeg2VidInfo[] = {
    &hwfmtiMpeg2Vid  // pointer to the MPEG 2 video format block
};

//
// this structure indicates that the audio stream supports two
// formats: AC3 (Mpeg2) and Linear PCM, as constructed above.
//

PKSDATAFORMAT AudioFormatBlocks[] = {
	(PKSDATAFORMAT) &Mpeg2AudioFormat, // pointer to the Mpeg2 format
	(PKSDATAFORMAT) &LPCMAudioFormat   // pointer to the LPCM format
};


// Sub-pic

PKSDATAFORMAT Mpeg2SubpicInfo[] = {
	&hwfmtiMpeg2Subpic  // pointer to the MPEG 2 subpic format block
};


PKSDATAFORMAT VPEInfo[] = {
    &hwfmtiVPEOut
};

PKSDATAFORMAT CCInfo[] = {   // CC output formats array
    &hwfmtiCCOut
};


/*****************************************************************************

                define the Individual property items for the video property sets

*****************************************************************************/

// Video

static const KSPROPERTY_ITEM mpegVidPropItm[]={
	{KSPROPERTY_DVDSUBPIC_PALETTE,
	FALSE,
	sizeof(KSPROPERTY),
	sizeof(KSPROPERTY_SPPAL),
	(PFNKSHANDLER) FALSE,
	NULL,
	0,
	NULL,
	NULL,
	0
	}};

// Audio

static const KSPROPERTY_ITEM audPropItm[] = {

    {KSPROPERTY_AUDDECOUT_MODES,// available audio decoder output formats
        // property
        (PFNKSHANDLER) TRUE,    // get available modes is supported
        sizeof(KSPROPERTY),
        sizeof(ULONG),          // minimum size of data requested
        (PFNKSHANDLER) FALSE,   // set available modes is not supported
        NULL,
        0,
        NULL,
        NULL,
        0
    },

    {KSPROPERTY_AUDDECOUT_CUR_MODE, // current audio decoder output format
        // property
        (PFNKSHANDLER) TRUE,    // get current mode is supported
        sizeof(KSPROPERTY),
        sizeof(ULONG),          // minimum size of data requested
        (PFNKSHANDLER) TRUE,    // set current modes is supported
        NULL,
        0,
        NULL,
        NULL,
        0
    }
};

// Sub-pic

static const KSPROPERTY_ITEM spPropItm[]={

	{KSPROPERTY_DVDSUBPIC_PALETTE,
	FALSE,
	sizeof (KSPROPERTY),
	sizeof (KSPROPERTY_SPPAL),
	(PFNKSHANDLER) TRUE,
	NULL,
	0,
	NULL,
	NULL,
	0
	},


	{KSPROPERTY_DVDSUBPIC_HLI,
	FALSE,
	sizeof (KSPROPERTY),
	sizeof (KSPROPERTY_SPHLI),
	(PFNKSHANDLER)TRUE,
	NULL,
	0,
	NULL,
	NULL,
	0
	},


	{KSPROPERTY_DVDSUBPIC_COMPOSIT_ON,
	FALSE,
	sizeof (KSPROPERTY),
	sizeof (KSPROPERTY_COMPOSIT_ON),
	(PFNKSHANDLER)TRUE,
	NULL,
	0,
	NULL,
	NULL,
	0
	}

	};

// NTSC

static const KSPROPERTY_ITEM MacroVisionPropItm[] = {
	{
		KSPROPERTY_COPY_MACROVISION,		// support for setting macrovision level
		(PFNKSHANDLER) FALSE, 		// get not supported
		sizeof (KSPROPERTY),
		sizeof (KS_COPY_MACROVISION),
		(PFNKSHANDLER) TRUE,		// set MACROVISION level supported
		NULL,
		0,
		NULL,
		NULL,
		0
	}
};

// VPE

static /* const */ KSPROPERTY_ITEM VideoPortPropItm[]={
	{
		KSPROPERTY_VPCONFIG_NUMCONNECTINFO,
		(PFNKSHANDLER) TRUE,
		sizeof(KSPROPERTY),
		sizeof(ULONG),
		(PFNKSHANDLER) FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_NUMVIDEOFORMAT,
		(PFNKSHANDLER) TRUE,
		sizeof(KSPROPERTY),
		sizeof(ULONG),
		(PFNKSHANDLER) FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_GETCONNECTINFO,
		(PFNKSHANDLER) TRUE,
		sizeof (KSMULTIPLE_DATA_PROP),  // minimum property input size
		sizeof (ULONG),
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_SETCONNECTINFO,
		(PFNKSHANDLER) FALSE,
		sizeof (KSPROPERTY),  			// minimum property input size
		sizeof (ULONG),					// minimum buffer size
		(PFNKSHANDLER)TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_VPDATAINFO,
		(PFNKSHANDLER) TRUE,
		sizeof (KSPROPERTY),
		sizeof (KS_AMVPDATAINFO),
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_MAXPIXELRATE,
		(PFNKSHANDLER) TRUE,
		sizeof (KSVPSIZE_PROP),
		sizeof (KSVPMAXPIXELRATE),
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_INFORMVPINPUT,
		(PFNKSHANDLER) TRUE,
		sizeof (PKSPROPERTY),
		sizeof (DDPIXELFORMAT),	// could be 0 too
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_DDRAWHANDLE,
		(PFNKSHANDLER)FALSE,
		sizeof (PKSPROPERTY),
		sizeof (ULONG),    // could be 0 too
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_VIDEOPORTID,
		(PFNKSHANDLER)FALSE,
		sizeof (PKSPROPERTY),
		sizeof (ULONG),    // could be 0 too
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_DDRAWSURFACEHANDLE,
		(PFNKSHANDLER)FALSE,
		sizeof (PKSPROPERTY),
		sizeof (ULONG),    // could be 0 too
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_GETVIDEOFORMAT,
		(PFNKSHANDLER) TRUE,
		sizeof (KSMULTIPLE_DATA_PROP), 		// for _GET; KSPROPERTY for _SET
		sizeof (ULONG),				// could be 4 or more
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_SETVIDEOFORMAT,
		(PFNKSHANDLER) FALSE,
		sizeof (KSPROPERTY),  			// minimum property input size
		sizeof (ULONG),					// minimum buffer size
		(PFNKSHANDLER)TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_INVERTPOLARITY,
		(PFNKSHANDLER)TRUE,
		sizeof (KSPROPERTY),
		0,
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_DECIMATIONCAPABILITY,
		(PFNKSHANDLER)TRUE,
		sizeof (KSPROPERTY),
		sizeof (BOOL),
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_VPCONFIG_SCALEFACTOR,
		(PFNKSHANDLER)TRUE,
		sizeof (KSPROPERTY),
		sizeof (KS_AMVPSIZE),
		(PFNKSHANDLER)FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	}
};

// CC

static /* const */ KSPROPERTY_ITEM CCPropItm[] = {
	{
		KSPROPERTY_CONNECTION_ALLOCATORFRAMING,	// support for setting CC buffer size
		(PFNKSHANDLER) TRUE, 					// get supported
		sizeof (KSPROPERTY),
		sizeof (KSALLOCATOR_FRAMING),
		(PFNKSHANDLER) FALSE,					// we only provide the allocator requirments
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_CONNECTION_STATE,			// support for setting CC buffer size
		(PFNKSHANDLER) TRUE, 					// get supported
		sizeof (KSPROPERTY),
		sizeof (KSSTATE),
		(PFNKSHANDLER) FALSE,					// we only provide the allocator requirments
		NULL,
		0,
		NULL,
		NULL,
		0
	}
};

// Copy Protect

static const KSPROPERTY_ITEM cppPropItm[]={

	{
		KSPROPERTY_DVDCOPY_CHLG_KEY,
		(PFNKSHANDLER) TRUE,
		sizeof( KSPROPERTY ),
		sizeof( KS_DVDCOPY_CHLGKEY ),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_DVDCOPY_DVD_KEY1,
		FALSE,
		sizeof( KSPROPERTY ),
		sizeof( KS_DVDCOPY_BUSKEY ),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_DVDCOPY_DEC_KEY2,
		(PFNKSHANDLER) TRUE,
		sizeof( KSPROPERTY ),
		sizeof( KS_DVDCOPY_BUSKEY ),
		(PFNKSHANDLER) FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_DVDCOPY_TITLE_KEY,
		FALSE,
		sizeof( KSPROPERTY ),
		sizeof( KS_DVDCOPY_TITLEKEY ),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_DVDCOPY_DISC_KEY,
		FALSE,
		sizeof( KSPROPERTY ),
		sizeof( KS_DVDCOPY_DISCKEY ),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},
	{
		KSPROPERTY_DVDCOPY_SET_COPY_STATE,
		(PFNKSHANDLER)TRUE,
		sizeof( KSPROPERTY ),
		sizeof( KS_DVDCOPY_SET_COPY_STATE ),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0
	},

//	{KSPROPERTY_DVDCOPY_REGION,  // DVD region request
//								 // the minidriver shall fit in exactly
//								 // one region bit, corresponding to the region
//   								 // that the decoder is currently in
//        (PFNKSHANDLER)TRUE,
//        sizeof(KSPROPERTY),
//        sizeof(KS_DVDCOPY_REGION),	 // minimum size of data requested
//        (PFNKSHANDLER)FALSE,     // set key is not valid
//        NULL,
//        0,
//        NULL,
//        NULL,
//        0
//    },
};

// Rate Change

static const KSPROPERTY_ITEM RateChangePropItm[]={

	{
		KS_AM_RATE_SimpleRateChange,
		(PFNKSHANDLER) TRUE,
		sizeof (KSPROPERTY),
		sizeof (KS_AM_SimpleRateChange),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0,
	},


	{
		KS_AM_RATE_ExactRateChange,
		(PFNKSHANDLER) FALSE,
		sizeof (KSPROPERTY),
		sizeof (KS_AM_ExactRateChange),
		(PFNKSHANDLER) FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0,
	},


	{
		KS_AM_RATE_MaxFullDataRate,
		(PFNKSHANDLER) TRUE,
		sizeof (KSPROPERTY),
		sizeof (KS_AM_MaxFullDataRate),
		(PFNKSHANDLER) FALSE,
		NULL,
		0,
		NULL,
		NULL,
		0,
	},


	{
		KS_AM_RATE_Step,
		(PFNKSHANDLER) FALSE,
		sizeof (KSPROPERTY),
		sizeof (KS_AM_Step),
		(PFNKSHANDLER) TRUE,
		NULL,
		0,
		NULL,
		NULL,
		0,
	}
};

/*****************************************************************************

				define the array of video property sets supported

*****************************************************************************/

//GUID Mpeg2Vid = {STATIC_KSPROPSETID_Mpeg2Vid};

// original is const
// modify for except warning
static /* const*/ KSPROPERTY_SET mpegVidPropSet[] = {
	{
		&KSPROPSETID_Mpeg2Vid,
		SIZEOF_ARRAY(mpegVidPropItm),
		(PKSPROPERTY_ITEM)mpegVidPropItm
	},
	{
		&KSPROPSETID_CopyProt,
		SIZEOF_ARRAY(cppPropItm),
		(PKSPROPERTY_ITEM)cppPropItm
	},
	{
		&KSPROPSETID_TSRateChange,
		SIZEOF_ARRAY(RateChangePropItm),
		(PKSPROPERTY_ITEM)RateChangePropItm
	},

//    {
//    	&KSPROPSETID_CopyProt,
//    	SIZEOF_ARRAY(MacroVisionPropItm),
//    	(PKSPROPERTY_ITEM) MacroVisionPropItm
//    }
};

static /* const*/ KSPROPERTY_SET mpegAudioPropSet[] = {
	{
		&KSPROPSETID_AudioDecoderOut,
		SIZEOF_ARRAY(audPropItm),
		(PKSPROPERTY_ITEM) audPropItm
	},
	{
		&KSPROPSETID_CopyProt,
		SIZEOF_ARRAY(cppPropItm),
		(PKSPROPERTY_ITEM)cppPropItm
	},
	{
		&KSPROPSETID_TSRateChange,
		SIZEOF_ARRAY(RateChangePropItm),
		(PKSPROPERTY_ITEM)RateChangePropItm
	}
};

static KSPROPERTY_SET SPPropSet[] = {
	{
		&KSPROPSETID_DvdSubPic,
		SIZEOF_ARRAY(spPropItm),
		(PKSPROPERTY_ITEM)spPropItm
	},
	{
		&KSPROPSETID_CopyProt,
		SIZEOF_ARRAY(cppPropItm),
		(PKSPROPERTY_ITEM)cppPropItm
	},
	{
		&KSPROPSETID_TSRateChange,
		SIZEOF_ARRAY(RateChangePropItm),
		(PKSPROPERTY_ITEM)RateChangePropItm
	}
};

GUID vpePropSetid = {STATIC_KSPROPSETID_VPConfig};

static /* const */ KSPROPERTY_SET VideoPortPropSet[] = {
	&vpePropSetid,
	SIZEOF_ARRAY(VideoPortPropItm),
	(PKSPROPERTY_ITEM)VideoPortPropItm
};

static /* const */ KSPROPERTY_SET CCPropSet[] = {
	&KSPROPSETID_Connection,
	SIZEOF_ARRAY(CCPropItm),
	(PKSPROPERTY_ITEM) CCPropItm
};

/*****************************************************************************

				other

*****************************************************************************/

static const KSTOPOLOGY_CONNECTION Connections[] = {
/*  FromNode   FromPin           ToNode   ToPin          */
   {(ULONG)-1,        strmVideo,        (ULONG)-1,      strmCCOut},
   {(ULONG)-1,        strmVideo,        (ULONG)-1,      strmYUVVideo},
   {(ULONG)-1,        strmSubpicture,   (ULONG)-1,      strmYUVVideo}
};

static const KSTOPOLOGY Topology = {
	1,
	(GUID *) & KSCATEGORY_DATADECOMPRESSOR,
	0,
	NULL,
   SIZEOF_ARRAY (Connections),
   Connections
};

KSEVENT_ITEM VPEventItm[] =
{
	{
		KSEVENT_VPNOTIFY_FORMATCHANGE,
		0,
		0,
		NULL,
		NULL,
		NULL
	}
};

GUID MY_KSEVENTSETID_VPNOTIFY = {STATIC_KSEVENTSETID_VPNotify};

KSEVENT_SET VPEventSet[] =
{
	{
		&MY_KSEVENTSETID_VPNOTIFY,
		SIZEOF_ARRAY(VPEventItm),
		VPEventItm,
	}
};

KSEVENT_ITEM ClockEventItm[] =
{
	{
        KSEVENT_CLOCK_POSITION_MARK,		// position mark event supported
		sizeof (KSEVENT_TIME_MARK),			// requires this data as input
		sizeof (KSEVENT_TIME_MARK),			// allocate space to copy the data
		NULL,
		NULL,
		NULL
	},
	{
		KSEVENT_CLOCK_INTERVAL_MARK,		// interval mark event supported
		sizeof (KSEVENT_TIME_INTERVAL),		// requires interval data as input
		sizeof (MYTIME),					// we use an additional workspace of
											// size longlong for processing
											// this event
		NULL,
		NULL,
		NULL
	}
};

KSEVENT_SET ClockEventSet[] =
{
	{
		&KSEVENTSETID_Clock,
		SIZEOF_ARRAY(ClockEventItm),
		ClockEventItm,
	}
};


// device property

typedef struct _DevPropData {
	DWORD	data;
} DEVPROPDATA, *PDEVPROPDATA;

static const KSPROPERTY_ITEM devicePropItm[]={
	{0,
	(PFNKSHANDLER)TRUE,
	sizeof(KSPROPERTY),
	sizeof(DEVPROPDATA),
	(PFNKSHANDLER)TRUE,
	NULL,
	0,
	NULL,
	NULL,
	0
	}};

static /* const*/ KSPROPERTY_SET devicePropSet[] = {
	&GUID_NULL,
	SIZEOF_ARRAY(devicePropItm),
	(PKSPROPERTY_ITEM)devicePropItm
};
