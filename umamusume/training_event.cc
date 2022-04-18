#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <umamusume/app.h>
#include <umamusume/config.h>
#include <umamusume/training_event.h>

std::unordered_map<size_t, std::vector<std::vector<std::string>>> TrainingEventDictionary::dictionary;

void TrainingEventDictionary::init()
{
  for (const auto &path : config->trainingEventDictionaries) {
    std::ifstream file(path);
    if (!file.good())
      continue;

    nlohmann::json json;
    file >> json;

    for (const auto &[key, value] : json.items())
      dictionary.emplace(std::stoull(key), value.get<std::vector<std::vector<std::string>>>());
  }
}

void TrainingEventDictionary::log(Il2CppString *str)
{
  if (str == nullptr)
    return;

  auto hash = std::hash<std::wstring> {}(str->start_char);
  auto it = dictionary.find(hash);
  if (it == dictionary.end())
    return;

  Locale::log(str);

  auto choices = it->second;
  for (size_t i = 0; i < choices.size(); i++) {
    std::cout << "[Choice " << i + 1 << "]" << std::endl;
    for (const auto &attr : choices[i]) {
      if (attr != "Randomly either" && attr != "or")
        std::cout << ">>";
      std::cout << attr << std::endl;
    }
  }
}