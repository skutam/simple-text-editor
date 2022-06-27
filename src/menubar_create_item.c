#include "menubar_create_item.h"

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
) {
    // Create box to hold label and icon
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create icon and label
    GtkWidget *icon = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
    GtkWidget *label = gtk_accel_label_new(name);
    
    // Create menu item that will hold box
    GtkWidget *menu_item = gtk_menu_item_new();

    // Add icon to box, to be on the left
    gtk_container_add(GTK_CONTAINER(box), icon);

    gtk_label_set_use_underline(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);

    // Set only when accel_group is defined
    if (accel_group != NULL)
    {
        // Add shortcut
        gtk_widget_add_accelerator(
            menu_item,
            "activate",
            accel_group, 
            accel_key,
            accel_mods,
            GTK_ACCEL_VISIBLE
        );
    }
    
    gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(label), menu_item);

    gtk_box_pack_end(GTK_BOX(box), label, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(menu_item), box);

    return menu_item;
}

GtkWidget *create_item(
    const gchar *name,
    GtkAccelGroup *accel_group,
    guint accel_key,
    GdkModifierType accel_mods
) {
    // Create menu item with name
    GtkWidget *menu_item = gtk_menu_item_new_with_mnemonic(name);

    // Set only when accel_group is defined
    if (accel_group != NULL)
    {
        // Add shortcut
        gtk_widget_add_accelerator(
            menu_item,
            "activate",
            accel_group, 
            accel_key,
            accel_mods,
            GTK_ACCEL_VISIBLE
        );
    }
    
    return menu_item;
}