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

#include "qfv-module.h"

enum {
    PROP_0,
    PROP_FILENAME
};

static void qfv_module_finalize(GObject *object);
static void qfv_module_get_property(GObject *object, guint param_id,
    GValue *value, GParamSpec *pspec);
static void qfv_module_set_property(GObject *object, guint param_id,
    const GValue *value, GParamSpec *pspec);
static gboolean qfv_module_load_module(GTypeModule *gmodule);
static void qfv_module_unload_module(GTypeModule *gmodule);

G_DEFINE_TYPE(QfvModule, qfv_module, G_TYPE_TYPE_MODULE);

static void qfv_module_class_init(QfvModuleClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS(klass);

    object_class->finalize = qfv_module_finalize;
    object_class->get_property = qfv_module_get_property;
    object_class->set_property = qfv_module_set_property;

    type_module_class->load = qfv_module_load_module;
    type_module_class->unload = qfv_module_unload_module;

    g_object_class_install_property(object_class, PROP_FILENAME,
        g_param_spec_string("filename", "Filename",
        "The filename of the module", NULL, G_PARAM_READWRITE |
        G_PARAM_CONSTRUCT_ONLY));
}

static void qfv_module_init(QfvModule *module)
{
    module->filename = NULL;
    module->library = NULL;
    module->load = NULL;
    module->unload = NULL;
}

static void qfv_module_finalize(GObject *object)
{
    QfvModule *module = QFV_MODULE(object);

    g_free(module->filename);

    G_OBJECT_CLASS(qfv_module_parent_class)->finalize(object);
}

static void qfv_module_get_property(GObject *object, guint param_id,
    GValue *value, GParamSpec *pspec)
{
    QfvModule *module = QFV_MODULE(object);

    switch (param_id) {
        case PROP_FILENAME:
            g_value_set_string(value, module->filename);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
    }
}

static void qfv_module_set_property(GObject *object, guint param_id,
    const GValue *value, GParamSpec *pspec)
{
    QfvModule *module = QFV_MODULE(object);

    switch (param_id) {
        case PROP_FILENAME:
            g_free(module->filename);
            module->filename = g_value_dup_string(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
    }
}

static gboolean qfv_module_load_module(GTypeModule *gmodule)
{
    QfvModule *module = QFV_MODULE(gmodule);

    if (!module->filename) {
        g_warning("Module path not set");
        return FALSE;
    }

    module->library = g_module_open(module->filename, 0);

    if (!module->library) {
        g_printerr("%s\n", g_module_error());
        return FALSE;
    }

    if (!g_module_symbol(module->library, "qfv_module_load",
        (gpointer *) &module->load) ||
        !g_module_symbol(module->library, "qfv_module_unload",
        (gpointer *) &module->unload))
    {
        g_printerr("%s\n", g_module_error());
        g_module_close(module->library);
        return FALSE;
    }

    module->load(module);

    return TRUE;
}

static void qfv_module_unload_module(GTypeModule *gmodule)
{
    QfvModule *module = QFV_MODULE(gmodule);

    module->unload(module);

    g_module_close(module->library);
    module->library = NULL;

    module->load = NULL;
    module->unload = NULL;
}

extern QfvModule *qfv_module_new(const gchar *filename)
{
    g_return_val_if_fail(filename != NULL, NULL);

    return g_object_new(QFV_TYPE_MODULE, "filename", filename, NULL);
}

