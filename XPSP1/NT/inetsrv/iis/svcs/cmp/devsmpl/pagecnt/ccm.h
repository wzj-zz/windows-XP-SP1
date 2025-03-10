#ifndef __COUNTER__
#define __COUNTER__


// ccm.h : Declaration of CCM, the Central Management Object
#include "resource.h"       // main symbols
#include "page.h"

// Registry Keys
#define HITCNT_HKEY      HKEY_CLASSES_ROOT
#define HITCNT_KEYNAME   _T("MSWC.PageCounter")
#define HITCNT_FILELOCN  _T("File_Location")
#define HITCNT_DFLT_FILE _T("%windir%\\HitCnt.cnt")
#define HITCNT_SAVECNT   _T("Save_Count")
#define HITCNT_DFLT_SAVE 25

/////////////////////////////////////////////////////////////////////////////
// Central Counter Manager

class CCM
{
public:
    CCM();
    ~CCM();

    //---- Object Methods ----

    //Increments hit count for this page
    DWORD IncrementAndGetHits(const BSTR bstrURL);

    // Get the hit count for this page
    LONG GetHits(const BSTR bstrURL);

    // Reset the hit count for this page to zero
    void Reset(const BSTR bstrURL);

    //---- Helper Functions ----
    
    //Initializes Page Counter
    BOOL Initialize();

    //Writes the data to disk
    BOOL Persist();

private:
    //Looks in Registry to get the file path and name
    BOOL GetFileName();

    //Looks in Registry to get the Save Count
    BOOL GetSaveCount();

    //Loads the persisted data into an array
    BOOL LoadData();

    TCHAR       m_tszFileName[MAX_PATH]; //path and filename for persisting
    DWORD       m_dwSaveCount;           //threshold to trigger persisting
    UINT        m_cUnsavedHits;          //hits since we last persisted
    CPageArray  m_pages;                 //array of pages
    CComAutoCriticalSection m_critsec;   //Critical Section to protect data

};

extern CCM CountManager;


#endif
