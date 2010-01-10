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

    void (* load) (QfvModule *module);
    void (* unload) (QfvModule *module);
};

struct _QfvModuleClass {
    GTypeModuleClass parent_class;
};

extern GType qfv_module_get_type() G_GNUC_CONST;

extern QfvModule *qfv_module_new(const gchar *filename);
extern void qfv_module_load(QfvModule *module);
extern void qfv_module_unload(QfvModule *module);

#endif

