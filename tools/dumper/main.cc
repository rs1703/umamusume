#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <ShlObj_core.h>
#include <Windows.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

sqlite3 *db;

bool initDatabase()
{
  TCHAR szPath[MAX_PATH];
  SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);

  std::filesystem::path dbPath(szPath);
  dbPath.remove_filename();
  dbPath += "LocalLow\\Cygames\\umamusume\\master\\master.mdb";

  return sqlite3_open(dbPath.string().c_str(), &db) == SQLITE_OK;
}

void dump(const std::string &table, const std::string &column)
{
  std::cout << "Dumping " << table << "..." << std::endl;
  sqlite3_stmt *stmt;

  std::string query {"SELECT " + column + " FROM " + table};
  if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    std::cerr << "Failed to prepare query: " << sqlite3_errmsg(db) << std::endl;
    return;
  }

  nlohmann::json json;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *value = (const char *)sqlite3_column_text(stmt, 0);
    size_t hash = std::hash<std::string> {}(value);
    json[std::to_string(hash)] = value;
  }

  if (sqlite3_finalize(stmt) != SQLITE_OK) {
    std::cerr << "Failed to finalize query: " << sqlite3_errmsg(db) << std::endl;
    return;
  }

  std::ofstream file(table + ".json");
  file << json.dump(2);
}

int main(int argc, char *argv[])
{
  initDatabase();

  dump("text_data", "text");
  dump("character_system_text", "text");
  dump("race_jikkyo_comment", "message");
  dump("race_jikkyo_message", "message");
}