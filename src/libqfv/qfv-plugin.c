#include "qfv-plugin.h"

G_DEFINE_TYPE(QfvPlugin, qfv_plugin, G_TYPE_OBJECT);

static void qfv_plugin_class_init(QfvPluginClass *klass)
{
    /* ... */
}

static void qfv_plugin_init(QfvPlugin *plugin)
{
    /* ... */
}

QfvPlugin *qfv_plugin_new(GType type)
{
    g_return_val_if_fail(g_type_is_a(type, QFV_TYPE_PLUGIN), NULL);

    return g_object_new(type, NULL);
}

gboolean qfv_plugin_parse_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error)
{
    g_return_val_if_fail(QFV_IS_PLUGIN(plugin), FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);
    g_return_val_if_fail(list_store != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (QFV_PLUGIN_GET_CLASS(plugin)->parse_file)
        return QFV_PLUGIN_GET_CLASS(plugin)->parse_file(plugin, filename,
            list_store, error);

    g_warning("%s: plugin class %s doesn't implement QfvPlugin::parse_file()"
        "\n", G_STRFUNC, g_type_name(G_TYPE_FROM_INSTANCE(plugin)));
    /* implement an error */
    return FALSE;
}

gboolean qfv_plugin_hash_file(QfvPlugin *plugin, const gchar *filename,
    gchar **hash, GError **error)
{
    g_return_val_if_fail(QFV_IS_PLUGIN(plugin), FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);
    g_return_val_if_fail(hash != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (QFV_PLUGIN_GET_CLASS(plugin)->hash_file)
        return QFV_PLUGIN_GET_CLASS(plugin)->hash_file(plugin, filename,
            hash, error);

    g_warning("%s: plugin class %s doesn't implement QfvPlugin::hash_file()"
        "\n", G_STRFUNC, g_type_name(G_TYPE_FROM_INSTANCE(plugin)));

    return FALSE;
}

gboolean qfv_plugin_write_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error)
{
    g_return_val_if_fail(QFV_IS_PLUGIN(plugin), FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);
    g_return_val_if_fail(list_store != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (QFV_PLUGIN_GET_CLASS(plugin)->write_file)
        return QFV_PLUGIN_GET_CLASS(plugin)->write_file(plugin, filename,
            list_store, error);

    g_warning("%s: plugin class %s doesn't implement QfvPlugin::write_file()"
        "\n", G_STRFUNC, g_type_name(G_TYPE_FROM_INSTANCE(plugin)));

    return FALSE;
}

