/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1996-1997 Microsoft Corporation
//
//	Module Name:
//		DDxDDv.h
//
//	Abstract:
//		Definition of custom dialog data exchange/dialog data validation
//		routines.
//
//	Implementation File:
//		DDxDDv.cpp
//
//	Author:
//		David Potter (davidp)	September 5, 1996
//
//	Revision History:
//
//	Notes:
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _DDXDDV_H_
#define _DDXDDV_H_

/////////////////////////////////////////////////////////////////////////////
// Include Files
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Function Prototypes
/////////////////////////////////////////////////////////////////////////////

void AFXAPI DDX_Number(
	IN OUT CDataExchange *	pDX,
	IN int					nIDC,
	IN OUT DWORD &			rdwValue,
	IN DWORD				dwMin,
	IN DWORD				dwMax,
	IN BOOL					bSigned = FALSE
	);
void AFXAPI DDV_RequiredText(
	IN OUT CDataExchange *	pDX,
	IN int					nIDC,
	IN int					nIDCLabel,
	IN const CString &		rstrValue
	);

inline void AFXAPI DDX_Number(
	IN OUT CDataExchange *	pDX,
	IN int					nIDC,
	IN OUT LONG &			rnValue,
	IN LONG					nMin,
	IN LONG					nMax,
	IN BOOL					bSigned
	)
{
	DDX_Number(pDX, nIDC, (DWORD &) rnValue, (DWORD) nMin, (DWORD) nMax, bSigned);
}

/////////////////////////////////////////////////////////////////////////////

#endif // _DDXDDV_H_
