//
// CCSHELL stock definition and declaration header
//


#ifndef __CCSTOCK_H__
#define __CCSTOCK_H__

#ifndef RC_INVOKED

// NT and Win95 environments set warnings differently.  This makes
// our project consistent across environments.

#pragma warning(3:4101)   // Unreferenced local variable

//
// Sugar-coating
//

#define PUBLIC
#define PRIVATE
#define IN
#define OUT
#define BLOCK

#ifndef DECLARE_STANDARD_TYPES

/*
 * For a type "FOO", define the standard derived types PFOO, CFOO, and PCFOO.
 */

#define DECLARE_STANDARD_TYPES(type)      typedef type *P##type; \
                                          typedef const type C##type; \
                                          typedef const type *PC##type;

#endif

#ifndef DECLARE_STANDARD_TYPES_U

/*
 * For a type "FOO", define the standard derived UNALIGNED types PFOO, CFOO, and PCFOO.
 *  WINNT: RISC boxes care about ALIGNED, intel does not.
 */

#define DECLARE_STANDARD_TYPES_U(type)    typedef UNALIGNED type *P##type; \
                                          typedef UNALIGNED const type C##type; \
                                          typedef UNALIGNED const type *PC##type;

#endif

// For string constants that are always wide
#define __TEXTW(x)    L##x
#define TEXTW(x)      __TEXTW(x)

//
// Count of characters to count of bytes
//
#define CbFromCchW(cch)             ((cch)*sizeof(WCHAR))
#define CbFromCchA(cch)             ((cch)*sizeof(CHAR))
#ifdef UNICODE
#define CbFromCch                   CbFromCchW
#else  // UNICODE
#define CbFromCch                   CbFromCchA
#endif // UNICODE

//
// General flag macros
//
#define SetFlag(obj, f)             do {obj |= (f);} while (0)
#define ToggleFlag(obj, f)          do {obj ^= (f);} while (0)
#define ClearFlag(obj, f)           do {obj &= ~(f);} while (0)
#define IsFlagSet(obj, f)           (BOOL)(((obj) & (f)) == (f))
#define IsFlagClear(obj, f)         (BOOL)(((obj) & (f)) != (f))

//
// String macros
//
#define IsSzEqual(sz1, sz2)         (BOOL)(lstrcmpi(sz1, sz2) == 0)
#define IsSzEqualC(sz1, sz2)        (BOOL)(lstrcmp(sz1, sz2) == 0)

#define lstrnicmpA(sz1, sz2, cch)           StrCmpNIA(sz1, sz2, cch)
#define lstrnicmpW(sz1, sz2, cch)           StrCmpNIW(sz1, sz2, cch)
#define lstrncmpA(sz1, sz2, cch)            StrCmpNA(sz1, sz2, cch)
#define lstrncmpW(sz1, sz2, cch)            StrCmpNW(sz1, sz2, cch)

//
// lstrcatnA and lstrcatnW are #defined here to StrCatBuff which is implemented
// in shlwapi. We do this here (and not in shlwapi.h or shlwapip.h) in case the
// kernel guys ever decided to implement this.
//
#define lstrcatnA(sz1, sz2, cchBuffSize)    StrCatBuffA(sz1, sz2, cchBuffSize)
#define lstrcatnW(sz1, sz2, cchBuffSize)    StrCatBuffW(sz1, sz2, cchBuffSize)
#ifdef UNICODE
#define lstrcatn lstrcatnW
#else
#define lstrcatn lstrcatnA
#endif // UNICODE

#ifdef UNICODE
#define lstrnicmp       lstrnicmpW
#define lstrncmp        lstrncmpW
#else
#define lstrnicmp       lstrnicmpA
#define lstrncmp        lstrncmpA
#endif

#ifndef SIZEOF
#define SIZEOF(a)                   sizeof(a)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)                (sizeof(a)/sizeof(a[0]))
#endif
#define SIZECHARS(sz)               (sizeof(sz)/sizeof(sz[0]))

#define InRange(id, idFirst, idLast)      ((UINT)((id)-(idFirst)) <= (UINT)((idLast)-(idFirst)))
#define IsInRange                   InRange

#define ZeroInit(pv, cb)            (memset((pv), 0, (cb)))

// ATOMICRELEASE
//
#ifndef ATOMICRELEASE
#ifdef __cplusplus
#define ATOMICRELEASET(p, type) { if(p) { type* punkT=p; p=NULL; punkT->Release();} }
#else
#define ATOMICRELEASET(p, type) { if(p) { type* punkT=p; p=NULL; punkT->lpVtbl->Release(punkT);} }
#endif

// doing this as a function instead of inline seems to be a size win.
//
#ifdef NOATOMICRELESEFUNC
#define ATOMICRELEASE(p) ATOMICRELEASET(p, IUnknown)
#else
#define ATOMICRELEASE(p) IUnknown_AtomicRelease((void **)&p)
#endif
#endif //ATOMICRELEASE

//
//  Helper macro for managing weak pointers to inner interfaces.
//  (It's the weak version of ATOMICRELEASE.)
//
//  The extra cast to (void **) is to keep C++ from doing strange
//  inheritance games when all I want to do is change the type.
//
#ifndef RELEASEINNERINTERFACE
#define RELEASEINNERINTERFACE(pOuter, p) \
        SHReleaseInnerInterface(pOuter, (IUnknown**)(void **)&(p))
#endif // RELEASEINNERINTERFACE

// For checking window charsets
#ifdef UNICODE
#define IsWindowTchar               IsWindowUnicode
#else  // !UNICODE
#define IsWindowTchar               !IsWindowUnicode
#endif // UNICODE

#ifdef DEBUG
// This macro is especially useful for cleaner looking code in
// declarations or for single lines.  For example, instead of:
//
//   {
//       DWORD dwRet;
//   #ifdef DEBUG
//       DWORD dwDebugOnlyVariable;
//   #endif
//
//       ....
//   }
//
// You can type:
//
//   {
//       DWORD dwRet;
//       DEBUG_CODE( DWORD dwDebugOnlyVariable; )
//
//       ....
//   }

#define DEBUG_CODE(x)               x
#else
#define DEBUG_CODE(x)

#endif  // DEBUG


//
// SAFECAST(obj, type)
//
// This macro is extremely useful for enforcing strong typechecking on other
// macros.  It generates no code.
//
// Simply insert this macro at the beginning of an expression list for
// each parameter that must be typechecked.  For example, for the
// definition of MYMAX(x, y), where x and y absolutely must be integers,
// use:
//
//   #define MYMAX(x, y)    (SAFECAST(x, int), SAFECAST(y, int), ((x) > (y) ? (x) : (y)))
//
//
#define SAFECAST(_obj, _type) (((_type)(_obj)==(_obj)?0:0), (_type)(_obj))


//
// Bitfields don't get along too well with bools,
// so here's an easy way to convert them:
//
#define BOOLIFY(expr)           (!!(expr))


// BUGBUG (scotth): we should probably make this a 'bool', but be careful
// because the Alpha compiler might not recognize it yet.  Talk to AndyP.

// This isn't a BOOL because BOOL is signed and the compiler produces 
// sloppy code when testing for a single bit.

typedef DWORD   BITBOOL;


// STOCKLIB util functions

// IsOS(): returns TRUE/FALSE if the platform is the indicated OS.

#ifndef OS_WINDOWS
#define OS_WINDOWS      0           // windows vs. NT
#define OS_NT           1           // windows vs. NT
#define OS_WIN95        2           // Win95 or greater
#define OS_NT4          3           // NT4 or greater
#define OS_NT5          4           // NT5 or greater
#define OS_MEMPHIS      5           // Win98 or greater
#define OS_MEMPHIS_GOLD 6           // Win98 Gold
#endif

STDAPI_(BOOL) staticIsOS(DWORD dwOS);

#include <pshpack2.h>
typedef struct tagDLGTEMPLATEEX
{
    WORD    wDlgVer;
    WORD    wSignature;
    DWORD   dwHelpID;
    DWORD   dwExStyle;
    DWORD   dwStyle;
    WORD    cDlgItems;
    short   x;
    short   y;
    short   cx;
    short   cy;
}   DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#include <poppack.h>

//
// round macro that rounds a to the next multiple of b.
//
#ifndef ROUNDUP
#define ROUNDUP(a,b)    ((((a)+(b)-1)/(b))*(b))
#endif

#define ROUND_TO_CLUSTER ROUNDUP

//
// macro that sees if a give char is an number
//
#define ISDIGIT(c)  ((c) >= TEXT('0') && (c) <= TEXT('9'))

//
// inline that does PathIsDotOrDotDot
//
__inline BOOL PathIsDotOrDotDotW(LPCWSTR pszPath)
{
    return ((pszPath[0] == L'.') && 
            ((pszPath[1] == L'\0') || ((pszPath[1] == L'.') && (pszPath[2] == L'\0'))));
}

__inline BOOL PathIsDotOrDotDotA(LPCSTR pszPath)
{
    return ((pszPath[0] == '.') && 
            ((pszPath[1] == '\0') || ((pszPath[1] == '.') && (pszPath[2] == '\0'))));
}

#ifdef UNICODE
#define PathIsDotOrDotDot PathIsDotOrDotDotW
#else
#define PathIsDotOrDotDot PathIsDotOrDotDotA
#endif


//
//  WindowLong accessor macros and other Win64 niceness
//

__inline void * GetWindowPtr(HWND hWnd, int nIndex) {
    return (void *)GetWindowLongPtr(hWnd, nIndex);
}

__inline void * SetWindowPtr(HWND hWnd, int nIndex, void * p) {
    return (void *)SetWindowLongPtr(hWnd, nIndex, (LONG_PTR)p);
}

//***   GetWindowLong0 -- 'fast' GetWindowLong (and GetWindowLongPtr)
// DESCRIPTION
//  what's up w/ this?  it's all about perf.  GetWindowLong has 'A' and 'W'
//  versions.  however 99% of the time they do the same thing (the other
//  0.1% has to do w/ setting the WndProc and having to go thru a thunk).
//  but we still need wrappers for the general case.  but most of the time
//  we're just doing a GWL(0), e.g. on entry to a wndproc to get our private
//  data.  so by having a special version of that, we save going thru the
//  wrapper (which was costing us 1-3% of our profile).
// NOTES
//  note that we call the 'A' version since that's guaranteed to exist on
// all platforms.
__inline LONG GetWindowLong0(HWND hWnd) {
    return GetWindowLongA(hWnd, 0);
}
__inline LONG SetWindowLong0(HWND hWnd, LONG l) {
    return SetWindowLongA(hWnd, 0, l);
}
__inline void * GetWindowPtr0(HWND hWnd) {
    return (void *)GetWindowLongPtrA(hWnd, 0);
}
__inline void * SetWindowPtr0(HWND hWnd, void * p) {
    return (void *)SetWindowLongPtrA(hWnd, 0, (LONG_PTR)p);
}


#define IS_WM_CONTEXTMENU_KEYBOARD(lParam) ((DWORD)(lParam) == 0xFFFFFFFF)

//
//  CharUpperChar - Convert a single character to uppercase
//
__inline WCHAR CharUpperCharW(WCHAR c)
{
    return (WCHAR)(DWORD_PTR)CharUpperW((LPWSTR)(DWORD_PTR)(c));
}

__inline CHAR CharUpperCharA(CHAR c)
{
    return (CHAR)(DWORD_PTR)CharUpperA((LPSTR)(DWORD_PTR)(c));
}

#ifdef UNICODE
#define CharUpperChar       CharUpperCharW
#else
#define CharUpperChar       CharUpperCharA
#endif

//
//  ShrinkProcessWorkingSet - Use this to stay Sundown-happy.
//
#define ShrinkWorkingSet() \
        SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T) -1)

//
//  COM Initialization.
//
//  Usage:
//
//      HRESULT hrInit = SHCoInitialize();
//      ... do COM stuff ...
//      SHCoUninitialize(hrInit);
//
//  Notice:  Continue doing COM stuff even if SHCoInitialize fails.
//  It might fail if somebody else already CoInit'd with different
//  flags, but we don't want to barf under those conditions.
//

STDAPI SHCoInitialize(void);
#define SHCoUninitialize(hr) if (SUCCEEDED(hr)) CoUninitialize()


//
//  OLE Initialization.
//
//  Usage:
//
//      HRESULT hrInit = SHOleInitialize(pMalloc);
//      ... do COM stuff ...
//      SHOleUninitialize(hrInit);
//

#define SHOleInitialize(pMalloc) OleInitialize(pMalloc)

#define SHOleUninitialize(hr)   if (SUCCEEDED(hr))  OleUninitialize()

//
//  MACRO HACK - Since not all clients of ccstock.h include shlobj.h first,
//  we need to use the long form of LP[C]ITEMIDLIST.  Since nobody remembers
//  this little quirk, to avoid future build breaks, redefine the symbols
//  to their long forms.
//
#define LPCITEMIDLIST const UNALIGNED struct _ITEMIDLIST *
#define  LPITEMIDLIST       UNALIGNED struct _ITEMIDLIST *

//
//  Name Parsing generic across the shell
//  
//  Usage:
// 
//      HRESULT SHGetNameAndFlags()
//          wrapper to bind to the folder and do a GetDisplayName()
//
STDAPI SHGetNameAndFlagsA(LPCITEMIDLIST pidl, DWORD dwFlags, LPSTR pszName, UINT cchName, DWORD *pdwAttribs);
STDAPI SHGetNameAndFlagsW(LPCITEMIDLIST pidl, DWORD dwFlags, LPWSTR pszName, UINT cchName, DWORD *pdwAttribs);
STDAPI SHGetNameAndFlags2A(struct IShellFolder *psfRoot, LPCITEMIDLIST pidl, DWORD dwFlags, LPSTR pszName, UINT cchName, DWORD *pdwAttribs);
STDAPI SHGetNameAndFlags2W(struct IShellFolder *psfRoot, LPCITEMIDLIST pidl, DWORD dwFlags, LPWSTR pszName, UINT cchName, DWORD *pdwAttribs);

//
//  Special values for SHGetNameAndFlags2::psfRoot
//
#define NAF2_SHELLDESKTOP   ((IShellFolder *)0)    // relative to shell desktop
#define NAF2_CURRENTROOT    ((IShellFolder *)-1)   // relative to current root

STDAPI SHBindToObject(struct IShellFolder *psf, REFIID riid, LPCITEMIDLIST pidl, void **ppvOut);
#define SHGetAttributesOf(pidl, prgfInOut) SHGetNameAndFlags(pidl, 0, NULL, 0, prgfInOut)

STDAPI_(DWORD) GetUrlSchemeW(LPCWSTR pszUrl);
STDAPI_(DWORD) GetUrlSchemeA(LPCSTR pszUrl);

#ifdef UNICODE
#define SHGetNameAndFlags       SHGetNameAndFlagsW
#define SHGetNameAndFlags2      SHGetNameAndFlags2W
#define GetUrlScheme            GetUrlSchemeW
#else
#define SHGetNameAndFlags       SHGetNameAndFlagsA
#define SHGetNameAndFlags2      SHGetNameAndFlags2A
#define GetUrlScheme            GetUrlSchemeA
#endif

// SHBindToIDListParent(LPCITEMIDLIST pidl, REFIID riid, void **ppv, LPCITEMIDLIST *ppidlLast)
//
// Given a pidl, you can get an interface pointer (as specified by riid) of the pidl's parent folder (in ppv)
// If ppidlLast is non-NULL, you can also get the pidl of the last item.
//
STDAPI SHBindToIDListParent(LPCITEMIDLIST pidl, REFIID riid, void **ppv, LPCITEMIDLIST *ppidlLast);

//
// SHBindToFolderIDListParent
//
//  Same as SHBindToIDListParent, except you also specify which root to use.
//
STDAPI SHBindToFolderIDListParent(struct IShellFolder *psfRoot, LPCITEMIDLIST pidl, REFIID riid, void **ppv, LPCITEMIDLIST *ppidlLast);

STDAPI_(void) SHRemoveURLTurd(LPTSTR pszUrl);

//  clones the parent of the pidl
STDAPI_(LPITEMIDLIST) ILCloneParent(LPCITEMIDLIST pidl);

//
//  END OF MACRO HACK
//
#undef LPITEMIDLIST
#undef LPCITEMIDLIST

//
// Mirroring-Support APIs (astracted in \shell\lib\stock5\rtlmir.cpp)
//
#ifdef __cplusplus
extern "C" {
#endif

extern BOOL g_bMirroredOS;

WORD  GetDefaultLang(BOOL bForceEnglish);
WORD  GetWindowLang (HWND hWndOwner);
BOOL  UseProperDlgTemplate(HINSTANCE hInst, HGLOBAL *phDlgTemplate, HRSRC *phResInfo, LPCSTR lpName, 
                           HWND hWndOwner, LPWORD lpwLangID, BOOL bForceEnglish);

#ifdef USE_MIRRORING

BOOL  IsBiDiLocalizedSystem( void );
BOOL  Mirror_IsEnabledOS( void );
LANGID Mirror_GetUserDefaultUILanguage( void );
BOOL  Mirror_IsWindowMirroredRTL( HWND hWnd );
DWORD Mirror_IsDCMirroredRTL( HDC hdc );
DWORD Mirror_MirrorDC( HDC hdc );
BOOL  Mirror_MirrorProcessRTL( void );
DWORD Mirror_GetLayout( HDC hdc );
DWORD Mirror_SetLayout( HDC hdc , DWORD dwLayout );
BOOL Mirror_GetProcessDefaultLayout( DWORD *pdwDefaultLayout );
BOOL Mirror_IsProcessRTL( void );
extern const DWORD dwNoMirrorBitmap;
extern const DWORD dwExStyleRTLMirrorWnd;
extern const DWORD dwExStyleNoInheritLayout;
extern const DWORD dwPreserveBitmap;
//
// 'g_bMirroredOS' is defined in each component which will use the
//  mirroring APIs. I decided to put it here, in order to make sure
//  each component has validated that the OS supports the mirroring
//  APIs before calling them.
//

#define IS_BIDI_LOCALIZED_SYSTEM()      IsBiDiLocalizedSystem()
#define IS_MIRRORING_ENABLED()          Mirror_IsEnabledOS()
#define IS_WINDOW_RTL_MIRRORED(hwnd)    (g_bMirroredOS && Mirror_IsWindowMirroredRTL(hwnd))
#define IS_DC_RTL_MIRRORED(hdc)         (g_bMirroredOS && Mirror_IsDCMirroredRTL(hdc))
#define GET_PROCESS_DEF_LAYOUT(pdwl)    (g_bMirroredOS && Mirror_GetProcessDefaultLayout(pdwl))
#define IS_PROCESS_RTL_MIRRORED()       (g_bMirroredOS && Mirror_IsProcessRTL())
#define SET_DC_RTL_MIRRORED(hdc)        Mirror_MirrorDC(hdc)
#define SET_DC_LAYOUT(hdc,dwl)          Mirror_SetLayout(hdc,dwl)
#define SET_PROCESS_RTL_LAYOUT()        Mirror_MirrorProcessRTL()
#define GET_DC_LAYOUT(hdc)              Mirror_GetLayout(hdc) 
#define DONTMIRRORBITMAP                dwNoMirrorBitmap
#define RTL_MIRRORED_WINDOW             dwExStyleRTLMirrorWnd
#define RTL_NOINHERITLAYOUT             dwExStyleNoInheritLayout
#define LAYOUT_PRESERVEBITMAP           dwPreserveBitmap

#else

#define IS_BIDI_LOCALIZED_SYSTEM()      FALSE
#define IS_MIRRORING_ENABLED()          FALSE
#define IS_WINDOW_RTL_MIRRORED(hwnd)    FALSE
#define IS_DC_RTL_MIRRORED(hdc)         FALSE
#define GET_PROCESS_DEF_LAYOUT(pdwl)    FALSE
#define IS_PROCESS_RTL_MIRRORED()       FALSE
#define SET_DC_RTL_MIRRORED(hdc)        
#define SET_DC_LAYOUT(hdc,dwl)
#define SET_PROCESS_DEFAULT_LAYOUT() 
#define GET_DC_LAYOUT(hdc)              0L

#define DONTMIRRORBITMAP                0L
#define RTL_MIRRORED_WINDOW             0L
#define LAYOUT_PRESERVEBITMAP           0L

#endif  // USE_MIRRROING

BOOL IsBiDiLocalizedWin95( BOOL bArabicOnly );


//
//====== Dynamic array functions  ================================================
//

//------------------------------------------------------------------------
// Dynamic key array
//
typedef struct _DKA * HDKA;     // hdka

HDKA   DKA_CreateA(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszFirst, LPCSTR pszDefOrder, BOOL fDefault);
HDKA   DKA_CreateW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszFirst, LPCWSTR pszDefOrder, BOOL fDefault);

int    DKA_GetItemCount(HDKA hdka);

LPCSTR  DKA_GetKeyA(HDKA hdka, int iItem);
LPCWSTR DKA_GetKeyW(HDKA hdka, int iItem);

LONG   DKA_QueryValueA(HDKA hdka, int iItem, LPSTR szValue, LONG  * pcb);
LONG   DKA_QueryValueW(HDKA hdka, int iItem, LPWSTR szValue, LONG  * pcb);

DWORD  DKA_QueryOtherValueA(HDKA pdka, int iItem, LPCSTR pszName, LPSTR pszValue, LONG * pcb);
DWORD  DKA_QueryOtherValueW(HDKA pdka, int iItem, LPCWSTR pszName, LPWSTR pszValue, LONG * pcb);

void   DKA_Destroy(HDKA hdka);

#ifdef UNICODE
#define DKA_Create          DKA_CreateW
#define DKA_GetKey          DKA_GetKeyW
#define DKA_QueryValue      DKA_QueryValueW
#define DKA_QueryOtherValue DKA_QueryOtherValueW
#else
#define DKA_Create          DKA_CreateA
#define DKA_GetKey          DKA_GetKeyA
#define DKA_QueryValue      DKA_QueryValueA
#define DKA_QueryOtherValue DKA_QueryOtherValueA
#endif

//------------------------------------------------------------------------
// Dynamic class array
//
typedef struct _DCA * HDCA;     // hdca

HDCA DCA_Create();
void DCA_Destroy(HDCA hdca);
int  DCA_GetItemCount(HDCA hdca);
BOOL DCA_AddItem(HDCA hdca, REFCLSID rclsid);
const CLSID * DCA_GetItem(HDCA hdca, int i);

void DCA_AddItemsFromKeyA(HDCA hdca, HKEY hkey, LPCSTR pszSubKey);
void DCA_AddItemsFromKeyW(HDCA hdca, HKEY hkey, LPCWSTR pszSubKey);

#ifdef UNICODE
#define DCA_AddItemsFromKey     DCA_AddItemsFromKeyW
#else
#define DCA_AddItemsFromKey     DCA_AddItemsFromKeyA
#endif 

STDAPI DCA_CreateInstance(HDCA hdca, int iItem, REFIID riid, void ** ppv);


#ifdef __cplusplus
};
#endif

#endif // RC_INVOKED

//------------------------------------------------------------------------
// Random helpful functions
//------------------------------------------------------------------------
//
STDAPI_(BOOL) _SHIsButtonObscured(HWND hwnd, PRECT prc, INT_PTR i);
STDAPI_(void) _SHPrettyMenu(HMENU hm);
STDAPI_(BOOL) _SHIsMenuSeparator(HMENU hm, int i);
STDAPI_(BOOL) _SHIsMenuSeparator2(HMENU hm, int i, BOOL *pbIsNamed);
STDAPI_(BYTE) SHBtnStateFromRestriction(DWORD dwRest, BYTE fsState);
STDAPI_(BOOL) SHIsDisplayable(LPCWSTR pwszName, BOOL fRunOnFE, BOOL fRunOnNT5);

STDAPI_(void) EnableOKButtonFromString(HWND hDlg, LPTSTR pszText);
STDAPI_(void) EnableOKButtonFromID(HWND hDlg, int id);

//------------------------------------------------------------------------

////////////////
//
//  Critical section stuff
//
//  Helper macros that give nice debug support
//
EXTERN_C CRITICAL_SECTION g_csDll;
#ifdef DEBUG
EXTERN_C UINT g_CriticalSectionCount;
EXTERN_C DWORD g_CriticalSectionOwner;
EXTERN_C void Dll_EnterCriticalSection(CRITICAL_SECTION*);
EXTERN_C void Dll_LeaveCriticalSection(CRITICAL_SECTION*);
#if defined(__cplusplus) && defined(AssertMsg)
class DEBUGCRITICAL {
protected:
    BOOL fClosed;
public:
    DEBUGCRITICAL() {fClosed = FALSE;};
    void Leave() {fClosed = TRUE;};
    ~DEBUGCRITICAL() 
    {
        AssertMsg(fClosed, TEXT("you left scope while holding the critical section"));
    }
};
#define ENTERCRITICAL DEBUGCRITICAL debug_crit; Dll_EnterCriticalSection(&g_csDll)
#define LEAVECRITICAL debug_crit.Leave(); Dll_LeaveCriticalSection(&g_csDll)
#define ENTERCRITICALNOASSERT Dll_EnterCriticalSection(&g_csDll)
#define LEAVECRITICALNOASSERT Dll_LeaveCriticalSection(&g_csDll)
#else // __cplusplus
#define ENTERCRITICAL Dll_EnterCriticalSection(&g_csDll)
#define LEAVECRITICAL Dll_LeaveCriticalSection(&g_csDll)
#define ENTERCRITICALNOASSERT Dll_EnterCriticalSection(&g_csDll)
#define LEAVECRITICALNOASSERT Dll_LeaveCriticalSection(&g_csDll)
#endif // __cplusplus
#define ASSERTCRITICAL ASSERT(g_CriticalSectionCount > 0 && GetCurrentThreadId() == g_CriticalSectionOwner)
#define ASSERTNONCRITICAL ASSERT(GetCurrentThreadId() != g_CriticalSectionOwner)
#else // DEBUG
#define ENTERCRITICAL EnterCriticalSection(&g_csDll)
#define LEAVECRITICAL LeaveCriticalSection(&g_csDll)
#define ENTERCRITICALNOASSERT EnterCriticalSection(&g_csDll)
#define LEAVECRITICALNOASSERT LeaveCriticalSection(&g_csDll)
#define ASSERTCRITICAL 
#define ASSERTNONCRITICAL
#endif // DEBUG

#endif // __CCSTOCK_H__
