#include <iostream>
#include <mutex>

#include <umamusume/app.h>
#include <umamusume/il2cpp.h>
#include <umamusume/training_event.h>

Locale *locale = nullptr;
std::unordered_map<void *, bool> queries;
std::mutex queriesMutex;

assignDefaultFont_t assignDefaultFont;
getFontStyle_t getFontStyle;
setFontStyle_t setFontStyle;
getFontSize_t getFontSize;
setFontSize_t setFontSize;
getLineSpacing_t getLineSpacing;
setLineSpacing_t setLineSpacing;

setFps_t orig_setFps;
void setFps(int fps)
{
  orig_setFps(60);
}

populateWithErrors_t orig_populateWithErrors;
bool populateWithErrors(void *self, Il2CppString *str, TextGenerationSettings_t *settings, void *context)
{
  return orig_populateWithErrors(self, locale->localize(str), settings, context);
}

onPopulateMesh_t orig_onPopulateMesh;
void *onPopulateMesh(void *self, void *mesh)
{
  if (getLineSpacing(self) != 1.05f) {
    assignDefaultFont(self);
    setFontStyle(self, 1);
    setFontSize(self, getFontSize(self) - 4);
    setLineSpacing(self, 1.05f);
  }
  return orig_onPopulateMesh(self, mesh);
}

localizeGet_t orig_LocalizeGet;
Il2CppString *localizeGet(int id)
{
  return locale->localize(orig_LocalizeGet(id));
}

queryCtor_t orig_queryCtor;
void *queryCtor(void *self, void *conn, Il2CppString *sql)
{
  auto wsql = std::wstring_view(sql->start_char);
  if (wsql.find(L"text_data") != std::wstring_view::npos
      || wsql.find(L"character_system_text") != std::wstring_view::npos
      || wsql.find(L"race_jikkyo_comment") != std::wstring_view::npos
      || wsql.find(L"race_jikkyo_message") != std::wstring_view::npos)
  {
    std::unique_lock lock(queriesMutex);
    queries.emplace(self, true);
    lock.unlock();
  }
  return orig_queryCtor(self, conn, sql);
}

queryDispose_t orig_queryDispose;
void *queryDispose(void *self)
{
  if (queries.find(self) != queries.end())
    queries.erase(self);
  return orig_queryDispose(self);
}

queryGetText_t orig_queryGetText;
Il2CppString *queryGetText(void *self, int id)
{
  auto str = orig_queryGetText(self, id);

  std::unique_lock lock(queriesMutex);
  auto ok = queries.find(self) != queries.end();
  lock.unlock();

  if (ok)
    TrainingEventDictionary::log(str);
  return ok ? locale->localize(str) : str;
}