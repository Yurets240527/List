#ifndef VERIFYER_H
#define VERIFYER_H

#include <stdio.h>
#include "List.h"

typedef struct ErrInfo
{
    const char *file;

    int line;

    const char *func;

} ErrInfo;

void DotDump(const List *list, const char *filename);

FILE *HtmlBegin(const char *filename);
void HtmlEnd(FILE* f);

void HtmlDump_(FILE *f, const List *list, const char *png, const char *title, const ErrInfo *info);

static inline void HtmlDumpWrap(FILE *f, const List *list, const char *png,
                                const char *title, const char *file, int line, const char *func)
{
    ErrInfo info = {file, line, func};
    HtmlDump_(f, list, png, title, &info);
}

#define HtmlDump(f, list, png, title) \
    HtmlDumpWrap((f), (list), (png), (title), __FILE__, __LINE__, __func__)

Errors ListVerifyer(const List *list);

#endif