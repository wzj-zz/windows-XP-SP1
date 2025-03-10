/*
generate comctl tool
based on gennt32t
*/
#pragma warning( disable : 4786) //disable identifier is too long for debugging error
#pragma warning( disable : 4503) //disable decorated name is too long
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <imagehlp.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <set>
#include <map>

extern "C" {

#include "gen.h"

#if !defined(NUMBER_OF)
#define NUMBER_OF(x) (sizeof(x)/sizeof((x)[0]))
#endif

// string to put in front of all error messages so that BUILD can find them.
const char *ErrMsgPrefix = "NMAKE :  U8603: 'GENCOMCTLT' ";

void
HandlePreprocessorDirective(
   char *p
   )
{
   ExitErrMsg(FALSE, "Preprocessor directives not allowed by gencomctlt.\n");
}

}

using namespace std;
typedef string String;

PRBTREE pFunctions = NULL;
PRBTREE pStructures = NULL;
PRBTREE pTypedefs = NULL;

void ExtractCVMHeader(PCVMHEAPHEADER pHeader) {
   pFunctions = &pHeader->FuncsList;
   pTypedefs =  &pHeader->TypeDefsList;
   pStructures =&pHeader->StructsList;
}

// globals so debugging works
PKNOWNTYPES pFunction; 
PFUNCINFO   pfuncinfo;

#if 0

#define NOTHING /* */
#define IGNORED /* */
#define NOPREFIX /* */
#define NOSUFFIX /* */

//HANDLE __stdcall MyCreateActCtx(void)
//{
//    static HANDLE Handle = INVALID_HANDLE_VALUE;

//    if (Handle == INVALID_HANDLE_VALUE)
//    {
//        WCHAR PathToManifest[MAX_PATH];
//        ACTCTXW ActCtx = {sizeof(ActCtx)};

//        SearchPathW(NULL, L"shell32.dll", NULL, NUMBER_OF(PathToManifest), PathToManifest, NULL);
//        ActCtx.lpSource = PathToManifest;

//        Handle = CreateActCtxW(&ActCtx);

//        if (Handle == INVALID_HANDLE_VALUE)
//            goto Exit;
//    }
//Exit:
//    return Handle;
//}

//BOOL MyLoadLibraryOrGetModuleHandle(PCWSTR name, HMODULE* Module)
//{
//    static HMODULE StaticModule = NULL;
//    HANDLE ActivationContext;
//    BOOL Success = FALSE;
//    ULONG Cookie = 0;
//    DWORD Error = NO_ERROR;

//    *Module = NULL;
//    if (StaticModule != NULL)
//    {
//        *Module = StaticModule;
//        Success = TRUE;
//        goto Exit;
//    }

//    ActivationContext = MyCreateActCtx();
//    if (ActivationContext == INVALID_HANDLE_VALUE)
//        goto Exit;
//    ActivateActCtx(ActivationContext, &Cookie);

//    *Module = LoadLibraryW(L"comctl32.dll");
//    if (*Module == NULL)
//        goto Exit;

//    Success = TRUE;
//Exit:
//    if (!Success && Error == NO_ERROR)
//        Error = GetLastError();
//    if (Cookie != 0)
//    {
//        if (!DeactivateActCtx(Cookie))
//        { // ?
//            if (Success)
//            {
//                Success = FALSE;
//                Error = GetLastError();
//            }
//        }
//    }
//    if (!Success)
//        SetLastError(Error);
//    return Success;
//}

//BOOL Patch(PCSTR ProcName, FARPROC* ProcAddress)
//{
//    HMODULE Module = NULL;
//    BOOL Success = FALSE;

//    *ProcAddress = NULL;
//     Success = MyLoadLibraryOrGetModuleHandle(L"comctl32.dll", &Module);
//    if (!Success)
//        goto Exit;
//    *ProcAddress = GetProcAddress(Module, ProcName);
//    if (*ProcAddress == NULL)
//        goto Exit;
//    Success = TRUE;
//Exit:
//    return Success;
//}

//#define DECLARE(ret, call, name, argNamesTypes, argNames, importType, importExtra, onError) \
//    ret (call* name) argNamesTypes ;

//#define DEFINE(ret, call, name, argNamesTypes, argNames, importType, importExtra, onError) \
//static ret call FIRST(name) argNamesTypes \
//{ \
//    static DWORD Error = NO_ERROR; \
//    if (Error != NO_ERROR) \
//    { \
//        onError(Error); \
//    } \
//    if (!Patch(importType(importExtra), (FARPROC*)&name)) \
//    { \
//        Error = GetLastError(); \
//        onError(Error); \
//    } \
//    return name argNames; \
//} \
//ret (call* name) argNamesTypes  = FIRST(name);

//#define IMPORT_BY_NAME(name, extra)    (#name)
//#define IMPORT_BY_ORDINAL(name, extra) ((const char*)(ULONG_PTR)extra)

//#define COMCTL_FUNCTIONS \
//COMCTL_FUNCTION(HRESULT, __stdcall, UninitializeFlatSB, (HWND _noname0), (_noname0), IMPORT_BY_NAME, IGNORED) \
//COMCTL_FUNCTION(BOOL, __stdcall, InitializeFlatSB, (HWND _noname0), (_noname0), IMPORT_BY_ORDINAL, 1)

//#define COMCTL_FUNCTION(ret, call, name, argNamesTypes, argNames, importType, importExtra, onError) \
//    DECLARE(ret, call, name, argNamesTypes, argNames, importType, importExtra, onError)

//COMCTL_FUNCTIONS


// HWND _noname0,
//)

DumpLib(PCWSTR filename)
{
    CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL
}

#endif

void DumpFunctionDeclarationsHeader(void)
{
    //PKNOWNTYPES pFunction; 
    //PFUNCINFO   pfuncinfo;

    cout << "///////////////////////////////////////////\n";
    cout << "// This file is autogenerated by gencomctlt. \n";
    cout << "// Do not edit                             \n";
    cout << "///////////////////////////////////////////\n";
    cout << '\n' << '\n';

    cout << "#include \"windows.h\"\n";
    cout << "#include \"commctrl.h\"\n\n";

    cout << "///////////////////////////////////////////\n";
    cout << "//  Functions                            //\n";
    cout << "///////////////////////////////////////////\n";
    for (
        pFunction = pFunctions->pLastNodeInserted;
        pFunction != NULL
            && pFunction->TypeName != NULL
            && strcmp(pFunction->TypeName, "MarkerFunction_8afccfaa_27e7_45d5_8ff7_7ac0b970789d") != 0 ;
        pFunction = pFunction->Next)
    {
    /*
    for now, just like print out commctrl as a demo/test of understanding the tool
    tomorrow, print out what we actually need
    */
#if 1
        cout << pFunction->FuncRet << ' ';
        cout << pFunction->FuncMod << ' '; // __stdcall
        cout << pFunction->TypeName << "(\n"; // function name
        pfuncinfo = pFunction->pfuncinfo;
        if (pfuncinfo == NULL || pfuncinfo->sType == NULL || pfuncinfo->sName == NULL)
        {
            cout << "void";
        }
        else
        {
            for ( ; pfuncinfo != NULL ; pfuncinfo = pfuncinfo->pfuncinfoNext )
            {
                cout << ' ' << pfuncinfo->sType << ' ' << pfuncinfo->sName << ",\n";
            }
        }
        cout << ")\n";
#endif
    }
    cout << '\n' << '\n';
}

int __cdecl main(int argc, char*argv[])
{
    ExtractCVMHeader(MapPpmFile(argv[1], TRUE));
    DumpFunctionDeclarationsHeader();
    return 0;
}
