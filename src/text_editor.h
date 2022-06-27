#ifndef __TEXT_EDITOR_H__
#define __TEXT_EDITOR_H__

#include "global.h"
#include "menubar.h"
#include "toolbar.h"
#include "file_tabs.h"
#include "statusbar.h"
#include "menubar_documents.h"

#define MINIMAL_WIDTH 400
#define MINIMAL_HEIGHT 250

#define INIT_WINDOW_SIZE 0.6f

void init_app(TextEditor *editor);

#endif