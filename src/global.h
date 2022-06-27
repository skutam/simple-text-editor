#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <gtk/gtk.h>
#include <gtk/gtkenums.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>

#ifndef GDK_KEY_n
#define GDK_KEY_n GDK_n
#define GDK_KEY_o GDK_o
#define GDK_KEY_s GDK_s
#define GDK_KEY_q GDK_q
#define GDK_KEY_x GDK_x
#define GDK_KEY_c GDK_c
#define GDK_KEY_v GDK_v
#define GDK_KEY_Delete GDK_Delete
#define GDK_KEY_TAB GDK_Tab
#define GDK_KEY_F GDK_F
#define GDK_KEY_E GDK_E
#define GDK_KEY_a GDK_a
#endif

#define _(x) gettext (x)
#define MAX_OPEN_TABS 50
#define PROGRAM_NAME "Simple Text Editor"

struct TextEditor_File;

typedef struct
{
	gchar *filename;
	gchar *filepath;
	GtkTextBuffer *buffer;
	GtkWidget *vbox;
	GtkWidget *text_view;
	gboolean file_saved;
	gchar *search_text;
} TextEditor_File;

typedef struct {
	// File
	GtkWidget *file_save;
	GtkWidget *file_save_as;
	GtkWidget *file_print;

	// Edit
	GtkWidget *edit_cut;
	GtkWidget *edit_copy;
	GtkWidget *edit_paste;
	GtkWidget *edit_delete;
	GtkWidget *edit_select_all;

	// Search
	GtkWidget *search_find;
	GtkWidget *search_find_next;
	GtkWidget *search_find_previous;

	// Documents
	GtkWidget *documents_save_all;
	GtkWidget *documents_close_all;
	GtkWidget *documents_prev_document;
	GtkWidget *documents_next_document;
	

} TextEditor_Menubar;

typedef struct {
	GtkWidget *close;
	GtkWidget *save;
	GtkWidget *print;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;
	GtkWidget *search;
} TextEditor_Toolbar;

typedef struct
{
	GtkWidget *window;
	GtkAccelGroup *accel_group;

	// Vbox to hold MENUBAR, TOOLBAR, NOTEBOOK, STATUS BAR	
	GtkWidget *vbox;

	GtkWidget *menubar;
    GtkWidget *toolbar;

	GtkWidget *stack_frame;
	GtkWidget *vbox_stack;
	GtkWidget *scroll_w_stack_switcher;
	GtkWidget *stack_switcher;
	GtkWidget *stack;

	// Hbox holding bottom status bar with col and line index
	GtkWidget *hbox_statusbar;
	GtkWidget *statusbar;
	guint statusbar_context_id;
	GtkWidget *col_line_label;


	// Array of pointers to open files
	TextEditor_File *files[MAX_OPEN_TABS];
	gint new_file_index;

	TextEditor_Menubar menubar_struct;
	TextEditor_Toolbar toolbar_struct;
} TextEditor;

void error_message(TextEditor *editor, const gchar *message);

void info_message(TextEditor *editor, const gchar *message);

#endif