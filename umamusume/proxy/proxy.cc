#include <string>

#include <Windows.h>

using namespace std;

typedef DWORD(WINAPI *VerFindFileA_t)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT, LPSTR, PUINT);
VerFindFileA_t orig_VerFindFileA;
DWORD WINAPI VerFindFileA(DWORD uFlags,
                          LPCSTR szFileName,
                          LPCSTR szWinDir,
                          LPCSTR szAppDir,
                          LPSTR szCurDir,
                          PUINT puCurDirLen,
                          LPSTR szDestDir,
                          PUINT puDestDirLen)
{
  return (orig_VerFindFileA)(uFlags, szFileName, szWinDir, szAppDir, szCurDir, puCurDirLen, szDestDir, puDestDirLen);
}

typedef DWORD(WINAPI *VerFindFileW_t)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
VerFindFileW_t orig_VerFindFileW;
DWORD WINAPI VerFindFileW(DWORD uFlags,
                          LPCWSTR szFileName,
                          LPCWSTR szWinDir,
                          LPCWSTR szAppDir,
                          LPWSTR szCurDir,
                          PUINT puCurDirLen,
                          LPWSTR szDestDir,
                          PUINT puDestDirLen)
{
  return (orig_VerFindFileW)(uFlags, szFileName, szWinDir, szAppDir, szCurDir, puCurDirLen, szDestDir, puDestDirLen);
}

typedef DWORD(WINAPI *VerInstallFileA_t)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT);
VerInstallFileA_t orig_VerInstallFileA;
DWORD WINAPI VerInstallFileA(DWORD uFlags,
                             LPCSTR szSrcFileName,
                             LPCSTR szDestFileName,
                             LPCSTR szSrcDir,
                             LPCSTR szDestDir,
                             LPCSTR szCurDir,
                             LPSTR szTmpFile,
                             PUINT puTmpFileLen)
{
  return (orig_VerInstallFileA)(uFlags,
                                szSrcFileName,
                                szDestFileName,
                                szSrcDir,
                                szDestDir,
                                szCurDir,
                                szTmpFile,
                                puTmpFileLen);
}

typedef DWORD(WINAPI *VerInstallFileW_t)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT);
VerInstallFileW_t orig_VerInstallFileW;
DWORD WINAPI VerInstallFileW(DWORD uFlags,
                             LPCWSTR szSrcFileName,
                             LPCWSTR szDestFileName,
                             LPCWSTR szSrcDir,
                             LPCWSTR szDestDir,
                             LPCWSTR szCurDir,
                             LPWSTR szTmpFile,
                             PUINT puTmpFileLen)
{
  return (orig_VerInstallFileW)(uFlags,
                                szSrcFileName,
                                szDestFileName,
                                szSrcDir,
                                szDestDir,
                                szCurDir,
                                szTmpFile,
                                puTmpFileLen);
}

typedef DWORD(WINAPI *GetFileVersionInfoSizeA_t)(LPCSTR, LPDWORD);
GetFileVersionInfoSizeA_t orig_GetFileVersionInfoSizeA;
DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
  return (orig_GetFileVersionInfoSizeA)(lptstrFilename, lpdwHandle);
}

typedef DWORD(WINAPI *GetFileVersionInfoSizeW_t)(LPCWSTR, LPDWORD);
GetFileVersionInfoSizeW_t orig_GetFileVersionInfoSizeW;
DWORD WINAPI GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle)
{
  return (orig_GetFileVersionInfoSizeW)(lptstrFilename, lpdwHandle);
}

typedef DWORD(WINAPI *GetFileVersionInfoA_t)(LPCSTR, DWORD, DWORD, LPVOID);
GetFileVersionInfoA_t orig_GetFileVersionInfoA;
BOOL WINAPI GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
  return (orig_GetFileVersionInfoA)(lptstrFilename, dwHandle, dwLen, lpData);
}

typedef DWORD(WINAPI *GetFileVersionInfoW_t)(LPCWSTR, DWORD, DWORD, LPVOID);
GetFileVersionInfoW_t orig_GetFileVersionInfoW;
BOOL WINAPI GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
  return (orig_GetFileVersionInfoW)(lptstrFilename, dwHandle, dwLen, lpData);
}

typedef DWORD(WINAPI *GetFileVersionInfoSizeExA_t)(DWORD, LPCSTR, LPDWORD);
GetFileVersionInfoSizeExA_t orig_GetFileVersionInfoSizeExA;
DWORD WINAPI GetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle)
{
  return (orig_GetFileVersionInfoSizeExA)(dwFlags, lpwstrFilename, lpdwHandle);
}

typedef DWORD(WINAPI *GetFileVersionInfoSizeExW_t)(DWORD, LPCWSTR, LPDWORD);
GetFileVersionInfoSizeExW_t orig_GetFileVersionInfoSizeExW;
DWORD WINAPI GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle)
{
  return (orig_GetFileVersionInfoSizeExW)(dwFlags, lpwstrFilename, lpdwHandle);
}

typedef DWORD(WINAPI *GetFileVersionInfoExA_t)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
GetFileVersionInfoExA_t orig_GetFileVersionInfoExA;
BOOL WINAPI GetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
  return (orig_GetFileVersionInfoExA)(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

typedef DWORD(WINAPI *GetFileVersionInfoExW_t)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
GetFileVersionInfoExW_t orig_GetFileVersionInfoExW;
BOOL WINAPI GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
  return (orig_GetFileVersionInfoExW)(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

typedef DWORD(WINAPI *VerLanguageNameA_t)(DWORD, LPSTR, DWORD);
VerLanguageNameA_t orig_VerLanguageNameA;
DWORD WINAPI VerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD cchLang)
{
  return (orig_VerLanguageNameA)(wLang, szLang, cchLang);
}

typedef DWORD(WINAPI *VerLanguageNameW_t)(DWORD, LPWSTR, DWORD);
VerLanguageNameW_t orig_VerLanguageNameW;
DWORD WINAPI VerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD cchLang)
{
  return (orig_VerLanguageNameW)(wLang, szLang, cchLang);
}

typedef DWORD(WINAPI *VerQueryValueA_t)(LPCVOID, LPCSTR, LPVOID *, PUINT);
VerQueryValueA_t orig_VerQueryValueA;
BOOL WINAPI VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
  return (orig_VerQueryValueA)(pBlock, lpSubBlock, lplpBuffer, puLen);
}

typedef DWORD(WINAPI *VerQueryValueW_t)(LPCVOID, LPCWSTR, LPVOID *, PUINT);
VerQueryValueW_t orig_VerQueryValueW;
BOOL WINAPI VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
  return (orig_VerQueryValueW)(pBlock, lpSubBlock, lplpBuffer, puLen);
}

typedef void(WINAPI *GetFileVersionInfoByHandle_t)();
GetFileVersionInfoByHandle_t orig_GetFileVersionInfoByHandle;
void WINAPI GetFileVersionInfoByHandle()
{
  (orig_GetFileVersionInfoByHandle)();
}

void initProxy()
{
  string dllPath;
  dllPath.resize(MAX_PATH);
  dllPath.resize(GetSystemDirectoryA(dllPath.data(), MAX_PATH));
  dllPath += "\\" + "version.dll"s;

  HMODULE hModule = LoadLibrary(dllPath.c_str());

  orig_VerFindFileA = (VerFindFileA_t)GetProcAddress(hModule, "VerFindFileA");
  orig_VerFindFileW = (VerFindFileW_t)GetProcAddress(hModule, "VerFindFileW");
  orig_VerInstallFileA = (VerInstallFileA_t)GetProcAddress(hModule, "VerInstallFileA");
  orig_VerInstallFileW = (VerInstallFileW_t)GetProcAddress(hModule, "VerInstallFileW");
  orig_GetFileVersionInfoA = (GetFileVersionInfoA_t)GetProcAddress(hModule, "GetFileVersionInfoA");
  orig_GetFileVersionInfoW = (GetFileVersionInfoW_t)GetProcAddress(hModule, "GetFileVersionInfoW");
  orig_GetFileVersionInfoSizeA = (GetFileVersionInfoSizeA_t)GetProcAddress(hModule, "GetFileVersionInfoSizeA");
  orig_GetFileVersionInfoSizeW = (GetFileVersionInfoSizeW_t)GetProcAddress(hModule, "GetFileVersionInfoSizeW");
  orig_GetFileVersionInfoExA = (GetFileVersionInfoExA_t)GetProcAddress(hModule, "GetFileVersionInfoExA");
  orig_GetFileVersionInfoExW = (GetFileVersionInfoExW_t)GetProcAddress(hModule, "GetFileVersionInfoExW");
  orig_GetFileVersionInfoSizeExA = (GetFileVersionInfoSizeExA_t)GetProcAddress(hModule, "GetFileVersionInfoSizeExA");
  orig_GetFileVersionInfoSizeExW = (GetFileVersionInfoSizeExW_t)GetProcAddress(hModule, "GetFileVersionInfoSizeExW");
  orig_VerLanguageNameA = (VerLanguageNameA_t)GetProcAddress(hModule, "VerLanguageNameA");
  orig_VerLanguageNameW = (VerLanguageNameW_t)GetProcAddress(hModule, "VerLanguageNameW");
  orig_VerQueryValueA = (VerQueryValueA_t)GetProcAddress(hModule, "VerQueryValueA");
  orig_VerQueryValueW = (VerQueryValueW_t)GetProcAddress(hModule, "VerQueryValueW");
  orig_GetFileVersionInfoByHandle = (GetFileVersionInfoByHandle_t)GetProcAddress(hModule, "GetFileVersionInfoByHandle");
}