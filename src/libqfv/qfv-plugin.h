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

#ifndef QFV_PLUGIN_H
#define QFV_PLUGIN_H

#include <glib-object.h>
#include <gtk/gtk.h>

#define QFV_TYPE_PLUGIN (qfv_plugin_get_type())
#define QFV_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_CAST((o), QFV_TYPE_PLUGIN, \
    QfvPlugin))
#define QFV_PLUGIN_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), QFV_TYPE_PLUGIN, \
    QfvPluginClass))
#define QFV_IS_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), QFV_TYPE_PLUGIN))
#define QFV_IS_PLUGIN_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), QFV_TYPE_PLUGIN))
#define QFV_PLUGIN_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
    QFV_TYPE_PLUGIN, QfvPluginClass))
#define QFV_PLUGIN_ERROR (qfv_plugin_error_quark())

typedef enum {
    QFV_PLUGIN_ERROR_IMPLEMENTATION
} QfvPluginError;

typedef struct _QfvPlugin QfvPlugin;
typedef struct _QfvPluginClass QfvPluginClass;

struct _QfvPlugin {
    GObject parent_instance;
};

struct _QfvPluginClass {
    GObjectClass parent_class;

    const gchar *name;
    const gchar *description;
    const gchar **filenames;

    gboolean (* parse_file) (QfvPlugin *plugin, const gchar *filename,
        GtkListStore *list_store, GError **error);
    gboolean (* hash_file) (QfvPlugin *plugin, const gchar *filename,
        gchar **hash, GError **error);
    gboolean (* write_file) (QfvPlugin *plugin, const gchar *filename,
        GtkListStore *list_store, GError **error);
};

extern GType qfv_plugin_get_type() G_GNUC_CONST;
extern GQuark qfv_plugin_error_quark();

extern QfvPlugin *qfv_plugin_new(GType type);
extern gboolean qfv_plugin_parse_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error);
extern gboolean qfv_plugin_hash_file(QfvPlugin *plugin, const gchar *filename,
    gchar **hash, GError **error);
extern gboolean qfv_plugin_write_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error);

#endif

