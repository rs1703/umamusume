#include <atomic>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include <Windows.h>
#include <umamusume/app.h>
#include <umamusume/hook.hpp>
#include <umamusume/locale.h>
#include <umamusume/proxy/proxy.h>

void init()
{
  Hook::load("GameAssembly.dll");
  locale = new Locale();

  auto textClass = Hook::getClass("UnityEngine.UI.dll", "UnityEngine.UI", "Text");
  auto textCommonClass = Hook::getClass("umamusume.dll", "Gallop", "TextCommon");
  auto localizeClass = Hook::getClass("umamusume.dll", "Gallop", "Localize");

  assignDefaultFont = (assignDefaultFont_t)(Hook::getMethod(textClass, "AssignDefaultFont", 0));
  getLineSpacing = (getLineSpacing_t)(Hook::getMethod(textClass, "get_lineSpacing", 0));
  setLineSpacing = (setLineSpacing_t)(Hook::getMethod(textClass, "set_lineSpacing", 1));
  getFontStyle = (getFontStyle_t)(Hook::getMethod(textClass, "get_fontStyle", 0));
  setFontStyle = (setFontStyle_t)(Hook::getMethod(textClass, "set_fontStyle", 1));
  getFontSize = (getFontSize_t)(Hook::getMethod(textCommonClass, "get_FontSize", 0));
  setFontSize = (setFontSize_t)(Hook::getMethod(textCommonClass, "set_FontSize", 1));

  Hook::hook("UnityEngine.TextRenderingModule.dll", "UnityEngine",
             "TextGenerator", "PopulateWithErrors",
             populateWithErrors, &orig_populateWithErrors);

  Hook::hook("LibNative.Runtime.dll", "LibNative.Sqlite3", "Query", ".ctor", queryCtor, &orig_queryCtor);
  Hook::hook("LibNative.Runtime.dll", "LibNative.Sqlite3", "Query", "Dispose", queryDtor, &orig_queryDtor);
  Hook::hook("LibNative.Runtime.dll", "LibNative.Sqlite3", "Query", "GetText", queryGetText, &orig_queryGetText);

  auto localizeGetPtr = Hook::findMethod(localizeClass, "Get", [](const MethodInfo *method) {
    return method->parameters->parameter_type->type == IL2CPP_TYPE_VALUETYPE;
  });

  Hook::hook((void *)localizeGetPtr->methodPointer, localizeGet, &orig_LocalizeGet);
}

void *orig_LoadLibraryW = nullptr;
HMODULE __stdcall hLoadLibraryW(const wchar_t *path)
{
  if (std::wstring(path) != L"cri_ware_unity.dll")
    return reinterpret_cast<decltype(LoadLibraryW) *>(orig_LoadLibraryW)(path);
  
  CloseHandle(CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), NULL, 0, NULL));
  Hook::unhook(LoadLibraryW);
  return LoadLibraryW(path);
}

void mainRoutine()
{
  AllocConsole();
  freopen("CONOUT$", "w+t", stdout);

  SetConsoleOutputCP(65001);
  std::locale::global(std::locale(""));

  Hook::init();
  Hook::hook(LoadLibraryW, hLoadLibraryW, &orig_LoadLibraryW);
}

int __stdcall DllMain(HINSTANCE, DWORD reason, LPVOID)
{
  if (reason != DLL_PROCESS_ATTACH)
    return true;

  initProxy();

  std::string moduleName;
  moduleName.resize(MAX_PATH);
  moduleName.resize(GetModuleFileName(nullptr, moduleName.data(), MAX_PATH));

  if (std::filesystem::path(moduleName).filename() == "umamusume.exe")
    CloseHandle(CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(mainRoutine), NULL, 0, NULL));

  return true;
}