//=======================================================================
//
//  Copyright (c) 1998-2000 Microsoft Corporation.  All Rights Reserved.
//
//  File:   iuxml.h
//
//  Description:
//
//      Declaration of the CIUXml class
//
//=======================================================================

#ifndef __IUXML_H_
#define __IUXML_H_

#include <msxml.h>

#define HANDLE_NODE_INVALID     -1
#define HANDLE_NODELIST_INVALID -1

typedef DWORD   HANDLE_NODE;
typedef DWORD   HANDLE_NODELIST;

// Bitmap of existence of all possible system info classes
extern const DWORD  COMPUTERSYSTEM;
extern const DWORD  REGKEYS;
extern const DWORD  PLATFORM;
extern const DWORD  LOCALE;
extern const DWORD  DEVICES;


/////////////////////////////////////////////////////////////////////////////
// CIUXml
class CIUXml
{
public:
    CIUXml();
    virtual ~CIUXml();

    /////////////////////////////////////////////////////////////////////////////
    // SafeCloseHandleNode()
    //
    // User can explicitly can this function to release a node for reuse when 
    // writing a xml doc.
    /////////////////////////////////////////////////////////////////////////////
    void SafeCloseHandleNode(HANDLE_NODE& hNode);

    /////////////////////////////////////////////////////////////////////////////
    // SafeFindCloseHandle()
    //
    // User can explicitly can this function to release a nodelist for reuse when 
    // reading a xml doc.
    /////////////////////////////////////////////////////////////////////////////
    void SafeFindCloseHandle(HANDLE_NODELIST& hNodeList);

    /////////////////////////////////////////////////////////////////////////////
    // GetDOMNodebyHandle()
    //
    // Retrieve the xml node with the given index of m_ppNodeArray
    /////////////////////////////////////////////////////////////////////////////
    IXMLDOMNode* GetDOMNodebyHandle(HANDLE_NODE hNode);

private:
    /////////////////////////////////////////////////////////////////////////////
    // Handle to the heap of the calling process, used for HeapAlloc()
    /////////////////////////////////////////////////////////////////////////////
    HANDLE  m_hHeap;

    /////////////////////////////////////////////////////////////////////////////
    // Length of the node array "m_ppNodeArray"
    /////////////////////////////////////////////////////////////////////////////
    DWORD   m_dwSizeNodeArray;

    /////////////////////////////////////////////////////////////////////////////
    // Length of the nodelist array "m_ppNodeListArray"
    /////////////////////////////////////////////////////////////////////////////
    DWORD   m_dwSizeNodeListArray;

    /////////////////////////////////////////////////////////////////////////////
    // InitNodeArray()
    //
    // Allocate or re-allocate memory for the node array "m_ppNodeArray"
    /////////////////////////////////////////////////////////////////////////////
    HRESULT InitNodeArray(BOOL fRealloc = FALSE);

    /////////////////////////////////////////////////////////////////////////////
    // InitNodeListArray()
    //
    // Allocate or re-allocate memory for the nodelist array "m_ppNodeListArray"
    /////////////////////////////////////////////////////////////////////////////
    HRESULT InitNodeListArray(BOOL fRealloc = FALSE);

    /////////////////////////////////////////////////////////////////////////////
    // GetNodeHandle()
    //
    // Look for the first un-used node from the "m_ppNodeArray" array,
    // including the memory allocation, if needed.
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODE GetNodeHandle();

    /////////////////////////////////////////////////////////////////////////////
    // GetNodeListHandle()
    //
    // Look for the first un-used nodelist from the "m_ppNodeListArray" array,
    // including the memory allocation, if needed.
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST GetNodeListHandle();

protected:
    /////////////////////////////////////////////////////////////////////////////
    // This array is used to keep track of different nodes that can remain
    // opened at the same time to write into; the index of the array will be
    // returned to caller as a HANDLE_NODE to perform writing against.
    /////////////////////////////////////////////////////////////////////////////
    IXMLDOMNode**           m_ppNodeArray;

    /////////////////////////////////////////////////////////////////////////////
    // This array is used to keep track of different nodelists that can remain
    // opened at the same time to read from; the index of the array will be
    // returned to caller as a HANDLE_NODELIST to perform reading against.
    /////////////////////////////////////////////////////////////////////////////
    IXMLDOMNodeList**       m_ppNodeListArray;

    /////////////////////////////////////////////////////////////////////////////
    // FindFirstDOMNode()
    //
    // Retrieve the first xml node with the given tag name under the given parent node
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST FindFirstDOMNode(IXMLDOMNode* pParentNode, BSTR bstrName, IXMLDOMNode** ppNode);

    /////////////////////////////////////////////////////////////////////////////
    // FindFirstDOMNode()
    //
    // Retrieve the handle of first xml node with the given tag name under the given parent node
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST FindFirstDOMNode(IXMLDOMNode* pParentNode, BSTR bstrName, HANDLE_NODE* phNode);

	/////////////////////////////////////////////////////////////////////////////
	// FindFirstDOMNode()
	//
	// Retrieve the first xml node with the given tag name in the given xml doc
	/////////////////////////////////////////////////////////////////////////////
	HANDLE_NODELIST FindFirstDOMNode(IXMLDOMDocument* pDoc, BSTR bstrName, IXMLDOMNode** ppNode);

	/////////////////////////////////////////////////////////////////////////////
	// FindFirstDOMNode()
	//
	// Retrieve the handle of first xml node with the given tag name in the given xml doc
	/////////////////////////////////////////////////////////////////////////////
	HANDLE_NODELIST FindFirstDOMNode(IXMLDOMDocument* pDoc, BSTR bstrName, HANDLE_NODE* phNode);

    /////////////////////////////////////////////////////////////////////////////
    // FindNextDOMNode()
    //
    // Retrieve the next xml node with the given tag name under the given parent node
    /////////////////////////////////////////////////////////////////////////////
    HRESULT FindNextDOMNode(HANDLE_NODELIST hNodeList, IXMLDOMNode** ppNode);

    /////////////////////////////////////////////////////////////////////////////
    // FindNextDOMNode()
    //
    // Retrieve the handle of next xml node with the given tag name under the given parent node
    /////////////////////////////////////////////////////////////////////////////
    HRESULT FindNextDOMNode(HANDLE_NODELIST hNodeList, HANDLE_NODE* phNode);

    /////////////////////////////////////////////////////////////////////////////
    // CreateDOMNodeWithHandle()
    //
    // Create an xml node of the given type
    // Return: index of the node array "m_ppNodeArray"; or -1 if failure.
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODE CreateDOMNodeWithHandle(IXMLDOMDocument* pDoc, SHORT nType, BSTR bstrName, BSTR bstrNamespaceURI = NULL);
};


/////////////////////////////////////////////////////////////////////////////
// CXmlSystemSpec
class CXmlSystemSpec : public CIUXml
{
public:
    /////////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    // Create IXMLDOMDocument* for SystemSpec
    /////////////////////////////////////////////////////////////////////////////
    CXmlSystemSpec();

    /////////////////////////////////////////////////////////////////////////////
    // Destructor
    //
    // Release IXMLDOMDocument* for SystemSpec
    /////////////////////////////////////////////////////////////////////////////
    ~CXmlSystemSpec();

    /////////////////////////////////////////////////////////////////////////////
    // AddComputerSystem()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddComputerSystem(BSTR bstrManufacturer,
                              BSTR bstrModel,
                              BSTR bstrSupportSite = NULL,
                              INT  iAdmin = -1,
                              INT  iWUDisabled = -1,
                              INT  iAUEnabled = -1,
							  BSTR bstrPID=NULL);

    /////////////////////////////////////////////////////////////////////////////
    // AddDriveSpace()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddDriveSpace(BSTR bstrDrive, INT iKBytes);

    /////////////////////////////////////////////////////////////////////////////
    // AddReg()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddReg(BSTR bstrProvider);

    /////////////////////////////////////////////////////////////////////////////
    // AddPlatform()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddPlatform(BSTR bstrName);

    /////////////////////////////////////////////////////////////////////////////
    // AddProcessor()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddProcessor(BSTR bstrProcessor);

    /////////////////////////////////////////////////////////////////////////////
    // AddVersion()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddVersion(INT  iMajor = -1,
                       INT  iMinor = -1,
                       INT  iBuild = -1,
                       INT  iSPMajor = -1,
                       INT  iSPMinor = -1,
                       BSTR bstrTimeStamp = NULL);

    /////////////////////////////////////////////////////////////////////////////
    // AddSuite()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddSuite(BSTR bstrSuite);

    /////////////////////////////////////////////////////////////////////////////
    // AddProductType()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddProductType(BSTR bstrProductType);

    /////////////////////////////////////////////////////////////////////////////
    // AddLocale()
    //
    // We need to pass back a handle to differentiate different <locale> node
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddLocale(BSTR bstrContext, HANDLE_NODE* phNodeLocale);

    /////////////////////////////////////////////////////////////////////////////
    // AddLanguage()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddLanguage(HANDLE_NODE hNodeLocale, BSTR bstrLocale);

    /////////////////////////////////////////////////////////////////////////////
    // AddDevice()
    //
    // We need to pass back a handle to differentiate different <device> node
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddDevice(BSTR bstrDeviceInstance,
                      INT  iIsPrinter,
                      BSTR bstrProvider,
                      BSTR bstrMfgName,
                      BSTR bstrDriverName,
                      HANDLE_NODE* phNodeDevice);

    /////////////////////////////////////////////////////////////////////////////
    // AddHWID()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddHWID(HANDLE_NODE hNodeDevice,
                    BOOL fIsCompatible,
                    UINT iRank,
                    BSTR bstrHWID,
                    BSTR bstrDriverVer = NULL);

    /////////////////////////////////////////////////////////////////////////////
    // GetSystemSpecBSTR()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetSystemSpecBSTR(BSTR *pbstrXmlSystemSpec);

private:
    IXMLDOMDocument*    m_pDocSystemSpec;
    IXMLDOMNode*        m_pNodeSystemInfo;
    IXMLDOMNode*        m_pNodeComputerSystem;
    IXMLDOMNode*        m_pNodeRegKeysSW;
    IXMLDOMNode*        m_pNodePlatform;
    IXMLDOMNode*        m_pNodeDevices;
};


/////////////////////////////////////////////////////////////////////////////
// CXmlSystemClass
class CXmlSystemClass : public CIUXml
{
public:
    /////////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    // Create IXMLDOMDocument* for SystemInfoClasses
    /////////////////////////////////////////////////////////////////////////////
    CXmlSystemClass();

    /////////////////////////////////////////////////////////////////////////////
    // Destructor
    //
    // Release IXMLDOMDocument* for SystemInfoClasses
    /////////////////////////////////////////////////////////////////////////////
    ~CXmlSystemClass();

    /////////////////////////////////////////////////////////////////////////////
    // LoadXMLDocument()
    //
    // Load an XML Document from string
    /////////////////////////////////////////////////////////////////////////////
    HRESULT LoadXMLDocument(BSTR bstrXml, BOOL fOfflineMode);

    /////////////////////////////////////////////////////////////////////////////
    // GetClasses()
    //
    // Return the bitmap of existence of all possible system info classes
    /////////////////////////////////////////////////////////////////////////////
    DWORD GetClasses();

private:
    IXMLDOMDocument*    m_pDocSystemClass;
};


/////////////////////////////////////////////////////////////////////////////
// CXmlCatalog
class CXmlCatalog : public CIUXml
{
public:
    /////////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    // Create IXMLDOMDocument* for Catalog
    /////////////////////////////////////////////////////////////////////////////
    CXmlCatalog();

    /////////////////////////////////////////////////////////////////////////////
    // Destructor
    //
    // Release IXMLDOMDocument* for Catalog
    /////////////////////////////////////////////////////////////////////////////
    ~CXmlCatalog();

    /////////////////////////////////////////////////////////////////////////////
    // LoadXMLDocument()
    //
    // Load an XML Document from string
    /////////////////////////////////////////////////////////////////////////////
    HRESULT LoadXMLDocument(BSTR bstrXml, BOOL fOfflineMode);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemCount()
    //
    // Gets a Count of How Many Items are in this Catalog
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemCount(LONG *plItemCount);

    /////////////////////////////////////////////////////////////////////////////
    // GetProviders()
    // 
    // Find a list of <provider> node in catalog xml
    /////////////////////////////////////////////////////////////////////////////
    IXMLDOMNodeList* GetProviders();

    /////////////////////////////////////////////////////////////////////////////
    // GetFirstProvider()
    //
    // Find the first provider in catalog xml doc
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST GetFirstProvider(HANDLE_NODE* phNodeProvider);
        
    /////////////////////////////////////////////////////////////////////////////
    // GetNextProvider()
    //
    // Find the next provider in catalog xml doc
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetNextProvider(HANDLE_NODELIST hNodeListProvider, HANDLE_NODE* phNodeProvider);    

    /////////////////////////////////////////////////////////////////////////////
    // GetFirstItem()
    //
    // Find the first item in provider (parent) node
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST GetFirstItem(HANDLE_NODE hNodeProvider, HANDLE_NODE* phNodeItem);
        
    /////////////////////////////////////////////////////////////////////////////
    // GetNextItem()
    //
    // Find the next item in provider (parent) node
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetNextItem(HANDLE_NODELIST hNodeListItem, HANDLE_NODE* phNodeItem);    
        
    /////////////////////////////////////////////////////////////////////////////
    // GetFirstItemDependency()
    //
    // Find the first dependency item in Item Dependencies node
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST GetFirstItemDependency(HANDLE_NODE hNodeItem, HANDLE_NODE* phNodeItem);

    /////////////////////////////////////////////////////////////////////////////
    // GetNextItem()
    //
    // Find the next dependency item in the Item Dependencies node
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetNextItemDependency(HANDLE_NODELIST hNodeListItem, HANDLE_NODE* phNodeItem);  

    /////////////////////////////////////////////////////////////////////////////
    // CloseItemList()
    //
    // Release the item nodelist
    /////////////////////////////////////////////////////////////////////////////
    void CloseItemList(HANDLE_NODELIST hNodeListItem);

    /////////////////////////////////////////////////////////////////////////////
    // GetIdentity()
    //
    // Retrieve the unique name (identity) of the given provider or item
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetIdentity(HANDLE_NODE hNode,
                        BSTR* pbstrName,
                        BSTR* pbstrPublisherName,
                        BSTR* pbstrGUID);

    /////////////////////////////////////////////////////////////////////////////
    // GetIdentityStr()
    //
    // Retrieve the string that can be used to uniquely identify an item.
    // This function defines the logic about what components can be used
    // to define the uniqueness of an item based on the 3 parts of data from
    // GetIdentity().
    //
    // The created string will be language neutral. That is, it can not
    // ensure the uniqueness for two items having the same <identity> node
    // except different only on <langauge> part inside <identity>
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetIdentityStr(HANDLE_NODE hNode,
                        BSTR* pbstrUniqIdentifierString);

	HRESULT GetIdentityStrForPing(HANDLE_NODE hNode,
                        BSTR* pbstrUniqIdentifierString);

    /////////////////////////////////////////////////////////////////////////////
    // GetBSTRItemForCallback()
    //
    // Create an item node as the passed-in node, have child nodes identity and
	// platform (anything uniquely idenitify this item) then output this 
	// item node data as string, then delete the crated node
    //
    /////////////////////////////////////////////////////////////////////////////
	HRESULT GetBSTRItemForCallback(HANDLE_NODE hItem, BSTR* pbstrXmlItemForCallback);

    /////////////////////////////////////////////////////////////////////////////
    // IsPrinterDriver()
    //
    // Retrieves from the Catalog whether this Item is a Printer Driver
    //
    /////////////////////////////////////////////////////////////////////////////
    BOOL IsPrinterDriver(HANDLE_NODE hNode);

    /////////////////////////////////////////////////////////////////////////////
    // GetDriverInfo()
    //
    // Retrieves the Driver Information from the Catalog for this Item. Returns
    // the Display Name and HWID for this driver - This is passed to the CDM 
    // installer
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetDriverInfo(HANDLE_NODE hNode, 
                          BSTR* pbstrHWID, 
                          BSTR* pbstrDisplayName);

    /////////////////////////////////////////////////////////////////////////////
    // GetPrinterDriverInfo()
    //
    // Retrieves the Printer Driver Information from the Catalog for this Item. 
    // Returns the DriverName and the Architecture - This is passed to the CDM
    // installer
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetPrinterDriverInfo(HANDLE_NODE hNode,
                                 BSTR* pbstrDriverName,
                                 BSTR* pbstrArchitecture);


	/////////////////////////////////////////////////////////////////////////////
	// GetDriverInfoEx()
	//
	// Combines functionality of IsPrinterDriver, GetDriverInfo, and
	// GetPrinterDriverInfo plus retreives MfgName and DriverProvider.
	// Used by FindMatchingDriver()
	//
	// If SUCCEEDES pbstrHWID, pbstrDriverVer, and pbstrDisplayName
	//    are always returned.
	// If SUCCEEDES && *pFIsPrinter == TRUE then pbstrDriverName,
	//    pbstrDriverProvider, and pbstrMfgName are returned.
	//
	// Currently pbstrArchitecture is never returned.
	//
	/////////////////////////////////////////////////////////////////////////////
    HRESULT CXmlCatalog::GetDriverInfoEx(HANDLE_NODE hNode,
                                         BOOL* pfIsPrinter,
                                         BSTR* pbstrHWID,
										 BSTR* pbstrDriverVer,
                                         BSTR* pbstrDisplayName,
                                         BSTR* pbstrDriverName,
                                         BSTR* pbstrDriverProvider,
                                         BSTR* pbstrMfgName,
                                         BSTR* pbstrArchitecture);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemFirstPlatformStr()
    //
    // The input node pointer points to an item node has <platform> as its child.
    // This function will retrieve the first <platform> node from this item node and
    // convert the data inside <platform> into a string that can be used to
    // uniquely identify a platform.
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemFirstPlatformStr(HANDLE_NODE hNodeItem,
                        BSTR* pbstrPlatform);
    
    /////////////////////////////////////////////////////////////////////////////
    // GetItemAllPlatformStr()
    //
    // The input node pointer points to an item node that has <platform> node(s).
    // This function will retrieve every <platform> node from this item node and
    // convert the data inside <platform> into a string that can be used to
    // uniquely identify a platform.
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemAllPlatformStr(HANDLE_NODE hNodeItem,
                        BSTR** ppbPlatforms, UINT* pnPlatformCount);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemFirstLanguageStr()
    //
    // The input node pointer points to a node has <identity> as its child.
    // This function will retrieve the first <language> node from <identity> 
    // node 
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemFirstLanguageStr(HANDLE_NODE hNodeItem,
                        BSTR* pbstrLanguage);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemAllLanguageStr()
    //
    // The input node pointer points to a node has <identity> as its child.
    // This function will retrieve every <language> node from <identity> node and
    // convert the data into an BSTR array to return.
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemAllLanguageStr(HANDLE_NODE hNodeItem,
                        BSTR** ppbstrLanguage, UINT* pnLangCount);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemFirstCodeBase()
    //
    // Find the first codebase (path) of the given item
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST GetItemFirstCodeBase(HANDLE_NODE hNodeItem,
                                         BSTR* pbstrCodeBase,
                                         BSTR* pbstrName,
                                         BSTR* pbstrCRC,
                                         BOOL* pfPatchAvailable,
                                         LONG* plSize);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemNextCodeBase()
    //
    // Find the next codebase (path) of the given item
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemNextCodeBase(HANDLE_NODELIST hNodeListCodeBase,
                                BSTR* pbstrCodeBase,
                                BSTR* pbstrName,
                                BSTR* pbstrCRC,
                                BOOL* pfPatchAvailable,
                                LONG* plSize);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemInstallInfo()
    //
    // Retrieve the installation information of the given item
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemInstallInfo(HANDLE_NODE hNodeItem,
                               BSTR* pbstrInstallerType,
							   BOOL* pfExclusive,
                               BOOL* pfReboot,
                               LONG* plNumCommands);
        
    /////////////////////////////////////////////////////////////////////////////
    // GetItemInstallCommand()
    //
    // Find the installation command type, command and switches of the given item
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemInstallCommand(HANDLE_NODE hNodeItem,
                                  INT   iOrder,
                                  BSTR* pbstrCommandType,
                                  BSTR* pbstrCommand,
                                  BSTR* pbstrSwitches,
                                  BSTR* pbstrInfSection);

    /////////////////////////////////////////////////////////////////////////////
    // CloseItem()
    //
    // Release the item node
    /////////////////////////////////////////////////////////////////////////////
    void CloseItem(HANDLE_NODE hNodeItem);

    /////////////////////////////////////////////////////////////////////////////
    // GetTotalEstimatedSize()
    //
    // Get the Total Estimated Download Size of all Items based on Codebase Size
    HRESULT GetTotalEstimatedSize(LONG *plTotalSize);

    /////////////////////////////////////////////////////////////////////////////
    // FindItemByIdentity()
    //
    // Finds an Item in the Catalog that Matches the Supplied Identity                
    /////////////////////////////////////////////////////////////////////////////
    HRESULT FindItemByIdentity(IXMLDOMNode* pNodeIdentity, HANDLE_NODE* phNodeItem);

 /*   /////////////////////////////////////////////////////////////////////////////
    // IfSameIdentity()
    //
    // Return TRUE if the two <identity> nodes are identical. Return FALSE otherwise.
    /////////////////////////////////////////////////////////////////////////////
    BOOL IfSameIdentity(IXMLDOMNode* pNodeIdentity1, IXMLDOMNode* pNodeIdentity2);
*/

    /////////////////////////////////////////////////////////////////////////////
    // GetItemLanguage()
    //
    // Get the Language Entity from the Item Identity
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemLanguage(HANDLE_NODE hNodeItem, BSTR* pbstrLanguage);

    /////////////////////////////////////////////////////////////////////////////
    // GetCorpItemPlatformStr()
    //
    // Get the Simplified Platform String for an Item (uses the first available platform element)
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetCorpItemPlatformStr(HANDLE_NODE hNodeItem, BSTR* pbstrPlatformStr);
    

private:
    IXMLDOMDocument*    m_pDocCatalog;

    //
    // helper functions
    //

    //
    // get data from a version node and convert them into a string with
    // format: 
    //          VersionStr   = <Version>[,<SvcPackVer>[,<timeStamp>]]
    //          <Version>    = <Major>[.<Minor>[.<Build>]]
    //          <SvcPackVer> = <Major>[.<minor>]
    //
    //
    // Assumption:
    //          pszVersion points to a buffer LARGE ENOUGH to store
    //          any legal version number.
    //
    HRESULT getVersionStr(IXMLDOMNode* pVersionNode, LPTSTR pszVersion);
    HRESULT getVersionStrWithoutSvcPack(IXMLDOMNode* pVersionNode, LPTSTR pszVersion);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemPlatformStr()
    //
    // The input node pointer points to a <platform> node.
    // This function will retrieve the data from this <platform> node and
    // convert the data into a string that can be used to
    // uniquely identify a platform.
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetPlatformStr(IXMLDOMNode* pNodePlatform, BSTR* pbstrPlatform);
    HRESULT GetPlatformStrForPing(IXMLDOMNode* pNodePlatform, BSTR* pbstrPlatform);

private:

};


/////////////////////////////////////////////////////////////////////////////
// CXmlItems
class CXmlItems : public CIUXml
{
public:
    /////////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    // Create IXMLDOMDocument* for Items; this is for write only
    /////////////////////////////////////////////////////////////////////////////
    CXmlItems();

    /////////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    // Create IXMLDOMDocument* for Items; take TRUE for read, FALSE for write
    /////////////////////////////////////////////////////////////////////////////
    CXmlItems(BOOL fRead);

    /////////////////////////////////////////////////////////////////////////////
    // Destructor
    //
    // Release IXMLDOMDocument* for Items
    /////////////////////////////////////////////////////////////////////////////
    ~CXmlItems();

    /////////////////////////////////////////////////////////////////////////////
    // GetItemsDocumentPtr()
    //
    // Retrieve the items XML Document node
    /////////////////////////////////////////////////////////////////////////////
    inline IXMLDOMDocument* GetItemsDocumentPtr() { return m_pDocItems; };

    /////////////////////////////////////////////////////////////////////////////
    // Clear()
    //
    // Reset IXMLDOMDocument* for Items
    /////////////////////////////////////////////////////////////////////////////
    void Clear();

	/////////////////////////////////////////////////////////////////////////////
	// LoadXMLDocument()
	//
	// Load an XML Document from string
	/////////////////////////////////////////////////////////////////////////////
	HRESULT LoadXMLDocument(BSTR bstrXml);

	/////////////////////////////////////////////////////////////////////////////
	// LoadXMLDocumentFile()
	//
	// Load an XML Document from the specified file
	/////////////////////////////////////////////////////////////////////////////
	HRESULT LoadXMLDocumentFile(BSTR bstrFilePath);

    /////////////////////////////////////////////////////////////////////////////
    // SaveXMLDocument()
    //
    // Save an XML Document to the specified location
    /////////////////////////////////////////////////////////////////////////////
    HRESULT SaveXMLDocument(BSTR bstrFilePath);

	/////////////////////////////////////////////////////////////////////////////
	// AddGlobalErrorCodeIfNoItems()
	//
	// Add the errorCode attribute for <items> if there's no <itemStatus> child node
	/////////////////////////////////////////////////////////////////////////////
	HANDLE_NODELIST AddGlobalErrorCodeIfNoItems(DWORD dwErrorCode);

    /////////////////////////////////////////////////////////////////////////////
    // GetFirstItem()
    //
    // Find the first item in Items xml doc
    /////////////////////////////////////////////////////////////////////////////
    HANDLE_NODELIST GetFirstItem(HANDLE_NODE* phNodeItem);
        
    /////////////////////////////////////////////////////////////////////////////
    // GetNextItem()
    //
    // Find the next item in Items xml doc
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetNextItem(HANDLE_NODELIST hNodeListItem, HANDLE_NODE* phNodeItem);    
        
    /////////////////////////////////////////////////////////////////////////////
    // CloseItemList()
    //
    // Release the item nodelist
    /////////////////////////////////////////////////////////////////////////////
    void CloseItemList(HANDLE_NODELIST hNodeListItem);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemDownloadPath()
    //
    // Retrieve the download path of the given item
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemDownloadPath(HANDLE_NODE hNodeItem, BSTR* pbstrDownloadPath);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemDownloadPath()
    //
    // Retrieve the download path of the given item in catalog
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemDownloadPath(CXmlCatalog* pCatalog, HANDLE_NODE hNodeItem, BSTR* pbstrDownloadPath);

    /////////////////////////////////////////////////////////////////////////////
    // CloseItem()
    //
    // Release the item node
    /////////////////////////////////////////////////////////////////////////////
    void CloseItem(HANDLE_NODE hNodeItem);

    /////////////////////////////////////////////////////////////////////////////
    // FindItem()
    //
    // Input:
    // pNodeItem    - the <itemStatus> node of the install items xml; we need
    //                to find the corresponding <itemStatus> node in the existing 
    //                items xml with the identical <identity>, <platform> and 
    //                <client> nodes.
    // Output:
    // phNodeItem   - the handle we pass back to differentiate different
    //                <itemStatus> node in the existing items xml
    /////////////////////////////////////////////////////////////////////////////
    HRESULT FindItem(IXMLDOMNode* pNodeItem, HANDLE_NODE* phNodeItem, BOOL fIdentityOnly = FALSE);

	/////////////////////////////////////////////////////////////////////////////
	// FindItem()
	//
	// Input:
	// pCatalog		- the pointer to the CXmlCatalog object
	// hNodeItem	- the handle of the <item> node of the catalog xml; we need
	//                to find the corresponding <itemStatus> node in the existing 
	//                items xml with the identical <identity>, <platform> and 
	//                <client> nodes.
	// Output:
	// phNodeItem	- the handle we pass back to differentiate different
	//				  <itemStatus> node in items xml
	/////////////////////////////////////////////////////////////////////////////
	HRESULT FindItem(CXmlCatalog* pCatalog,	HANDLE_NODE hNodeItem, HANDLE_NODE* phNodeItem);

    /////////////////////////////////////////////////////////////////////////////
    // MergeItemDownloaded()
    //
    // Insert items with download history into existing history
    /////////////////////////////////////////////////////////////////////////////
    HRESULT MergeItemDownloaded(CXmlItems *pHistoryDownload);

    /////////////////////////////////////////////////////////////////////////////
    // UpdateItemInstalled()
    //
    // Update items with installation history in existing history
    /////////////////////////////////////////////////////////////////////////////
    HRESULT UpdateItemInstalled(CXmlItems *pHistoryInstall);

    /////////////////////////////////////////////////////////////////////////////
    // UpdateItemInstallStatus()
    //
    // Update the install status of the given item
    /////////////////////////////////////////////////////////////////////////////
    HRESULT UpdateItemInstallStatus(HANDLE_NODE hNodeItem,
                                    BSTR bstrValue,
                                    INT iNeedsReboot = -1,
                                    DWORD dwErrorCode = 0);

    /////////////////////////////////////////////////////////////////////////////
    // AddItem()
    //
    // Input:
    // pNodeItem    - the <item> node of the catalog xml; we need to read
    //                <identity> node, <description> node and <platform> nodes
    //                from it and write to the items xml (insert in front).
    // Output:
    // phNodeItem   - the handle we pass back to differentiate different
    //                <itemStatus> node in items xml
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddItem(IXMLDOMNode* pNodeItem, HANDLE_NODE* phNodeItem);

    /////////////////////////////////////////////////////////////////////////////
    // AddItem()
    //
    // Input:
    // pCatalog     - the pointer to the CXmlCatalog object
    // hNodeItem    - the handle of the <item> node of the catalog xml; we need
    //                to read <identity> node, <description> node and <platform>
    //                nodes from it and write to the items xml (insert in front).
    // Output:
    // phNodeItem   - the handle we pass back to differentiate different
    //                <itemStatus> node in items xml
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddItem(CXmlCatalog* pCatalog, HANDLE_NODE hNodeItem, HANDLE_NODE* phNodeItem);
	
	/////////////////////////////////////////////////////////////////////////////
    // AddTimeStamp()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddTimeStamp(HANDLE_NODE hNodeItem);

    /////////////////////////////////////////////////////////////////////////////
    // AddDetectResult()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddDetectResult(HANDLE_NODE hNodeItem,
                            INT iInstalled    = -1,
                            INT iUpToDate     = -1,
                            INT iNewerVersion = -1,
                            INT iExcluded     = -1,
                            INT iForce        = -1,
							INT iComputerSystem = -1);

    /////////////////////////////////////////////////////////////////////////////
    // AddDownloadStatus()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddDownloadStatus(HANDLE_NODE hNodeItem, BSTR bstrValue, DWORD dwErrorCode = 0);

    /////////////////////////////////////////////////////////////////////////////
    // AddDownloadPath()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddDownloadPath(HANDLE_NODE hNodeItem, BSTR bstrDownloadPath);

    /////////////////////////////////////////////////////////////////////////////
    // AddInstallStatus()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddInstallStatus(HANDLE_NODE hNodeItem, BSTR bstrValue, BOOL fNeedsReboot, DWORD dwErrorCode = 0);

    /////////////////////////////////////////////////////////////////////////////
    // AddClientInfo()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT AddClientInfo(HANDLE_NODE hNodeItem, BSTR bstrClient);

	/////////////////////////////////////////////////////////////////////////////
	// MigrateV3History()
	//
	// Migrate V3 History: Consumer history only.
	/////////////////////////////////////////////////////////////////////////////
	HRESULT MigrateV3History(LPCTSTR pszHistoryFilePath);

    /////////////////////////////////////////////////////////////////////////////
    // GetItemsBSTR()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetItemsBSTR(BSTR *pbstrXmlItems);

    /////////////////////////////////////////////////////////////////////////////
    // GetFilteredHistoryBSTR()
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetFilteredHistoryBSTR(BSTR bstrBeginDateTime,
                                   BSTR bstrEndDateTime,
                                   BSTR bstrClient,
                                   BSTR *pbstrXmlHistory);

private:

	/////////////////////////////////////////////////////////////////////////////
	//
	// Initialize XML DOC node pointers before writing
	/////////////////////////////////////////////////////////////////////////////
	void Init();

/*    /////////////////////////////////////////////////////////////////////////////
    // IfSameClientInfo()
    //
    // Return TRUE if the two <client> nodes are identical. Return FALSE otherwise.
    /////////////////////////////////////////////////////////////////////////////
    BOOL IfSameClientInfo(IXMLDOMNode* pNodeClient1, IXMLDOMNode* pNodeClient2);

    /////////////////////////////////////////////////////////////////////////////
    // IfSameIdentity()
    //
    // Return TRUE if the two <identity> nodes are identical. Return FALSE otherwise.
    /////////////////////////////////////////////////////////////////////////////
    BOOL IfSameIdentity(IXMLDOMNode* pNodeIdentity1, IXMLDOMNode* pNodeIdentity2);

    /////////////////////////////////////////////////////////////////////////////
    // IfSamePlatform()
    //
    // Return TRUE if the two <platform> nodes are identical. Return FALSE otherwise.
    /////////////////////////////////////////////////////////////////////////////
    BOOL IfSamePlatform(IXMLDOMNode* pNodePlatform1, IXMLDOMNode* pNodePlatform2);
*/

    IXMLDOMDocument*    m_pDocItems;
    IXMLDOMNode*        m_pNodeItems;
};




/////////////////////////////////////////////////////////////////////////////
// CXmlClientInfo
class CXmlClientInfo
{
public:

    CXmlClientInfo();
    ~CXmlClientInfo();

    //
    // load and parse and validate an XML document from string
    //
    HRESULT LoadXMLDocument(BSTR bstrXml, BOOL fOfflineMode);

    //
    // retrieve client name attribute
    //
    HRESULT GetClientName(BSTR* pbstrClientName);

	inline IXMLDOMDocument* GetDocument() { return m_pDocClientInfo; }

private:
    IXMLDOMDocument*    m_pDocClientInfo;


};



#endif //__IUXML_H_
