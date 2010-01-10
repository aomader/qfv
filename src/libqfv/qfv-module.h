#ifndef QFV_MODULE_H
#define QFV_MODULE_H

#include <glib-object.h>
#include <gmodule.h>

#define QFV_TYPE_MODULE (qfv_module_get_type())
#define QFV_MODULE(o) (G_TYPE_CHECK_INSTANCE_CAST((o), QFV_TYPE_MODULE, \
    QfvModule))
#define QFV_MODULE_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), QFV_TYPE_MODULE, \
    QfvModuleClass))
#define QFV_IS_MODULE(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), QFV_TYPE_MODULE))
#define QFV_IS_MODULE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), QFV_TYPE_MODULE))
#define QFV_MODULE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
    QFV_TYPE_MODULE, QfvModuleClass))

typedef struct _QfvModule QfvModule;
typedef struct _QfvModuleClass QfvModuleClass;

struct _QfvModule {
    GTypeModule parent_instance;

    gchar *filename;
    GModule *library;

    void (* load)(QfvModule *module);
    void (* unload)(QfvModule *module);
};

struct _QfvModuleClass {
    GTypeModuleClass parent_class;
};

GType qfv_module_get_type() G_GNUC_CONST;

QfvModule *qfv_module_new(const gchar *filename);

void qfv_module_load(QfvModule *module);
void qfv_module_unload(QfvModule *module);

#endif

