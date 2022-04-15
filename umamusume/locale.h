#ifndef UMAMUSUME_LOCALE_H_
#define UMAMUSUME_LOCALE_H_

#include <unordered_map>
#include <nlohmann/json.hpp>
#include <umamusume/il2cpp.h>

class Locale
{
  static std::unordered_map<size_t, std::string> originalDictionary;
  std::unordered_map<size_t, std::string> dictionary;

public:
  Locale();
  static void init();

  bool has(size_t hash);
  bool has(Il2CppString *str);

  std::string get(size_t hash);
  std::string get(Il2CppString *str);
  void set(Il2CppString *str, size_t hash = 0);

  std::string utf8(std::wstring str);
  std::string utf8(Il2CppString *str);

  Il2CppString *localize(Il2CppString *str);

  void dump();
  void log(Il2CppString *str);
};

#endif  // UMAMUSUME_LOCALE_H_