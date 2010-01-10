#include <stdio.h>

#include <qfv-module-manager.h>
#include <libqfv/qfv-plugin.h>

int main(int argc, const char *argv[])
{
    printf("lorn\n");
    QfvModuleManager *manager = g_object_new(QFV_TYPE_MODULE_MANAGER,
        "module-path", "/home/b52/Projekte/qfv/src/modules/.libs/", NULL);

    guint n_plugins;
    GType *plugins = g_type_children(QFV_TYPE_PLUGIN, &n_plugins);

    for (int i = 0; i < n_plugins; ++i) {
        QfvPluginClass *klass = g_type_class_ref(plugins[i]);

        printf("plugin: %s - %s\n", klass->name, klass->description);

        g_type_class_unref(klass);
    }

    g_free(plugins);

    return 0;
}
