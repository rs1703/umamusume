#ifndef UMAMUSUME_LOCALE_H_
#define UMAMUSUME_LOCALE_H_

#include <unordered_map>

#include <umamusume/il2cpp.h>

class Locale
{
  std::unordered_map<size_t, std::string> dictionary;

public:
  Locale(const char *path = "locale.json");

  std::string get(size_t hash);
  std::string get(Il2CppString *str);
  void set(Il2CppString *str);

  bool has(size_t hash);
  bool has(Il2CppString *str);

  std::string utf8(std::wstring str);
  std::string utf8(Il2CppString *str);

  Il2CppString *localize(Il2CppString *str);
};

#endif  // UMAMUSUME_LOCALE_H_