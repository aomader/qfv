#ifndef QFV_MODULE_MANAGER_H
#define QFV_MODULE_MANAGER_H

#include <glib-object.h>

#include <libqfv/qfv-module.h>
#include <libqfv/qfv-plugin.h>

#define QFV_TYPE_MODULE_MANAGER (qfv_module_manager_get_type())
#define QFV_MODULE_MANAGER(o) (G_TYPE_CHECK_INSTANCE_CAST((o), \
    QFV_TYPE_MODULE_MANAGER, QfvModuleManager))
#define QFV_MODULE_MANAGER_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), \
    QFV_TYPE_MODULE_MANAGER, QfvModuleManagerClass))
#define QFV_IS_MODULE_MANAGER(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), \
    QFV_TYPE_MODULE_MANAGER))
#define QFV_IS_MODULE_MANAGER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), \
    QFV_TYPE_MODULE_MANAGER))
#define QFV_MODULE_MANAGER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
    QFV_TYPE_MODULE_MANAGER, QfvModuleManagerClass))

typedef struct _QfvModuleManager QfvModuleManager;
typedef struct _QfvModuleManagerClass QfvModuleManagerClass;

struct _QfvModuleManager {
    GObject parent_instance;

    gchar *module_path;
    GList *modules;
};

struct _QfvModuleManagerClass {
    GObjectClass parent_class;
};

extern GType qfv_module_manager_get_type() G_GNUC_CONST;

extern QfvModuleManager *qfv_module_manager_new(const gchar *module_path);
extern QfvPlugin *qfv_module_manager_find_plugin(const gchar *filename);

#endif

