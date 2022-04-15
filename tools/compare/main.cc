#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include <ShlObj_core.h>
#include <Windows.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

sqlite3 *originalDB;
sqlite3 *localizedDB;

bool initDatabase(const std::string &original, const std::string &localized)
{
  return sqlite3_open(original.c_str(), &originalDB) == SQLITE_OK
      && sqlite3_open(localized.c_str(), &localizedDB) == SQLITE_OK;
}

std::unordered_map<std::string, std::string> dumpTextData(sqlite3 *db)
{
  std::unordered_map<std::string, std::string> result;
  sqlite3_stmt *stmt;

  std::string query {"SELECT * FROM text_data"};
  if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    std::cerr << "Failed to prepare query: " << sqlite3_errmsg(db) << std::endl;
    return result;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *id = (const char *)sqlite3_column_text(stmt, 0);
    const char *index = (const char *)sqlite3_column_text(stmt, 2);
    const char *text = (const char *)sqlite3_column_text(stmt, 3);
    result[std::string(id) + ":" + std::string(index)] = text;
  }

  if (sqlite3_finalize(stmt) != SQLITE_OK) {
    std::cerr << "Failed to finalize query: " << sqlite3_errmsg(db) << std::endl;
    return result;
  }

  return result;
}

void extractLocalizedTextData()
{
  auto originalTextData = dumpTextData(originalDB);
  auto localizedTextData = dumpTextData(localizedDB);

  nlohmann::json json;
  for (auto &original : originalTextData) {
    auto localized = localizedTextData.find(original.first);
    if (localized == localizedTextData.end())
      continue;

    if (original.second != localized->second)
      json[original.first] = localized->second;
  }

  std::ofstream file("text_data.json");
  file << json.dump(2);
}

int main(int argc, char *argv[])
{
  TCHAR szPath[MAX_PATH];
  SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);

  std::filesystem::path dir(szPath);
  dir.remove_filename();
  dir += "LocalLow\\Cygames\\umamusume\\master";

  auto original = (dir / "master.mdb").string();
  auto localized = (dir / "master_localized.mdb").string();
  initDatabase(original, localized);

  extractLocalizedTextData();
}