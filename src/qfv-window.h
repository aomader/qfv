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

#ifndef QFV_WINDOW_H
#define QFV_WINDOW_H

#include <config.h>

#include <gtk/gtk.h>

#include "qfv-module-manager.h"
#include "libqfv/qfv-module.h"

#define QFV_TYPE_WINDOW (qfv_window_get_type())
#define QFV_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), QFV_TYPE_WINDOW, \
    QfvWindow))
#define QFV_IS_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), QFV_TYPE_WINDOW))
#define QFV_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), \
    QFV_TYPE_WINDOW, QfvWindowClass))
#define QFV_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), \
    QFV_TYPE_WINDOW))
#define QFV_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
    QFV_TYPE_WINDOW, QfvWindowClass))

typedef struct _QfvWindow QfvWindow;
typedef struct _QfvWindowClass QfvWindowClass;
typedef struct _QfvWindowPrivate QfvWindowPrivate;

struct _QfvWindow {
    GtkWindow parent_instance;
    
    QfvModuleManager *module_manager;
    
    QfvModule *current_module;
    gchar *current_dir;
    
    GtkListStore *list_store;
    
    GtkWidget *about_dialog;
    GtkWidget *modules_dialog;
    GtkWidget *verify_dialog;

    QfvWindowPrivate *priv;
};

struct _QfvWindowClass {
    GtkWindowClass parent_class;
};

enum {
    QFV_WINDOW_TREEVIEW_COLUMN_STATUS = 0,
    QFV_WINDOW_TREEVIEW_COLUMN_ICON,
    QFV_WINDOW_TREEVIEW_COLUMN_FILE,
    QFV_WINDOW_TREEVIEW_COLUMN_HASH,
    QFV_WINDOW_TREEVIEW_COLUMN_DESCRIPTION
};

typedef enum {
    QFV_WINDOW_TREEVIEW_STATUS_ERROR = -1,
    QFV_WINDOW_TREEVIEW_STATUS_UNKNOWN = 0,
    QFV_WINDOW_TREEVIEW_STATUS_OK = 1
} QfvWindowTreeviewStatus;

extern GType qfv_window_get_type() G_GNUC_CONST;

extern QfvWindow *qfv_window_new();

extern void qfv_window_error(QfvWindow *self, const gchar *format, ...);
extern void qfv_window_treeview_add(QfvWindow *self, const gchar *file,
    const gchar *hash);
extern void qfv_window_treeview_update(QfvWindow *self, GtkTreeIter *iter,
    QfvWindowTreeviewStatus status, const gchar *description);
extern void qfv_window_treeview_clear(QfvWindow *self);
extern void qfv_window_title_clear(QfvWindow *self);
extern void qfv_window_title_update(QfvWindow *self, const gchar *filename);

#endif
