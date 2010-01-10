#ifndef QFV_PLUGIN_H
#define QFV_PLUGIN_H

#include <glib.h>
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

typedef struct _QfvPlugin QfvPlugin;
typedef struct _QfvPluginClass QfvPluginClass;

struct _QfvPlugin {
    GObject parent_instance;
};

struct _QfvPluginClass {
    GObjectClass parent_class;

    const gchar *name;
    const gchar *description;

    gboolean (* parse_file) (QfvPlugin *plugin, const gchar *filename,
        GtkListStore *list_store, GError **error);
    gboolean (* hash_file) (QfvPlugin *plugin, const gchar *filename,
        gchar **hash, GError **error);
    gboolean (* write_file) (QfvPlugin *plugin, const gchar *filename,
        GtkListStore *list_store, GError **error);
};

GType qfv_plugin_get_type() G_GNUC_CONST;

gboolean qfv_plugin_parse_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error);
gboolean qfv_plugin_hash_file(QfvPlugin *plugin, const gchar *filename,
    gchar **hash, GError **error);
gboolean qfv_plugin_write_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error);

#endif

