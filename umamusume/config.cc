#include <fstream>

#include <nlohmann/json.hpp>
#include <umamusume/config.h>

static const char *configPath = "config.json";
AppConfig *config = nullptr;

void AppConfig::load()
{
  if (config != nullptr)
    return;

  config = new AppConfig();
  config->showConsole = true;
  config->enableLocale = true;

  std::ifstream file(configPath);
  if (!file.good())
    return write();

  nlohmann::json json;
  file >> json;

  config->showConsole = json["showConsole"];
  config->enableLocale = json["enableLocale"];
  config->dictionaries = json["dictionaries"].get<std::vector<std::string>>();

  write();
}

void AppConfig::write()
{
  if (config == nullptr)
    return;

  nlohmann::json json;
  json["showConsole"] = config->showConsole;
  json["enableLocale"] = config->enableLocale;
  json["dictionaries"] = config->dictionaries;

  std::ofstream file(configPath);
  file << json.dump(2);
}