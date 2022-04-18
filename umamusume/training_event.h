#ifndef UMAMUSUME_TRAINING_EVENT_H_
#define UMAMUSUME_TRAINING_EVENT_H_

#include <unordered_map>
#include <vector>

#include <umamusume/il2cpp.h>

class TrainingEventDictionary
{
  static std::unordered_map<size_t, std::vector<std::vector<std::string>>> dictionary;

public:
  static void init();
  static void log(Il2CppString *str);
};

#endif UMAMUSUME_TRAINING_EVENT_H_