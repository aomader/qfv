/**
 * This file is part of qfv.
 * Copyright (C) 2009,2010  Oliver Mader <dotb52@gmail.com>
 *
 * AUTHORS
 *     Oliver Mader <dotb52@gmail.com>
 *
 * Qfv is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Qfv is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qfv.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qfv-window.h"

#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>

#include <qfv-module-manager.h>
#include <libqfv/qfv-plugin.h>

#define QFV_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
    QFV_TYPE_WINDOW, QfvWindowPrivate))

struct _QfvWindowPrivate {
    GtkAccelGroup *accel_group;
    /* GtkListStore *list_store; */
    GtkWidget *tree_view;
    GdkPixbuf *icon_unknown;
    GdkPixbuf *icon_success;
    GdkPixbuf *icon_fail;
};

G_DEFINE_TYPE(QfvWindow, qfv_window, GTK_TYPE_WINDOW);
static void qfv_window_finalize(GObject *object);

static GtkWidget *qfv_window_create_menubar(QfvWindow *self);
/* static GtkWidget *qfv_window_create_toolbar(QfvWindow *self); */
static GtkWidget *qfv_window_create_treeview(QfvWindow *self);

G_MODULE_EXPORT void on_window_destroy(GtkObject *object, QfvWindow *window);
G_MODULE_EXPORT gint on_tree_view_sort_int(GtkTreeModel *model,
    GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);
G_MODULE_EXPORT gint on_tree_view_sort_string(GtkTreeModel *model,
    GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);
G_MODULE_EXPORT void on_menu_item_open_clicked(GtkMenuItem *menuitem,
    QfvWindow *window);
G_MODULE_EXPORT void on_menu_item_close_clicked(GtkMenuItem *menuitem,
    QfvWindow *window);
G_MODULE_EXPORT void on_menu_item_modules_clicked(GtkMenuItem *menuitem,
    QfvWindow *window);
G_MODULE_EXPORT void on_menu_item_about_clicked(GtkMenuItem *menuitem,
    QfvWindow *window);
G_MODULE_EXPORT void on_modules_response(GtkDialog *dialog, gint response_id,
    QfvWindow *window);
G_MODULE_EXPORT void on_about_response(GtkDialog *dialog, gint response_id,
    QfvWindow *window);
G_MODULE_EXPORT void on_about_link(GtkAboutDialog *about, const gchar *link_,
    gpointer user_data);

static void qfv_window_class_init(QfvWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = qfv_window_finalize;
    
    g_type_class_add_private(klass, sizeof(QfvWindowPrivate));
}

static void qfv_window_init(QfvWindow *self)
{
    self->about_dialog = NULL;
    self->modules_dialog = NULL;
    self->verify_dialog = NULL;
    self->current_dir = NULL;
    
    QfvWindowPrivate *priv = self->priv = QFV_WINDOW_GET_PRIVATE(self);
    self->priv->accel_group = gtk_accel_group_new();
    
    GError *error = NULL;
    
    if ((priv->icon_unknown = gdk_pixbuf_new_from_file("/home/b52/Projekte/qfv/data/unknown.png",
        &error)) == NULL)
    {
        g_warning(error->message);
        g_clear_error(&error);
    }
    
    if ((priv->icon_success = gdk_pixbuf_new_from_file("/home/b52/Projekte/qfv/data/success.png",
        &error)) == NULL)
    {
        g_warning(error->message);
        g_clear_error(&error);
    }
    
    if ((priv->icon_fail = gdk_pixbuf_new_from_file("/home/b52/Projekte/qfv/data/fail.png",
        &error)) == NULL)
    {
        g_warning(error->message);
        g_clear_error(&error);
    }
    
    gtk_window_add_accel_group(GTK_WINDOW(self), self->priv->accel_group);
    gtk_window_set_title(GTK_WINDOW(self), _("qfv"));
    gtk_window_set_default_size(GTK_WINDOW(self), 400, 600);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(self), vbox);

    GtkWidget *menubar = qfv_window_create_menubar(self);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
/*
    GtkWidget *toolbar = _qfv_window_create_toolbar(self);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
*/
    GtkWidget *treeview = qfv_window_create_treeview(self);
    gtk_box_pack_end(GTK_BOX(vbox), treeview, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(self), "destroy", G_CALLBACK(on_window_destroy),
        self);
    
    self->module_manager = qfv_module_manager_new(
        "/home/b52/Projekte/qfv/src/modules/.libs/");
}

static void qfv_window_finalize(GObject *object)
{
    QfvWindow *window = QFV_WINDOW(object);
    QfvWindowPrivate *priv = window->priv;

    g_object_unref(window->module_manager);
    
    g_object_unref(priv->icon_unknown);
    g_object_unref(priv->icon_success);
    g_object_unref(priv->icon_fail);
    
    g_free(window->current_dir);

    G_OBJECT_CLASS(qfv_window_parent_class)->finalize(object);
}

extern QfvWindow *qfv_window_new()
{
    return QFV_WINDOW(g_object_new(QFV_TYPE_WINDOW, NULL));
}

extern void qfv_window_error(QfvWindow *self, const gchar *format, ...)
{
    va_list args;
    va_start(args, format);
    
    gchar *message = g_strdup_vprintf(format, args);
    
    va_end(args);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
        message);
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy),
        dialog);
    gtk_widget_show_all(dialog);

    g_free(message);
}

extern void qfv_window_treeview_add(QfvWindow *self, const gchar *file,
    const gchar *hash)
{
    GtkTreeIter iter;
    gtk_list_store_append(self->list_store, &iter);
    gtk_list_store_set(self->list_store, &iter,
        QFV_WINDOW_TREEVIEW_COLUMN_STATUS, 0, QFV_WINDOW_TREEVIEW_COLUMN_ICON,
        self->priv->icon_unknown, QFV_WINDOW_TREEVIEW_COLUMN_FILE, file,
        QFV_WINDOW_TREEVIEW_COLUMN_HASH, hash, -1);
}

extern void qfv_window_treeview_update(QfvWindow *self, GtkTreeIter *iter,
    QfvWindowTreeviewStatus status, const gchar *description)
{
    GdkPixbuf *icon;
    
    switch (status) {
        case QFV_WINDOW_TREEVIEW_STATUS_ERROR:
            icon = self->priv->icon_fail;
            break;
        case QFV_WINDOW_TREEVIEW_STATUS_UNKNOWN:
            icon = self->priv->icon_unknown;
            break;
        case QFV_WINDOW_TREEVIEW_STATUS_OK:
            icon = self->priv->icon_success;
            break;
    }
    
    gtk_list_store_set(self->list_store, iter,
        QFV_WINDOW_TREEVIEW_COLUMN_STATUS, status,
        QFV_WINDOW_TREEVIEW_COLUMN_ICON, icon,
        QFV_WINDOW_TREEVIEW_COLUMN_DESCRIPTION, description, -1);
}

extern void qfv_window_treeview_clear(QfvWindow *self)
{
    gtk_list_store_clear(self->list_store);
}

extern void qfv_window_title_clear(QfvWindow *self)
{
    gtk_window_set_title(GTK_WINDOW(self), _("qfv"));
}

extern void qfv_window_title_update(QfvWindow *self, const gchar *filename)
{
    gchar *base = g_filename_display_basename(filename);
    gchar *title = g_strconcat(base, _(" - qfv"), NULL);
    
    gtk_window_set_title(GTK_WINDOW(self), title);
    
    g_free(title);
    g_free(base);
}

static GtkWidget *qfv_window_create_menubar(QfvWindow *self)
{
    GtkWidget *bar = gtk_menu_bar_new();
    
    GtkWidget *file = gtk_menu_item_new_with_mnemonic(_("_File"));
    GtkWidget *file_menu = gtk_menu_new();
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
    
    GtkRecentFilter *filter = gtk_recent_filter_new();
    gtk_recent_filter_add_application(filter, "qfv");

    GtkWidget *recent = gtk_recent_chooser_menu_new();
    gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent),
        GTK_RECENT_SORT_MRU);
    gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent), 10);
    gtk_recent_chooser_add_filter(GTK_RECENT_CHOOSER(recent), filter);

    GtkWidget *open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
    GtkWidget *opened = gtk_menu_item_new_with_label(_("Recently opened ..."));
    GtkWidget *separator3 = gtk_separator_menu_item_new();
    GtkWidget *close = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE,
        NULL);
    GtkWidget *separator = gtk_separator_menu_item_new();
    GtkWidget *quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(opened), recent);
    
    gtk_widget_add_accelerator(open, "activate", self->priv->accel_group,
        GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(close, "activate", self->priv->accel_group,
        GDK_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(quit, "activate", self->priv->accel_group,
        GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    
    g_signal_connect(open, "activate", G_CALLBACK(on_menu_item_open_clicked),
        self);
    g_signal_connect(close, "activate", G_CALLBACK(on_menu_item_close_clicked),
        self);
    g_signal_connect(quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), opened);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator3);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), close);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file);
    
    GtkWidget *help = gtk_menu_item_new_with_mnemonic(_("_Help"));
    GtkWidget *help_menu = gtk_menu_new();
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), help_menu);
    
    GtkWidget *modules = gtk_menu_item_new_with_mnemonic(_("_Modules"));
    GtkWidget *separator2 = gtk_separator_menu_item_new();
    GtkWidget *about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,
        NULL);
    
    g_signal_connect(modules, "activate",
        G_CALLBACK(on_menu_item_modules_clicked), self);
    g_signal_connect(about, "activate", G_CALLBACK(on_menu_item_about_clicked),
        self);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), modules);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), help);
    
    return bar;
}
/*
static GtkWidget *qfv_window_create_toolbar(QfvWindow *self)
{
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

    GtkToolItem *open = gtk_menu_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, -1);

    g_signal_connect(G_OBJECT(open), "clicked", G_CALLBACK(
        on_toolbar_open_clicked), self);

    GtkRecentFilter *filter = gtk_recent_filter_new();
    gtk_recent_filter_add_application(filter, "qfv");

    GtkWidget *recent = gtk_recent_chooser_menu_new();
    gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent),
        GTK_RECENT_SORT_MRU);
    gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent), 10);
    gtk_recent_chooser_add_filter(GTK_RECENT_CHOOSER(recent), filter);

    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(open), recent);

    g_signal_connect(G_OBJECT(recent), "item-activated", G_CALLBACK(
        on_toolbar_open_item), self);

    return toolbar;
}
*/
static GtkWidget *qfv_window_create_treeview(QfvWindow *self)
{
    QfvWindowPrivate *priv = self->priv;
    self->list_store = gtk_list_store_new(5, G_TYPE_INT, GDK_TYPE_PIXBUF,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    priv->tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(
        self->list_store));
    
    GtkTreeView *tree_view = GTK_TREE_VIEW(priv->tree_view);
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
    
    gtk_tree_view_set_rules_hint(tree_view, TRUE);
    gtk_tree_view_set_rubber_banding(tree_view, TRUE);
    gtk_tree_view_set_headers_clickable(tree_view, TRUE);
    
    GtkCellRenderer *renderer_icon = gtk_cell_renderer_pixbuf_new();
    GtkCellRenderer *renderer_file = gtk_cell_renderer_text_new();
    GtkCellRenderer *renderer_hash = gtk_cell_renderer_text_new();
    GtkCellRenderer *renderer_desc = gtk_cell_renderer_text_new();
    
    GtkTreeViewColumn *column_icon = gtk_tree_view_column_new_with_attributes(
        "", renderer_icon, "pixbuf", QFV_WINDOW_TREEVIEW_COLUMN_ICON, NULL);
    GtkTreeViewColumn *column_file = gtk_tree_view_column_new_with_attributes(
        _("File"), renderer_file, "text", QFV_WINDOW_TREEVIEW_COLUMN_FILE,
        NULL);
    GtkTreeViewColumn *column_hash = gtk_tree_view_column_new_with_attributes(
        _("Hash"), renderer_hash, "text", QFV_WINDOW_TREEVIEW_COLUMN_HASH,
        NULL);
    GtkTreeViewColumn *column_desc = gtk_tree_view_column_new_with_attributes(
        _("Description"), renderer_desc, "text",
        QFV_WINDOW_TREEVIEW_COLUMN_DESCRIPTION, NULL);
    
    gtk_tree_view_column_set_expand(column_file, TRUE);
    gtk_tree_view_column_set_expand(column_hash, TRUE);
    gtk_tree_view_column_set_expand(column_desc, TRUE);
    
    gtk_tree_view_column_set_resizable(column_icon, TRUE);
    gtk_tree_view_column_set_resizable(column_file, TRUE);
    gtk_tree_view_column_set_resizable(column_hash, TRUE);
    gtk_tree_view_column_set_resizable(column_desc, TRUE);
    
    gtk_tree_view_column_set_sort_column_id(column_icon,
        QFV_WINDOW_TREEVIEW_COLUMN_STATUS);
    gtk_tree_view_column_set_sort_column_id(column_file,
        QFV_WINDOW_TREEVIEW_COLUMN_FILE);
    gtk_tree_view_column_set_sort_column_id(column_hash,
        QFV_WINDOW_TREEVIEW_COLUMN_HASH);
    gtk_tree_view_column_set_sort_column_id(column_desc,
        QFV_WINDOW_TREEVIEW_COLUMN_DESCRIPTION);
    
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(self->list_store),
        QFV_WINDOW_TREEVIEW_COLUMN_STATUS, on_tree_view_sort_int, NULL, NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(self->list_store),
        QFV_WINDOW_TREEVIEW_COLUMN_FILE, on_tree_view_sort_string,
        GINT_TO_POINTER(QFV_WINDOW_TREEVIEW_COLUMN_FILE), NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(self->list_store),
        QFV_WINDOW_TREEVIEW_COLUMN_HASH, on_tree_view_sort_string,
        GINT_TO_POINTER(QFV_WINDOW_TREEVIEW_COLUMN_HASH), NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(self->list_store),
        QFV_WINDOW_TREEVIEW_COLUMN_DESCRIPTION, on_tree_view_sort_string,
        GINT_TO_POINTER(QFV_WINDOW_TREEVIEW_COLUMN_DESCRIPTION), NULL);
    
    gtk_tree_view_append_column(tree_view, column_icon);
    gtk_tree_view_append_column(tree_view, column_file);
    gtk_tree_view_append_column(tree_view, column_hash);
    gtk_tree_view_append_column(tree_view, column_desc);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    gtk_container_add(GTK_CONTAINER(scrolled), priv->tree_view);

    return scrolled;
}

G_MODULE_EXPORT void on_window_destroy(GtkObject *object, QfvWindow *window)
{
    gtk_main_quit();
}

G_MODULE_EXPORT gint on_tree_view_sort_int(GtkTreeModel *model,
    GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
    gint int1 = 0, int2 = 0;
    
    gtk_tree_model_get(model, a, QFV_WINDOW_TREEVIEW_COLUMN_STATUS, &int1, -1);
    gtk_tree_model_get(model, b, QFV_WINDOW_TREEVIEW_COLUMN_STATUS, &int2, -1);
    
    if (int1 == int2)
        return 0;
    
    return (int1 < int2) ? -1 : 1;
}

G_MODULE_EXPORT gint on_tree_view_sort_string(GtkTreeModel *model,
    GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
    gchar *string1, *string2;
    gint data = GPOINTER_TO_INT(user_data), ret = 0;

    gtk_tree_model_get(model, a, data, &string1, -1);
    gtk_tree_model_get(model, b, data, &string2, -1);
    
    if (string1 == NULL || string2 == NULL) {
        if (string1 == NULL && string2 == NULL)
            ret = 0;
        else
            ret = (string1 == NULL) ? -1 : 1;
    } else {
        ret = g_utf8_collate(string1, string2);
    }
    
    g_free(string1);
    g_free(string2);

    return ret;
}

G_MODULE_EXPORT void on_menu_item_open_clicked(GtkMenuItem *menuitem,
    QfvWindow *window)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Open File"),
        GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
        GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
            dialog));
        
        if (G_LIKELY(filename != NULL)) {
         /*   qfv_parse_file(filename, window);*/
            QfvPlugin *plugin = qfv_module_manager_find_plugin(filename);

            if (plugin != NULL) {
                printf("found plugin!\n");
                qfv_plugin_parse_file(plugin, filename, window->list_store, NULL);

                qfv_plugin_write_file(plugin, "/home/b52/testmd5", window->list_store, NULL);
            }
            g_free(filename);
        }
    }
    
    gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void on_menu_item_close_clicked(GtkMenuItem *menuitem,
    QfvWindow *window)
{
    qfv_window_title_clear(window);
    qfv_window_treeview_clear(window);
    
    if (window->current_dir != NULL) {
        g_free(window->current_dir);
        window->current_dir = NULL;
    }
    
    window->current_module = NULL;
}

G_MODULE_EXPORT void on_menu_item_modules_clicked(GtkMenuItem *menuitem,
    QfvWindow *window)
{ /*
    if (window->modules_dialog != NULL) {
        gtk_window_present(GTK_WINDOW(window->modules_dialog));
        return;
    }
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons(_("Modules"), NULL, 0,
        GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkTreeIter iter;
    GtkListStore *list_store = gtk_list_store_new(2, G_TYPE_STRING,
        G_TYPE_STRING);
    
    for (GSList *item = window->module_manager->modules; item != NULL; item =
        item->next)
    {
        QfvModule *module = QFV_MODULE(item->data);
        
        gchar *name = g_strdup_printf("<b>%s %s</b>\n%s", module->name,
            module->version, module->description);
        gchar *author = g_strdup_printf("%s\n%s", module->author,
            module->website);
        
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter, 0, name, 1, author, -1);
        
        g_free(name);
        g_free(author);
    }
    
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(
        list_store));
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        _("Module"), renderer, "markup", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Author"), renderer,
        "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    gtk_container_add(GTK_CONTAINER(content), tree_view);

    g_signal_connect(dialog, "response", G_CALLBACK(on_modules_response),
        window);

    window->modules_dialog = dialog;
    gtk_widget_show_all(dialog);
    */
}

G_MODULE_EXPORT void on_menu_item_about_clicked(GtkMenuItem *menuitem,
    QfvWindow *window)
{
    static const gchar *authors[2] = {
        "Oliver Mader  <dotb52@gmail.com>",
        NULL
    };
    
    static const gchar *artists[2] = {
        "Mark James  <mjames@gmail.com>",
        NULL
    };
    
    if (window->about_dialog != NULL) {
        gtk_window_present(GTK_WINDOW(window->about_dialog));
        return;
    }
    
    GtkAboutDialog *dialog = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
    gtk_about_dialog_set_program_name(dialog, _("qfv"));
    gtk_about_dialog_set_version(dialog, VERSION);
    gtk_about_dialog_set_copyright(dialog, _("Copyright (C) 2009  Oliver "
        "Mader"));
    gtk_about_dialog_set_license(dialog, _("Qfv is free software: you can "
        "redistribute it and/or modify it under the terms of the GNU General "
        "Public License as published by the Free Software Foundation, either "
        "version 3 of the License, or (at your option) any later version.\n\n"
        "Qfv is distributed in the hope that it will be useful, but WITHOUT "
        "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
        "FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License "
        "for more details.\n\nYou should have received a copy of the GNU "
        "General Public License along with qfv. If not, see "
        "<http://www.gnu.org/licenses/>."));
    gtk_about_dialog_set_wrap_license(dialog, TRUE);
    gtk_about_dialog_set_url_hook(on_about_link, window, NULL);
    gtk_about_dialog_set_website(dialog, "http://qfv.sourceforge.net");
    gtk_about_dialog_set_authors(dialog, authors);
    gtk_about_dialog_set_artists(dialog, artists);
    gtk_about_dialog_set_translator_credits(dialog, _("translator-credits"));

    g_signal_connect(dialog, "response", G_CALLBACK(on_about_response),
        window);
    
    window->about_dialog = GTK_WIDGET(dialog);
    gtk_widget_show_all(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_modules_response(GtkDialog *dialog, gint response_id,
    QfvWindow *window)
{
    gtk_widget_destroy(GTK_WIDGET(dialog));
    window->modules_dialog = NULL;
}

G_MODULE_EXPORT void on_about_response(GtkDialog *dialog, gint response_id,
    QfvWindow *window)
{
    gtk_widget_destroy(GTK_WIDGET(dialog));
    window->about_dialog = NULL;
}

G_MODULE_EXPORT void on_about_link(GtkAboutDialog *about, const gchar *link_,
    gpointer user_data)
{
    GError *error = NULL;
    
    if (gtk_show_uri(NULL, link_, GDK_CURRENT_TIME, &error) == FALSE) {
        g_warning(error->message);
        g_error_free(error);
    }
}
