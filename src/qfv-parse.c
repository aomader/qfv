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

#include "qfv-parse.h"

#include <string.h>

#include "qfv-module-manager.h"

extern void qfv_parse_file(QfvWindow *window, const char *filename)
{
    /**
     * TODO: in case to support win i should also recognies thair slashes
     */
    char *file = strchr(filename, '/');
    
    if (file == NULL)
        file = filename;

    window->plugin = qfv_module_manager_find_plugin(file);

    if (window->plugin == NULL) {
        qfv_window_error(window, _("%s: Wasn't able to find the correct plugin"
            " to parse this file."), filename);
    } else {
        GError *error = NULL;

        if (qfv_plugin_parse_file(plugin, filename, window, &error)
            == FALSE)
        {
            qfv_window_error(window, error->message);
            g_error_free(error);
        } else {
            /* verify !! */
        }
    }
}

