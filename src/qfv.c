/**
 * This file is part of qfv.
 * Copyright (C) 2009,2010  Oliver Mader <dotb52@gmail.com>
 *
 * AUTHORS
 *     Oliver Mader <dotb52@gmail.com>
 * 
 * CONTRIBUTORS
 *     Sven Herzberg <herzi@lanedo.com>
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

#define GETTEXT_PACKAGE "gtk20"

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "qfv-window.h"

static gboolean verbose = FALSE;
static gboolean version = FALSE;

static GOptionEntry entries[] = {
	{"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL},
	{"version", 0, 0, G_OPTION_ARG_NONE, &version, "Show version", NULL},
	{NULL}
};

static void qfv_log(const gchar *domain, GLogLevelFlags flags,
	const gchar *message, gpointer user_data);

int main(int argc, char *argv[])
{
	GOptionContext *option = g_option_context_new("[FILE]");
	g_option_context_add_group(option, gtk_get_option_group(TRUE));
	g_option_context_add_main_entries(option, entries, GETTEXT_PACKAGE);
	g_option_context_set_summary(option, "A GTK+ based file verification "
		"utility.");
	g_option_context_set_help_enabled(option, TRUE);

	GError *error = NULL;
	if (g_option_context_parse(option, &argc, &argv, &error) == FALSE) {
		g_print("%s\n", error->message);
		g_option_context_free(option);
		g_error_free(error);

		return 1;
	}

	g_option_context_free(option);
	
	if (version == TRUE) {
		g_print("qfv " VERSION "\n");
		return 0;
	}
	
	if (verbose == FALSE) {
		GLogFunc old_handler = g_log_set_default_handler(qfv_log, NULL);
		g_log_set_default_handler(qfv_log, old_handler);
	}
	
	g_thread_init(NULL);
	
	QfvWindow *window = qfv_window_new();
	gtk_widget_show_all(GTK_WIDGET(window));
	
	gtk_main();
	
	return 0;
}

static void qfv_log(const gchar *domain, GLogLevelFlags flags,
	const gchar *message, gpointer user_data)
{
	GLogFunc old_func = user_data;
	
	if (user_data == NULL)
		old_func = g_log_default_handler;
	
	if ((flags & G_LOG_LEVEL_MASK) < G_LOG_LEVEL_MESSAGE)
		old_func(domain, flags, message, NULL);
}

