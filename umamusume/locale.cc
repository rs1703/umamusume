#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include <Windows.h>
#include <umamusume/app.h>
#include <umamusume/config.h>
#include <umamusume/locale.h>

static const char *originalLocalePath = "locale.json";
static std::mutex originalDictionaryMutex;

std::unordered_map<size_t, std::string> Locale::originalDictionary;
int originalDictionaryModifiedCount = 0;

Locale::Locale()
{
  for (const auto &path : config->dictionaries) {
    std::ifstream file(path);
    if (!file.good())
      return;

    nlohmann::json json;
    file >> json;

    for (const auto &[key, value] : json.items())
      dictionary.emplace(std::stoull(key), value);
  }
}

void Locale::init()
{
  std::ifstream file(originalLocalePath);
  if (file.good()) {
    nlohmann::json json;
    file >> json;

    for (const auto &[key, value] : json.items())
      originalDictionary.emplace(std::stoull(key), value);
  }

  CloseHandle(CreateThread(NULL, 0, [](LPVOID) -> DWORD {
    int prevModifiedCount = originalDictionaryModifiedCount;
    for (;;) {
      std::this_thread::sleep_for(std::chrono::seconds(3));
      std::lock_guard lock(originalDictionaryMutex);

      if (originalDictionaryModifiedCount == prevModifiedCount)
        continue;
      prevModifiedCount = originalDictionaryModifiedCount;

      std::ofstream file(originalLocalePath, std::ios::trunc);
      nlohmann::json json;

      for (const auto &[key, value] : originalDictionary)
        json[std::to_string(key)] = value;
      file << json.dump(2);
    }
  }, NULL, 0, NULL));
}

bool Locale::has(size_t hash)
{
  return dictionary.find(hash) != dictionary.end();
}

bool Locale::has(Il2CppString *str)
{
  if (str == nullptr)
    return false;

  size_t hash = std::hash<std::wstring> {}(str->start_char);
  return has(hash);
}

void Locale::set(Il2CppString *str, size_t hash)
{
  if (str == nullptr)
    return;

  std::lock_guard lock(originalDictionaryMutex);

  if (hash == 0)
    hash = std::hash<std::wstring> {}(str->start_char);

  if (originalDictionary.find(hash) != originalDictionary.end())
    return;

  originalDictionary.emplace(hash, utf8(str));
  originalDictionaryModifiedCount++;
}

std::string Locale::get(size_t hash)
{
  auto it = dictionary.find(hash);
  return it == dictionary.end() ? "" : it->second;
}

std::string Locale::get(Il2CppString *str)
{
  if (str == nullptr)
    return "";

  size_t hash = std::hash<std::wstring> {}(str->start_char);
  return get(hash);
}

std::string Locale::utf8(std::wstring str)
{
  std::string result;
  result.resize(str.length() * 4);

  int len = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.length(), result.data(), result.size(), nullptr, nullptr);
  result.resize(len);
  return result;
}

std::string Locale::utf8(Il2CppString *str)
{
  return utf8(str->start_char);
}

Il2CppString *Locale::localize(Il2CppString *str)
{
  if (str == nullptr)
    return nullptr;

  auto hash = std::hash<std::wstring> {}(str->start_char);
  if (!has(hash)) {
    set(str, hash);
    return str;
  }

  if (!config->enableLocale)
    return str;

  auto localeString = get(hash);
  if (hash == std::hash<std::string> {}(localeString))
    return str;

  return il2cpp_string_new(localeString.c_str());
}

void Locale::dump()
{
  for (int i = 1;; i++) {
    Il2CppString *str = localizeGet(i);
    if (str != nullptr && *str->start_char)
      set(str);
    else if (localizeGet(i + 1) == nullptr)
      break;
  }
}

void Locale::log(Il2CppString *str)
{
  auto hash = std::hash<std::wstring> {}(str->start_char);
  std::cout << hash << ": " << utf8(str) << std::endl;
}
