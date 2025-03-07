//-----------------------------------------------------------------------------

//  

//  File: WMIClass.H

// Copyright (c) 1994-2001 Microsoft Corporation, All Rights Reserved 
//  All rights reserved.
//  
//  
//  
//-----------------------------------------------------------------------------


#ifndef WMICLASS_H
#define WMICLASS_H

class CWMILocClassFactory : public IClassFactory, public CLObject
{
public:
	CWMILocClassFactory();

#ifdef _DEBUG
	void AssertValid(void) const;
	void Dump(CDumpContext &) const;
#endif
	
	STDMETHOD_(ULONG, AddRef)(); 
	STDMETHOD_(ULONG, Release)(); 
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

	STDMETHOD(CreateInstance)(THIS_ LPUNKNOWN, REFIID, LPVOID *);
	STDMETHOD(LockServer)(THIS_ BOOL);

	~CWMILocClassFactory();
	
private:
	UINT m_uiRefCount;
	
};


#endif // WMICLASS_H
