#include <umamusume/app.h>
#include <umamusume/il2cpp.h>

Locale *locale = nullptr;
std::unordered_map<void *, bool> queries;

assignDefaultFont_t assignDefaultFont;
getFontStyle_t getFontStyle;
setFontStyle_t setFontStyle;
getFontSize_t getFontSize;
setFontSize_t setFontSize;
getLineSpacing_t getLineSpacing;
setLineSpacing_t setLineSpacing;

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
  auto wsql = std::wstring(sql->start_char);
  if (wsql.find(L"text_data") != std::wstring::npos || wsql.find(L"character_system_text") != std::wstring::npos
      || wsql.find(L"race_jikkyo_comment") != std::string::npos
      || wsql.find(L"race_jikkyo_message") != std::string::npos)
  {
    queries.insert({self, true});
  }
  return orig_queryCtor(self, conn, sql);
}

queryDtor_t orig_queryDtor;
void *queryDtor(void *self)
{
  if (queries.find(self) != queries.end())
    queries.erase(self);
  return orig_queryDtor(self);
}

queryGetText_t orig_queryGetText;
Il2CppString *queryGetText(void *self, int id)
{
  auto str = orig_queryGetText(self, id);
  return queries.find(self) == queries.end() ? str : locale->localize(str);
}