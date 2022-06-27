#ifndef __MENUBAR_CREATE_ITEM_H__
#define __MENUBAR_CREATE_ITEM_H__

#include "global.h"

/**
 * Create menu item with name, icon and accel_group
 * @ref https://valadoc.org/gtk+-3.0/Gtk.ImageMenuItem.html
 * */
GtkWidget *create_icon_item(
    const gchar *name,
    const gchar *icon_name,
    GtkAccelGroup *accel_group,
    guint accel_key,
    GdkModifierType accel_mods
);

GtkWidget *create_item(
    const gchar *name,
    GtkAccelGroup *accel_group,
    guint accel_key,
    GdkModifierType accel_mods
);


#endif