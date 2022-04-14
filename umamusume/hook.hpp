#pragma once

#include <functional>
#include <iostream>

#include <MinHook.h>
#include <Windows.h>
#include <umamusume/il2cpp.h>

namespace Hook
{
namespace
{
#define RESOLVE(name) \
  std::cout << "Resolving " << #name << "..." << std::endl; \
  while (name == nullptr) { \
    auto addr = GetProcAddress(instance, #name); \
    if (addr != nullptr) \
      name = reinterpret_cast<decltype(name)>(addr); \
  }

static HMODULE instance = nullptr;
static bool isInitialized = false;
static void *domain;

template<typename T, typename... Types>
constexpr size_t getArgumentCount(T (*f)(Types...))
{
  return sizeof...(Types);
}
}  // namespace

void init()
{
  if (isInitialized)
    return;

  while (MH_Initialize() != MH_OK)
    continue;

  isInitialized = true;
}

void load(const char *moduleName)
{
  if (instance != nullptr)
    return;

  while (instance == nullptr)
    instance = LoadLibrary(moduleName);

  RESOLVE(il2cpp_string_new_utf16);
  RESOLVE(il2cpp_string_new);
  RESOLVE(il2cpp_domain_get);
  RESOLVE(il2cpp_domain_assembly_open);
  RESOLVE(il2cpp_assembly_get_image);
  RESOLVE(il2cpp_class_from_name);
  RESOLVE(il2cpp_class_get_methods);
  RESOLVE(il2cpp_class_get_method_from_name);
  RESOLVE(il2cpp_method_get_param);
  RESOLVE(il2cpp_object_new);
  RESOLVE(il2cpp_resolve_icall);
  RESOLVE(il2cpp_array_new);
  RESOLVE(il2cpp_thread_attach);
  RESOLVE(il2cpp_thread_detach);

  domain = il2cpp_domain_get();
  if (domain == nullptr)
    std::cout << "Unable to get domain" << std::endl;
}

void *getClass(const char *assemblyName, const char *namespaze, const char *className)
{
  void *assembly = il2cpp_domain_assembly_open(domain, assemblyName);
  if (assembly == nullptr) {
    std::cout << "Unable to open assembly " << assemblyName << std::endl;
    return nullptr;
  }

  void *image = il2cpp_assembly_get_image(assembly);
  if (image == nullptr) {
    std::cout << "Unable to get image for assembly " << assemblyName << std::endl;
    return nullptr;
  }

  return il2cpp_class_from_name(image, namespaze, className);
}

MethodInfo *getMethod(void *klass, const char *name, int argsCount)
{
  return il2cpp_class_get_method_from_name(klass, name, argsCount);
}


MethodInfo *findMethod(void *klass, const char *name, std::function<bool(const MethodInfo *)> predicate)
{
  void *iter = nullptr;
  MethodInfo *method = nullptr;

  while ((method = il2cpp_class_get_methods(klass, &iter)) != nullptr) {
    if (strcmp(method->name, name) == 0 && predicate(method))
      return method;
  }

  return nullptr;
}

template<typename A, typename B>
void hook(void *target, A replace, B original)
{
  std::cout << "MH_CreateHook: " << MH_CreateHook(target, (void *)replace, (void **)original) << std::endl;
  std::cout << "MH_EnableHook: " << MH_EnableHook(target) << std::endl;
}

template<typename A, typename B>
void hook(const char *imageName, const char *namespaze, const char *className, const char *fn, A replace, B original)
{
  if (imageName == nullptr)
    imageName = "Assembly-CSharp.dll";

  void *klass = getClass(imageName, namespaze, className);
  if (klass == nullptr) {
    std::cout << "Failed to get class " << className << std::endl;
    return;
  }

  MethodInfo *method = getMethod(klass, fn, getArgumentCount(replace) - 1);
  if (method == nullptr) {
    std::cout << "Failed to get method " << fn << std::endl;
    return;
  }

  std::cout << "Hooking " << className << "::" << fn << std::endl;
  hook((void *)method->methodPointer, replace, original);
}

template<typename A, typename B>
void hook(const char *namespaze, const char *className, const char *fn, A replace, B original)
{
  return hook(nullptr, namespaze, className, fn, replace, original);
}

template<typename A, typename B>
void hook(const char *className, const char *fn, A replace, B original)
{
  return hook(nullptr, nullptr, className, fn, replace, original);
}

void unhook(void *target)
{
  if (target == nullptr)
    return;

  MH_DisableHook(target);
  MH_RemoveHook(target);
}

void dumpBytes(void *addr)
{
  printf("Dumping %p\n", addr);
  char *ptr = (char *)addr;
  for (int i = 0; i < 0x20; i++) {
    if (i > 0 && i % 16 == 0)
      printf("\n");
    printf("%02hhx", *(ptr++));
  }
  printf("\n\n");
}
}  // namespace Hook