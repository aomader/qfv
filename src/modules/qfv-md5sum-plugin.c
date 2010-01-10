#include <libqfv/qfv-plugin.h>
#include <libqfv/qfv-module.h>

#define QFV_TYPE_MD5SUM_PLUGIN (qfv_md5sum_plugin_type)
#define QFV_MD5SUM_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_CAST((o), \
    QFV_TYPE_MD5SUM_PLUGIN, QfvMd5sumPlugin))
#define QFV_MD5SUM_PLUGIN_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), \
    QFV_TYPE_MD5SUM_PLUGIN, QfvMd5sumPluginClass))
#define QFV_IS_MD5SUM_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), \
    QFV_TYPE_MD5SUM_PLUGIN))

typedef struct _QfvMd5sumPlugin QfvMd5sumPlugin;
typedef struct _QfvMd5sumPluginClass QfvMd5sumPluginClass;

struct _QfvMd5sumPlugin {
    QfvPlugin parent_instance;
};

struct _QfvMd5sumPluginClass {
    QfvPluginClass parent_class;
};

static GType qfv_md5sum_plugin_get_type(GTypeModule *module);

static void qfv_md5sum_plugin_class_init(QfvMd5sumPluginClass *klass);
static void qfv_md5sum_plugin_init(QfvMd5sumPlugin *plugin);

static gboolean qfv_md5sum_plugin_parse_file(QfvPlugin *plugin,
    const gchar *filename, GtkListStore *list_store, GError **error);
static gboolean qfv_md5sum_plugin_hash_file(QfvPlugin *plugin,
    const gchar *filename, gchar **hash, GError **error);
static gboolean qfv_md5sum_plugin_write_file(QfvPlugin *plugin,
    const gchar *filename, GtkListStore *list_store, GError **error);

static GType qfv_md5sum_plugin_type = 0;
static QfvPluginClass *qfv_md5sum_plugin_parent_class = NULL;

G_MODULE_EXPORT void qfv_module_load(QfvModule *module)
{
    qfv_md5sum_plugin_get_type(G_TYPE_MODULE(module));
}

G_MODULE_EXPORT void qfv_module_unload(QfvModule *module)
{
    /* ... */
}

static GType qfv_md5sum_plugin_get_type(GTypeModule *module)
{
    if (qfv_md5sum_plugin_type == 0) {
        static const GTypeInfo plugin_info = {
            sizeof(QfvMd5sumPluginClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) qfv_md5sum_plugin_class_init,
            NULL,
            NULL,
            sizeof(QfvMd5sumPlugin),
            0,
            (GInstanceInitFunc) qfv_md5sum_plugin_init
        };

        qfv_md5sum_plugin_type = g_type_module_register_type(module,
            QFV_TYPE_PLUGIN, "QfvMd5sumPlugin", &plugin_info, 0);
    }

    return qfv_md5sum_plugin_type;
}

static void qfv_md5sum_plugin_class_init(QfvMd5sumPluginClass *klass)
{
    QfvPluginClass *plugin_class = QFV_PLUGIN_CLASS(klass);

    qfv_md5sum_plugin_parent_class = g_type_class_peek_parent(klass);

    plugin_class->name = "md5sum";
    plugin_class->description = "md5sum dum dum";
    plugin_class->parse_file = qfv_md5sum_plugin_parse_file;
    plugin_class->hash_file = qfv_md5sum_plugin_hash_file;
    plugin_class->write_file = qfv_md5sum_plugin_write_file;
}

static void qfv_md5sum_plugin_init(QfvMd5sumPlugin *plugin)
{
    /* ... */
}

static gboolean qfv_md5sum_plugin_parse_file(QfvPlugin *plugin,
    const gchar *filename, GtkListStore *list_store, GError **error)
{
    return TRUE;
}

static gboolean qfv_md5sum_plugin_hash_file(QfvPlugin *plugin,
    const gchar *filename, gchar **hash, GError **error)
{

    GIOChannel *file = g_io_channel_new_file(filename, "r", error);

    if (file == NULL)
        return FALSE;

    g_io_channel_set_encoding(file, NULL, NULL);

    GChecksum *md5 = g_checksum_new(G_CHECKSUM_MD5);

    GIOStatus status;
    guchar buffer[16384];
    gsize read;

    while (1) {
        status = g_io_channel_read_chars(file, buffer, 16384, &read, error);

        if (status == G_IO_STATUS_ERROR)
            return FALSE;

        if (status == G_IO_STATUS_AGAIN)
            continue;

        g_checksum_update(md5, buffer, read);

        if (status == G_IO_STATUS_EOF)
            break;
    }

    *hash = g_strdup(g_checksum_get_string(md5));

    g_io_channel_shutdown(file, FALSE, NULL);
    g_checksum_free(md5);

    return TRUE;
}

static gboolean qfv_md5sum_plugin_write_file(QfvPlugin *plugin,
    const gchar *filename, GtkListStore *list_store, GError **error)
{
    GIOChannel *file = g_io_channel_new_file(filename, "r", error);

    if (file == NULL)
        return FALSE;

    return TRUE;
}

