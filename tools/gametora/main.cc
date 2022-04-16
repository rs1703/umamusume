
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <winhttp.h>

static const std::string baseUrl {"https://gametora.com/_next/data/g9xqZgJg2ebtmmgiafEOi"};

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
std::string sendRequest(const std::string &url);

std::string getLocalLastUpdated()
{
  std::ifstream file("gametora.txt");
  if (!file.is_open())
    return "";

  std::stringstream result;
  result << file.rdbuf();
  return result.str();
}

void setLocalLastUpdated(const std::string &lastUpdated)
{
  std::ofstream file("gametora.txt");
  file << lastUpdated;
}

std::string getLastUpdated()
{
  std::string res = sendRequest(baseUrl + "/umamusume.json");
  if (res.empty()) {
    std::cerr << "Unable to get last updated" << std::endl;
    return "";
  }

  nlohmann::json json = nlohmann::json::parse(res);
  auto pageProps = json["pageProps"];
  if (pageProps.is_null()) {
    std::cerr << "Unable to get last updated, pageProps is null" << std::endl;
    return "";
  }

  auto changelogs = pageProps["changelog"];
  if (changelogs.is_null() || changelogs.size() == 0)
    return "";

  return changelogs[0]["date"].get<std::string>();
}

typedef std::vector<std::vector<std::string>> Event_t;
typedef std::unordered_map<std::string, Event_t> Events_t;

struct EntityData
{
  std::string name;
  Events_t events;
};

static std::unordered_map<std::string, std::string> types;

const std::string TYPE_BO {"bo"};
const std::string TYPE_CT {"ct"};
const std::string TYPE_DI {"di"};
const std::string TYPE_NO {"no"};
const std::string TYPE_SE {"se"};
const std::string TYPE_SK {"sk"};
const std::string TYPE_SG {"sg"};
const std::string TYPE_SR {"sr"};

void initTypes()
{
  types.emplace("en", "Energy");
  types.emplace("sp", "Speed");
  types.emplace("st", "Stamina");
  types.emplace("po", "Power");
  types.emplace("gu", "Guts");
  types.emplace("in", "Intelligence");
  types.emplace("fe", "Full energy recovery");
  types.emplace("mo", "Motivation");
  types.emplace("pt", "Skill point(s)");
  types.emplace("no", "Nothing happens");
  types.emplace("5s", "All stats");
  types.emplace("rs", "Random stat(s)");
  types.emplace("ls", "Last trained stat");
  types.emplace("ds", "Can start dating");
  types.emplace("ee", "Event chain ended");
  types.emplace("me", "Maximum energy");
  types.emplace("he", "Heal a negative status effect");
}

static std::unordered_map<int, std::string> statuses;

void initStatuses()
{
  statuses.emplace(2, "Get 'Lazy Habit' status (Has a random chance to not show up for training)");
  statuses.emplace(4, "Get 'Overweight' status (Speed cannot be raised through training)");
  statuses.emplace(7, "Get 'Sharp' status (The SP cost of all skills is discounted by 10%)");
  statuses.emplace(8, "Get 'Charming ○' status (Bond gain with support cards is increased by 2)");
  statuses.emplace(9, "Get 'Rising Star' status (Bond gain with NPCs is increased by 2)");
  statuses.emplace(10, "Get 'Good Practice ○' status (Training fail rate is decreased by 2%)");
}

void parseEvent(const nlohmann::json &json, Events_t &events)
{
  const auto &choices = json["c"];
  if (choices.is_null())
    return;

  const auto name = json["n"].get<std::string>();
  for (size_t i = 0; i < choices.size(); ++i) {
    const auto &attrs = choices[i]["r"];
    if (attrs.is_null())
      continue;

    std::vector<std::string> event;
    bool di = false;  // is or

    for (const auto &attr : attrs) {
      if (!attr.contains("t"))
        continue;

      const auto type = attr["t"].get<std::string>();
      if (types.find(type) == types.end() && type != TYPE_BO && type != TYPE_CT && type != TYPE_DI && type != TYPE_SE
          && type != TYPE_SK && type != TYPE_SG && type != TYPE_SR)
      {
        printf("Unknown attribute: %s\n", type.c_str());
        continue;
      }

      const bool isSkillEffect = type == TYPE_SE;

      std::string value;
      if (!isSkillEffect && attr.contains("v")) {
        auto v = attr["v"];
        if (!v.is_null())
          value = v.get<std::string>();
      }

      bool isRandom = false;
      if (attr.contains("r")) {
        auto v = attr["r"];
        if (!v.is_null())
          isRandom = v.get<bool>();
      }

      std::string random;
      if (isRandom)
        random = "(Random) ";

      if (type == TYPE_BO) {
        // TODO: parse bond
      } else if (type == TYPE_CT) {
        if (!attr.contains("d"))
          continue;

        auto v = attr["d"];
        if (v.is_null())
          continue;

        event.push_back(random + v.get<std::string>());
      } else if (type == TYPE_DI) {
        if (!di) {
          di = true;
          event.insert(event.begin(), "Randomly either");
        }
        event.push_back("or");
      } else if (type == TYPE_SE) {
        if (!attr.contains("d"))
          continue;

        auto v = attr["d"];
        if (v.is_null())
          continue;

        auto d = v.get<int>();
        if (statuses.find(d) == statuses.end()) {
          printf("Unknown status: %d | %s\n", d, name.c_str());
          continue;
        }

        event.push_back(random + statuses[d]);
      } else if (type == TYPE_SK) {
        // TODO: parse skill (hint)
      } else if (type == TYPE_SG) {
        // TODO: parse skill (skill get)
      } else if (type == TYPE_SR) {
        // TODO: parse skill (multiple hints)
      } else if (value.empty()) {
        event.push_back(random + types[type]);
      } else {
        std::string count;
        if (attr.contains("d")) {
          auto v = attr["d"];
          if (!v.is_null())
            count = std::to_string(v.get<int>()) + " ";
        }
        event.push_back(random + count + types[type] + " " + value);
      }
    }

    if (!event.empty())
      events[name].push_back(event);
  }
}

EntityData *parseEntity(std::string path, const std::vector<std::string> &fields)
{
  std::cout << "Parsing " << path << std::endl;
  const std::string res = sendRequest(baseUrl + path);
  if (res.empty()) {
    std::cerr << "Unable to get " << path << std::endl;
    return nullptr;
  }

  nlohmann::json json = nlohmann::json::parse(res);
  auto pageProps = json["pageProps"];
  if (pageProps.is_null()) {
    std::cerr << "Unable to get " << path << ", pageProps is null" << std::endl;
    return nullptr;
  }

  const auto eventData = pageProps["eventData"];
  if (eventData.is_null()) {
    std::cerr << "Unable to get " << path << ", eventData is null" << std::endl;
    return nullptr;
  }

  const auto itemData = pageProps["itemData"];
  if (itemData.is_null()) {
    std::cerr << "Unable to get " << path << ", itemData is null" << std::endl;
    return nullptr;
  }

  const auto entityData = new EntityData;
  if (itemData.contains("name_jp"))
    entityData->name = itemData["name_jp"].get<std::string>();
  else
    entityData->name = itemData["char_name"].get<std::string>();

  auto eventsJson = nlohmann::json::parse(eventData.get<std::string>());

  for (const auto &field : fields) {
    const auto &events = eventsJson[field];
    if (!events.is_null()) {
      for (const auto &event : events)
        parseEvent(event, entityData->events);
    }
  }

  return entityData;
}

void fetch(const char *type, const char *root, std::vector<std::string> fields)
{
  const std::string sType {type};

  printf("Fetching %s\n", type);
  std::string res = sendRequest(baseUrl + "/umamusume/" + type + ".json");
  if (res.empty()) {
    printf("Unable to get %s\n", type);
    return;
  }

  nlohmann::json json = nlohmann::json::parse(res);
  auto pageProps = json["pageProps"];
  if (pageProps.is_null()) {
    printf("Unable to get %s, pageProps is null\n", type);
    return;
  }

  auto entities = pageProps[root];
  if (entities.is_null()) {
    printf("Unable to get %s, %s is null\n", type, root);
    return;
  }

  nlohmann::json data;
  for (const auto &entity : entities) {
    auto name = entity["url_name"];
    if (name.is_null())
      continue;

    auto entityData = parseEntity("/umamusume/" + sType + "/" + name.get<std::string>() + ".json", fields);
    if (entityData != nullptr)
      data[entityData->name] = entityData->events;
  }

  std::ofstream file("training_event_" + sType + ".json", std::ios::trunc);
  file << data.dump(2);
}

int main(int argc, char *argv[])
{
  std::string localLastUpdated = getLocalLastUpdated();
  std::string lastUpdated = getLastUpdated();

  if (localLastUpdated == lastUpdated)
    return 0;

  initTypes();
  initStatuses();

  fetch("characters", "characterData", {"outings", "version", "wchoice"});
  fetch("supports", "supportData", {"arrows", "random"});

  setLocalLastUpdated(lastUpdated);
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string sendRequest(const std::string &url)
{
  CURL *curl = curl_easy_init();
  if (!curl)
    return NULL;

  std::string readBuffer;

  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  return readBuffer;
}