/*++

Copyright (c) 1991-1999,  Microsoft Corporation  All rights reserved.

Module Name:

    nlstest.c

Abstract:

    Test module for NLS API.

    NOTE: This code was simply hacked together quickly in order to
          test the different code modules of the NLS component.
          This is NOT meant to be a formal regression test.

Revision History:

    06-14-91    JulieB    Created.

--*/



//
//  Include Files.
//

#include "nlstest.h"




//
//  Global Variables.
//

BOOL Verbose = 0;                      // verbose flag

LCID pAllLocales[] =                   // all supported locale ids
{
    0x0402,
    0x0404,
    0x0804,
    0x0c04,
    0x1004,
    0x0405,
    0x0406,
    0x0407,
    0x0807,
    0x0c07,
    0x0408,
    0x0409,
    0x0809,
    0x0c09,
    0x1009,
    0x1409,
    0x1809,
    0x040a,
    0x080a,
    0x0c0a,
    0x040b,
    0x040c,
    0x080c,
    0x0c0c,
    0x100c,
    0x040e,
    0x040f,
    0x0410,
    0x0810,
    0x0411,
    0x0412,
    0x0413,
    0x0813,
    0x0414,
    0x0814,
    0x0415,
    0x0416,
    0x0816,
    0x0418,
    0x0419,
    0x041a,
    0x041b,
    0x041d,
    0x041f,
    0x0424
};

int NumLocales = ( sizeof(pAllLocales) / sizeof(LCID) );

// Buffer used in GetUnicodeString()/GetAnsiString()
CHAR TempStringBuffer[1024];


////////////////////////////////////////////////////////////////////////////
//
//  main
//
//  Main Routine.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

int _cdecl main(
    int argc,
    char *argv[])
{
    int NumErrs = 0;              // number of errors


    //
    //  Check for verbose switch.
    //
    if ( (argc > 1) && (_stricmp(argv[1], "-v") == 0) )
    {
        Verbose = 1;
    }


    //
    //  Print out what's being done.
    //
    printf("\nTesting NLS Component.\n");


    //
    //  Test MultiByteToWideChar.
    //
    NumErrs += TestMBToWC();


    //
    //  Test WideCharToMultiByte.
    //
    NumErrs += TestWCToMB();


    //
    //  Test GetCPInfo.
    //
    NumErrs += TestGetCPInfo();


    //
    //  Test CompareString.
    //
    NumErrs += TestCompareString();


    //
    //  Test GetStringType.
    //
    NumErrs += TestGetStringType();


    //
    //  Test FoldString.
    //
    NumErrs += TestFoldString();


    //
    //  Test LCMapString.
    //
    NumErrs += TestLCMapString();


    //
    //  Test GetLocaleInfo.
    //
    NumErrs += TestGetLocaleInfo();


    //
    //  Test SetLocaleInfo.
    //
    NumErrs += TestSetLocaleInfo();


    //
    //  Test GetCalendarInfo.
    //
    NumErrs += TestGetCalendarInfo();


    //
    //  Test SetCalendarInfo.
    //
    NumErrs += TestSetCalendarInfo();


    //
    //  Test GetGeoInfo.
    //
    NumErrs += TestGetGeoInfo();


    //
    //  Test GetTimeFormat.
    //
    NumErrs += TestGetTimeFormat();


    //
    //  Test GetDateFormat.
    //
    NumErrs += TestGetDateFormat();


    //
    //  Test GetNumberFormat.
    //
    NumErrs += TestGetNumberFormat();


    //
    //  Test GetCurrencyFormat.
    //
    NumErrs += TestGetCurrencyFormat();


    //
    //  Test IsDBCSLeadByte.
    //
    NumErrs += TestIsDBCSLeadByte();


    //
    //  Test IsValidCodePage.
    //
    NumErrs += TestIsValidCodePage();


    //
    //  Test IsValidLanguageGroup.
    //
    NumErrs += TestIsValidLanguageGroup();


    //
    //  Test IsValidLocale.
    //
    NumErrs += TestIsValidLocale();


    //
    //  Test GetACP, GetOEMCP,
    //       GetSystemDefaultUILanguage, GetUserDefaultUILanguage,
    //       GetSystemDefaultLangID, GetUserDefaultLangID,
    //       GetSystemDefaultLCID, GetUserDefaultLCID,
    //       GetThreadLocale, SetThreadLocale
    //
    NumErrs += TestUtilityAPIs();


    //
    //  Test EnumUILanguages.
    //
    NumErrs += TestEnumUILanguages();


    //
    //  Test EnumSystemLanguageGroups.
    //
    NumErrs += TestEnumSystemLanguageGroups();


    //
    //  Test EnumLanguageGroupLocales.
    //
    NumErrs += TestEnumLanguageGroupLocales();


    //
    //  Test EnumSystemLocales.
    //
    NumErrs += TestEnumSystemLocales();


    //
    //  Test EnumSystemCodePages.
    //
    NumErrs += TestEnumSystemCodePages();


    //
    //  Test EnumCalendarInfo.
    //
    NumErrs += TestEnumCalendarInfo();


    //
    //  Test EnumTimeFormats.
    //
    NumErrs += TestEnumTimeFormats();


    //
    //  Test EnumDateFormats.
    //
    NumErrs += TestEnumDateFormats();


    //
    //  Test EnumSystemGeoID.
    //
    NumErrs += TestEnumSystemGeoID();


    //
    //  Print out final result.
    //
    if (NumErrs == 0)
        printf("\n\n\nNO Errors Found.\n\n");
    else
        printf("\n\n\n%d  ERRORS FOUND.\n\n", NumErrs);


    //
    //  Return number of errors found.
    //
    return (NumErrs);


    argc;
    argv;
}


////////////////////////////////////////////////////////////////////////////
//
//  CompStringsW
//
//  Compares a wide character string to another wide character string.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

int CompStringsW(
    WCHAR *WCStr1,
    WCHAR *WCStr2,
    int   size)
{
    int ctr;                      // loop counter


    for (ctr = 0; WCStr1[ctr] == WCStr2[ctr]; ctr++)
    {
        if (ctr == (size - 1))
            return (0);
    }

    return (1);
}


////////////////////////////////////////////////////////////////////////////
//
//  CompStringsA
//
//  Compares a multibyte string to another multibyte character string.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

int CompStringsA(
    BYTE *MBStr1,
    BYTE *MBStr2,
    int  size)
{
    int ctr;                      // loop counter

    for (ctr = 0; MBStr1[ctr] == MBStr2[ctr]; ctr++)
    {
        if (ctr == (size - 1))
            return (0);
    }

    return (1);
}


////////////////////////////////////////////////////////////////////////////
//
//  PrintWC
//
//  Prints out a wide character string to the screen.
//  Need the size parameter because the string may not be zero terminated.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void PrintWC(
    WCHAR *WCStr,
    int   size)
{
    int ctr;                      // loop counter

    //
    //  Print the wide character string.
    //
    printf("       WC String  =>  [%s]\n", GetUnicodeString(WCStr, size));
}


////////////////////////////////////////////////////////////////////////////
//
//  PrintMB
//
//  Prints out a multibyte character string to the screen.
//  Need the size parameter because the string may not be zero terminated.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void PrintMB(
    BYTE *MBStr,
    int   size)
{
    int ctr;                      // loop counter


    //
    //  Print the multibyte character string.
    //
    printf("       MB String  =>  [");
    for (ctr = 0; ctr < size; ctr++)
    {
        printf(((MBStr[ctr] < 0x21) || (MBStr[ctr] > 0x7e)) ? "(0x%x)" : "%c",
               MBStr[ctr]);
    }
    printf("]\n");
}

LPSTR GetUnicodeString(LPWSTR wstr, int count)
{
    int i;
    LPSTR lpResult = TempStringBuffer;
    if (count == -1)
    {
        count = wcslen(wstr) + 1;
    }


    for (i = 0; i < count; i++)
    {
        lpResult += sprintf(lpResult, "\\x%04x", (int)wstr[i]);
    }
    lpResult = L'\0';

    return (TempStringBuffer);
}

LPSTR GetAnsiString(LPSTR str, int count)
{
    int i;
    LPSTR lpResult = TempStringBuffer;
    if (count == -1)
    {
        count = strlen(str);
    }

    for (i = 0; i < count; i++)
    {
        lpResult += sprintf(lpResult, "\\x%02x", (unsigned char)str[i]);
    }
    lpResult = L'\0';

    return (TempStringBuffer);
}

////////////////////////////////////////////////////////////////////////////
//
//  CheckLastError
//
//  Checks the last error value.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckLastError(
    DWORD ExpectedLastError,
    LPSTR pErrString,
    int *pNumErrors)
{
    DWORD CurrentLastError;       // last error
    WCHAR Buffer[512];


    if ((CurrentLastError = GetLastError()) != ExpectedLastError)
    {
        printf(">>>>ERROR: %s - \n", pErrString);
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, CurrentLastError, 0, Buffer, 512, NULL);
        wprintf(L"    LastError = %x, String: %s\n", CurrentLastError, Buffer);
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, ExpectedLastError, 0, Buffer, 512, NULL);
        wprintf(L"    Expected = %x, String: %s\n", ExpectedLastError, Buffer);

        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnBadParam
//
//  Checks the return code from a call with a bad parameter.  It prints out
//  the appropriate error if either the return code or the last error is
//  incorrect.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnBadParam(
    int CurrentReturn,
    int ExpectedReturn,
    DWORD ExpectedLastError,
    LPSTR pErrString,
    int *pNumErrors)
{
    DWORD CurrentLastError;       // last error


    if ( (CurrentReturn != ExpectedReturn) ||
         ((CurrentLastError = GetLastError()) != ExpectedLastError) )
    {
        printf("ERROR: %s - \n", pErrString);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);
        printf("  LastError = %d, Expected = %d\n", CurrentLastError, ExpectedLastError);
        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnBadParamEnum
//
//  Checks the return code from an enumeration call with a bad parameter.
//  It prints out the appropriate error if either the return code, the last
//  error, or the enumeration counter is incorrect.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnBadParamEnum(
    int CurrentReturn,
    int ExpectedReturn,
    DWORD ExpectedLastError,
    LPSTR pErrString,
    int *pNumErrors,
    int CurrentEnumCtr,
    int ExpectedEnumCtr)
{
    DWORD CurrentLastError;       // last error


    if ( (CurrentReturn != ExpectedReturn) ||
         ((CurrentLastError = GetLastError()) != ExpectedLastError) ||
         (CurrentEnumCtr != ExpectedEnumCtr) )
    {
        printf("ERROR: %s - \n", pErrString);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);
        printf("  LastError = %d, Expected = %d\n", CurrentLastError, ExpectedLastError);
        printf("  EnumCtr = %d, Expected = %d\n", CurrentEnumCtr, ExpectedEnumCtr);
        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnEqual
//
//  Checks the return code from the valid NLS api "A" call to be sure that
//  it does NOT equal a particular value.  If it does equal that value,
//  then it prints out the appropriate error.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnEqual(
    int CurrentReturn,
    int NonExpectedReturn,
    LPSTR pErrString,
    int *pNumErrors)
{
    if (CurrentReturn == NonExpectedReturn)
    {
        printf("ERROR: %s - \n", pErrString);
        printf("  Unexpected Return = %d\n", CurrentReturn);
        printf("  Last Error = %d\n", GetLastError());

        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidEnumLoop
//
//  Checks the return code from the valid NLS api "Enum" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidEnumLoop(
    int CurrentReturn,
    int ExpectedReturn,
    int CurrentCtr,
    int ExpectedCtr,
    LPSTR pErrString,
    DWORD ItemValue,
    int *pNumErrors)
{
    if ( (CurrentReturn != ExpectedReturn) ||
         (CurrentCtr != ExpectedCtr) )
    {
        printf("ERROR: %s  %x - \n", pErrString, ItemValue);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);
        printf("  Counter = %d, Expected = %d\n", CurrentCtr, ExpectedCtr);

        (*pNumErrors)++;
    }

    if (Verbose)
    {
        printf("\n");
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidEnum
//
//  Checks the return code from the valid NLS api "Enum" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidEnum(
    int CurrentReturn,
    int ExpectedReturn,
    int CurrentCtr,
    int ExpectedCtr,
    LPSTR pErrString,
    int *pNumErrors)
{
    if ( (CurrentReturn != ExpectedReturn) ||
         (CurrentCtr != ExpectedCtr) )
    {
        printf("ERROR: %s - \n", pErrString);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);
        printf("  Counter = %d, Expected = %d\n", CurrentCtr, ExpectedCtr);

        (*pNumErrors)++;
    }

    if (Verbose)
    {
        printf("\n");
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidLoopW
//
//  Checks the return code from the valid NLS api "W" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidLoopW(
    int CurrentReturn,
    int ExpectedReturn,
    LPWSTR pCurrentString,
    LPWSTR pExpectedString,
    LPSTR pErrString,
    DWORD ItemValue,
    int *pNumErrors)
{
    if ((ExpectedReturn == -1) && (pExpectedString != NULL))
    {
        ExpectedReturn = WC_STRING_LEN_NULL(pExpectedString);
    }

    if ( (CurrentReturn != ExpectedReturn) ||
         ( (pCurrentString != NULL) &&
           (CompStringsW(pCurrentString, pExpectedString, CurrentReturn)) ) )
    {
        printf("ERROR: %s  %x - \n", pErrString, ItemValue);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);

        if (pCurrentString != NULL)
        {
            PrintWC(pCurrentString, CurrentReturn);
        }

        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidLoopA
//
//  Checks the return code from the valid NLS api "A" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidLoopA(
    int CurrentReturn,
    int ExpectedReturn,
    LPSTR pCurrentString,
    LPSTR pExpectedString,
    LPSTR pErrString,
    DWORD ItemValue,
    int *pNumErrors)
{
    if ((ExpectedReturn == -1) && (pExpectedString != NULL))
    {
        ExpectedReturn = MB_STRING_LEN_NULL(pExpectedString);
    }

    if ( (CurrentReturn != ExpectedReturn) ||
         ( (pCurrentString != NULL) &&
           (CompStringsA(pCurrentString, pExpectedString, CurrentReturn)) ) )
    {
        printf("ERROR: %s  %x - \n", pErrString, ItemValue);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);

        if (pCurrentString != NULL)
        {
            PrintMB(pCurrentString, CurrentReturn);
        }

        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidW
//
//  Checks the return code from the valid NLS api "W" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidW(
    int CurrentReturn,
    int ExpectedReturn,
    LPWSTR pCurrentString,
    LPWSTR pExpectedString,
    LPSTR pErrString,
    int *pNumErrors)
{
    if ((ExpectedReturn == -1) && (pExpectedString != NULL))
    {
        ExpectedReturn = WC_STRING_LEN_NULL(pExpectedString);
    }

    if ( (CurrentReturn != ExpectedReturn) ||
         ( (pCurrentString != NULL) &&
           (CompStringsW(pCurrentString, pExpectedString, CurrentReturn)) ) )
    {
        printf(">>>>ERROR: %s - \n", pErrString);
        printf("    Return = %d\n", CurrentReturn);
        printf("    Expected = %d\n", ExpectedReturn);

        if (pCurrentString != NULL)
        {
            PrintWC(pCurrentString, CurrentReturn);
        }

        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidA
//
//  Checks the return code from the valid NLS api "A" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidA(
    int CurrentReturn,
    int ExpectedReturn,
    LPSTR pCurrentString,
    LPSTR pExpectedString,
    LPBOOL pUsedDef,
    LPSTR pErrString,
    int *pNumErrors)
{
    if ((ExpectedReturn == -1) && (pExpectedString != NULL))
    {
        ExpectedReturn = MB_STRING_LEN_NULL(pExpectedString);
    }

    if ( (CurrentReturn != ExpectedReturn) ||
         ( (pCurrentString != NULL) &&
           (CompStringsA(pCurrentString, pExpectedString, CurrentReturn)) ) ||
         ( (pUsedDef != NULL) &&
           (*pUsedDef != TRUE) ) )
    {
        printf("ERROR: %s - \n", pErrString);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);

        if (pUsedDef != NULL)
        {
            printf("  UsedDef = %d\n", *pUsedDef);
        }

        if (pCurrentString != NULL)
        {
            PrintMB(pCurrentString, CurrentReturn);
        }

        (*pNumErrors)++;
    }
}


////////////////////////////////////////////////////////////////////////////
//
//  CheckReturnValidInt
//
//  Checks the return code from the valid NLS api "W" call.  It prints out
//  the appropriate error if the incorrect result is found.
//
//  06-14-91    JulieB    Created.
////////////////////////////////////////////////////////////////////////////

void CheckReturnValidInt(
    int CurrentReturn,
    int ExpectedReturn,
    DWORD CurrentInt,
    DWORD ExpectedInt,
    LPSTR pErrString,
    int *pNumErrors)
{
    if ( (CurrentReturn != ExpectedReturn) ||
         (CurrentInt != ExpectedInt) )
    {
        printf("ERROR: %s - \n", pErrString);
        printf("  Return = %d, Expected = %d\n", CurrentReturn, ExpectedReturn);
        printf("  Return Int = %d, Expected Int = %d\n", CurrentInt, ExpectedInt);

        (*pNumErrors)++;
    }
}
