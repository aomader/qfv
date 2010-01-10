#include <qfv-module-manager.h>

enum {
    PROP_0,
    PROP_MODULE_PATH
};

static GObject *qfv_module_manager_constructor(GType type, guint n_params,
    GObjectConstructParam *params);
static void qfv_module_manager_finalize(GObject *object);
static void qfv_module_manager_get_property(GObject *object, guint param_id,
    GValue *value, GParamSpec *pspec);
static void qfv_module_manager_set_property(GObject *object, guint param_id,
    const GValue *value, GParamSpec *pspec);
static gboolean qfv_module_manager_query_modules(QfvModuleManager *manager);

G_DEFINE_TYPE(QfvModuleManager, qfv_module_manager, G_TYPE_OBJECT);

static void qfv_module_manager_class_init(QfvModuleManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->constructor = qfv_module_manager_constructor;
    object_class->finalize = qfv_module_manager_finalize;
    object_class->get_property = qfv_module_manager_get_property;
    object_class->set_property = qfv_module_manager_set_property;

    g_object_class_install_property(object_class, PROP_MODULE_PATH,
        g_param_spec_string("module-path", "Module path", "The path "
        "where to look for modules", NULL, G_PARAM_READWRITE |
        G_PARAM_CONSTRUCT_ONLY));
}

static void qfv_module_manager_init(QfvModuleManager *manager)
{
    manager->module_path = NULL;
    manager->modules = NULL;
}

static GObject *qfv_module_manager_constructor(GType type, guint n_params,
    GObjectConstructParam *params)
{
    GObject *object = G_OBJECT_CLASS(qfv_module_manager_parent_class)
        ->constructor(type, n_params, params);
    QfvModuleManager *manager = QFV_MODULE_MANAGER(object);

    if (manager->module_path)
        qfv_module_manager_query_modules(manager);

    return object;
}

static void qfv_module_manager_finalize(GObject *object)
{
    QfvModuleManager *manager = QFV_MODULE_MANAGER(object);

    g_free(manager->module_path);
    g_list_free(manager->modules);

    G_OBJECT_CLASS(qfv_module_manager_parent_class)->finalize(object);
}

static void qfv_module_manager_get_property(GObject *object, guint param_id,
    GValue *value, GParamSpec *pspec)
{
    QfvModuleManager *manager = QFV_MODULE_MANAGER(object);

    switch (param_id) {
        case PROP_MODULE_PATH:
            g_value_set_string(value, manager->module_path);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
    }
}

static void qfv_module_manager_set_property(GObject *object, guint param_id,
    const GValue *value, GParamSpec *pspec)
{
    QfvModuleManager *manager = QFV_MODULE_MANAGER(object);

    switch (param_id) {
        case PROP_MODULE_PATH:
            manager->module_path = g_value_dup_string(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
    }
}

static gboolean qfv_module_manager_query_modules(QfvModuleManager *manager)
{
    GError *error = NULL;
    GDir *dir = g_dir_open(manager->module_path, 0,  &error);

    if (dir == NULL) {
        g_printerr("Error while opening module dir: %s\n", error->message);
        g_clear_error(&error);
        return FALSE;
    }

    const gchar *name;
    while ((name = g_dir_read_name(dir)) != NULL) {
        if (g_str_has_suffix(name, ".so") == FALSE)
            continue;

        gchar *path = g_build_filename(manager->module_path, name, NULL);
        QfvModule *module = qfv_module_new(path);

        if (g_type_module_use(G_TYPE_MODULE(module)) == FALSE) {
            g_printerr("Failed to load module: %s\n", path);
            g_object_unref(module);
            g_free(path);
            continue;
        }

        g_free(path);

        g_type_module_unuse(G_TYPE_MODULE(module));

        manager->modules = g_list_prepend(manager->modules, module);
    }

    g_dir_close(dir);

    return TRUE;
}

extern QfvModuleManager *qfv_module_manager_new(const gchar *module_path)
{
    g_return_val_if_fail(module_path != NULL, NULL);

    return g_object_new(QFV_TYPE_MODULE_MANAGER, "module-path", module_path,
        NULL);
}

extern QfvPlugin *qfv_module_manager_find_plugin(const gchar *filename)
{
    g_return_val_if_fail(filename != NULL, NULL);

    guint n_plugins;
    GType *plugins = g_type_children(QFV_TYPE_PLUGIN, &n_plugins);
    QfvPlugin *plugin = NULL;

    for (int i = 0; i < n_plugins && plugin == NULL; ++i) {
        QfvPluginClass *klass = g_type_class_ref(plugins[i]);

        for (gchar **patterns = klass->filenames; *patterns != NULL;
            ++patterns)
        {
            if (g_pattern_match_simple(*patterns, filename) == TRUE) {
                plugin = qfv_plugin_new(plugins[i]);
                break;
            }
        }

        g_type_class_unref(klass);
    }

    g_free(plugins);

    return plugin;
}
