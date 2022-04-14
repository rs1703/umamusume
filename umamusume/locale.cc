#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include <Windows.h>
#include <nlohmann/json.hpp>
#include <umamusume/locale.h>

Locale::Locale(const char *path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open locale file: " << path << std::endl;
    return;
  }

  nlohmann::json json;
  file >> json;

  for (auto &[key, value] : json.items())
    dictionary.insert({std::stoull(key), value.get<std::string>()});

  file.close();
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

void Locale::set(Il2CppString *str)
{
  if (str == nullptr)
    return;

  static std::mutex mutex;
  std::lock_guard lock(mutex);

  auto hash = std::hash<std::wstring> {}(str->start_char);
  if (has(hash))
    return;
  dictionary.insert({hash, utf8(str)});

  nlohmann::json json;
  for (auto &[key, value] : dictionary) {
    json[std::to_string(key)] = value;
  }

  std::ofstream file("locale.json", std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open locale file: locale.json" << std::endl;
    return;
  }

  file << json.dump(2);
  file.close();
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
  return dictionary.find(hash) != dictionary.end();
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

  if (!has(str)) {
    set(str);
    return str;
  }

  auto hash = std::hash<std::wstring> {}(str->start_char);
  auto localeString = get(hash);

  if (hash == std::hash<std::string> {}(localeString))
    return str;

  return il2cpp_string_new(localeString.c_str());
}