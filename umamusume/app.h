#ifndef UMAMUSUME_APP_H_
#define UMAMUSUME_APP_H_

#include <umamusume/il2cpp.h>
#include <umamusume/locale.h>

struct Config
{
  std::string localePath;
  bool showConsole;
};

typedef void (*assignDefaultFont_t)(void *);
typedef int (*getFontStyle_t)(void *);
typedef void (*setFontStyle_t)(void *, int);
typedef int (*getFontSize_t)(void *);
typedef void (*setFontSize_t)(void *, int);
typedef float (*getLineSpacing_t)(void *);
typedef void (*setLineSpacing_t)(void *, float);
typedef void (*setFps_t)(int);

typedef bool *(*populateWithErrors_t)(void *, Il2CppString *, TextGenerationSettings_t *, void *);
typedef void *(*onPopulateMesh_t)(void *, void *);
typedef Il2CppString *(*localizeGet_t)(int);
typedef void *(*queryCtor_t)(void *, void *, Il2CppString *);
typedef void *(*queryDispose_t)(void *);
typedef Il2CppString *(*queryGetText_t)(void *, int);

extern Locale *locale;
extern std::unordered_map<void *, bool> queries;

extern assignDefaultFont_t assignDefaultFont;
extern getFontStyle_t getFontStyle;
extern setFontStyle_t setFontStyle;
extern getFontSize_t getFontSize;
extern setFontSize_t setFontSize;
extern getLineSpacing_t getLineSpacing;
extern setLineSpacing_t setLineSpacing;

extern setFps_t orig_setFps;
extern void setFps(int fps);

extern populateWithErrors_t orig_populateWithErrors;
extern bool populateWithErrors(void *self, Il2CppString *str, TextGenerationSettings_t *settings, void *context);

extern onPopulateMesh_t orig_onPopulateMesh;
extern void *onPopulateMesh(void *self, void *mesh);

extern localizeGet_t orig_LocalizeGet;
extern Il2CppString *localizeGet(int id);

extern queryCtor_t orig_queryCtor;
extern void *queryCtor(void *self, void *conn, Il2CppString *sql);

extern queryDispose_t orig_queryDispose;
extern void *queryDispose(void *self);

extern queryGetText_t orig_queryGetText;
extern Il2CppString *queryGetText(void *self, int id);

#endif  // UMAMUSUME_APP_H_