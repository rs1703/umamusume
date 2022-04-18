#ifndef UMAMUSUME_CONFIG_H_
#define UMAMUSUME_CONFIG_H_

#include <string>
#include <vector>

struct AppConfig
{
  bool showConsole;
  bool enableLocale;

  std::vector<std::string> dictionaries;
  std::vector<std::string> trainingEventDictionaries;

  static void load();
  static void write();
};

extern AppConfig *config;

#endif  // UMAMUSUME_CONFIG_H_