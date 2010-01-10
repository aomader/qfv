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

#include "qfv-plugin.h"

#include <glib/gi18n.h>

G_DEFINE_TYPE(QfvPlugin, qfv_plugin, G_TYPE_OBJECT);

static void qfv_plugin_class_init(QfvPluginClass *klass)
{
    /* ... */
}

static void qfv_plugin_init(QfvPlugin *plugin)
{
    /* ... */
}

extern GQuark qfv_plugin_error_quark()
{
    return g_quark_from_static_string("qfv-plugin-error-quark");
}

extern QfvPlugin *qfv_plugin_new(GType type)
{
    g_return_val_if_fail(g_type_is_a(type, QFV_TYPE_PLUGIN), NULL);

    return g_object_new(type, NULL);
}

extern gboolean qfv_plugin_parse_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error)
{
    g_return_val_if_fail(QFV_IS_PLUGIN(plugin), FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);
    g_return_val_if_fail(list_store != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (QFV_PLUGIN_GET_CLASS(plugin)->parse_file)
        return QFV_PLUGIN_GET_CLASS(plugin)->parse_file(plugin, filename,
            list_store, error);

    g_set_error(error, QFV_PLUGIN_ERROR, QFV_PLUGIN_ERROR_IMPLEMENTATION,
        _("%s: plugin class %s doesn't implement QfvPlugin::parse_file()\n"),
        G_STRFUNC, g_type_name(G_TYPE_FROM_INSTANCE(plugin)));

    return FALSE;
}

extern gboolean qfv_plugin_hash_file(QfvPlugin *plugin, const gchar *filename,
    gchar **hash, GError **error)
{
    g_return_val_if_fail(QFV_IS_PLUGIN(plugin), FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);
    g_return_val_if_fail(hash != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (QFV_PLUGIN_GET_CLASS(plugin)->hash_file)
        return QFV_PLUGIN_GET_CLASS(plugin)->hash_file(plugin, filename,
            hash, error);

    g_set_error(error, QFV_PLUGIN_ERROR, QFV_PLUGIN_ERROR_IMPLEMENTATION,
        _("%s: plugin class %s doesn't implement QfvPlugin::hash_file()\n"),
        G_STRFUNC, g_type_name(G_TYPE_FROM_INSTANCE(plugin)));

    return FALSE;
}

extern gboolean qfv_plugin_write_file(QfvPlugin *plugin, const gchar *filename,
    GtkListStore *list_store, GError **error)
{
    g_return_val_if_fail(QFV_IS_PLUGIN(plugin), FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);
    g_return_val_if_fail(list_store != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (QFV_PLUGIN_GET_CLASS(plugin)->write_file)
        return QFV_PLUGIN_GET_CLASS(plugin)->write_file(plugin, filename,
            list_store, error);

    g_set_error(error, QFV_PLUGIN_ERROR, QFV_PLUGIN_ERROR_IMPLEMENTATION,
        _("%s: plugin class %s doesn't implement QfvPlugin::write_file()\n"),
        G_STRFUNC, g_type_name(G_TYPE_FROM_INSTANCE(plugin)));

    return FALSE;
}
