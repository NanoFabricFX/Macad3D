// dllmain.cpp : Defines the entry point for the DLL application.
#include "ShellExtensionPCH.h"
#include "ModelInfoClassFactory.h"
#include "Registration.h"

// {E8984C18-85E9-4AEB-8662-26F3C7323D92}
static const GUID CLSID_ModelInfoProvider = 
{ 0xe8984c18, 0x85e9, 0x4aeb, { 0x86, 0x62, 0x26, 0xf3, 0xc7, 0x32, 0x3d, 0x92 } };
 
HINSTANCE   g_hInst     = nullptr; 
long        g_cDllRef   = 0;

//--------------------------------------------------------------------------------------------------

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		// Hold the instance of this DLL module, we will use it to get the  
        // path of the DLL to register the component. 
        g_hInst = hModule; 
        DisableThreadLibraryCalls(hModule);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//--------------------------------------------------------------------------------------------------

// 
//   FUNCTION: DllGetClassObject 
// 
//   PURPOSE: Create the class factory and query to the specific interface. 
// 
//   PARAMETERS: 
//   * rclsid - The CLSID that will associate the correct data and code. 
//   * riid - A reference to the identifier of the interface that the caller  
//     is to use to communicate with the class object. 
//   * ppv - The address of a pointer variable that receives the interface  
//     pointer requested in riid. Upon successful return, *ppv contains the  
//     requested interface pointer. If an error occurs, the interface pointer  
//     is NULL.  
// 
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) 
{ 
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE; 
 
    if (IsEqualCLSID(CLSID_ModelInfoProvider, rclsid)) 
    { 
        hr = E_OUTOFMEMORY; 
 
        ModelInfoClassFactory *pClassFactory = new ModelInfoClassFactory(); 
        if (pClassFactory) 
        { 
            hr = pClassFactory->QueryInterface(riid, ppv); 
            pClassFactory->Release(); 
        } 
    } 
 
    return hr; 
} 

// 
//   FUNCTION: DllCanUnloadNow 
// 
//   PURPOSE: Check if we can unload the component from the memory. 
// 
//   NOTE: The component can be unloaded from the memory when its reference  
//   count is zero (i.e. nobody is still using the component). 
//  
STDAPI DllCanUnloadNow(void) 
{ 
    return g_cDllRef > 0 ? S_FALSE : S_OK; 
} 
 
// 
//   FUNCTION: DllRegisterServer 
// 
//   PURPOSE: Register the COM server and the thumbnail handler. 
//  
STDAPI DllRegisterServer(void) 
{ 
    HRESULT hr; 
 
    wchar_t szModule[MAX_PATH]; 
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) 
    { 
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        return hr; 
    } 
 
    // Register the component. 
    hr = RegisterInprocServer(szModule, CLSID_ModelInfoProvider, L"Macad.ModelInfoProvider Class", L"Apartment"); 
    if (SUCCEEDED(hr)) 
    { 
        // Register the thumbnail handler. The thumbnail handler is associated 
        // with the .recipe file class. 
        hr = RegisterShellExtThumbnailHandler(L".model", CLSID_ModelInfoProvider); 
        if (SUCCEEDED(hr)) 
        { 
            // This tells the shell to invalidate the thumbnail cache. It is  
            // important because any .recipe files viewed before registering  
            // this handler would otherwise show cached blank thumbnails. 
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL); 
        } 
    } 
 
    return hr; 
} 


// 
//   FUNCTION: DllUnregisterServer 
// 
//   PURPOSE: Unregister the COM server and the thumbnail handler. 
//  
STDAPI DllUnregisterServer(void) 
{ 
    HRESULT hr = S_OK; 
 
    wchar_t szModule[MAX_PATH]; 
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) 
    { 
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        return hr; 
    } 
 
    // Unregister the component. 
    hr = UnregisterInprocServer(CLSID_ModelInfoProvider); 
    if (SUCCEEDED(hr)) 
    { 
        // Unregister the thumbnail handler. 
        hr = UnregisterShellExtThumbnailHandler(L".model"); 
    } 
 
    return hr; 
}

