/*
 * This file is generated by gdbus-codegen, do not modify it.
 *
 * The license of this code is the same as for the D-Bus interface description
 * it was derived from. Note that it links to GLib, so must comply with the
 * LGPL linking clauses.
 */

#ifndef __NIMF_VKEYBOARDSERVICE_H__
#define __NIMF_VKEYBOARDSERVICE_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define NIMF_VKEYBOARD_SERVICE_NAME "com.tmaxos.Nimf.VKeyboardService"
#define NIMF_VKEYBOARD_PATH "/com/tmaxos/Nimf/VKeyboardService"
/* ------------------------------------------------------------------------ */
/* Declarations for com.tmaxos.Nimf.VKeyboardService */

#define NIMF_TYPE_VKEYBOARD_SERVICE (nimf_vkeyboard_service_get_type ())
#define NIMF_VKEYBOARD_SERVICE(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), NIMF_TYPE_VKEYBOARD_SERVICE, NimfVKeyboardService))
#define NIMF_IS_VKEYBOARD_SERVICE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), NIMF_TYPE_VKEYBOARD_SERVICE))
#define NIMF_VKEYBOARD_SERVICE_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), NIMF_TYPE_VKEYBOARD_SERVICE, NimfVKeyboardServiceIface))

struct _NimfVKeyboardService;
typedef struct _NimfVKeyboardService NimfVKeyboardService;
typedef struct _NimfVKeyboardServiceIface NimfVKeyboardServiceIface;

struct _NimfVKeyboardServiceIface
{
  GTypeInterface parent_iface;

  gboolean (*handle_engine_change) (
    NimfVKeyboardService *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_engineid,
    const gchar *arg_method);

  gboolean (*handle_focus_in) (
    NimfVKeyboardService *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_ic);

  gboolean (*handle_focus_out) (
    NimfVKeyboardService *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_ic);

};

GType nimf_vkeyboard_service_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *nimf_vkeyboard_service_interface_info (void);
guint nimf_vkeyboard_service_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void nimf_vkeyboard_service_complete_focus_in (
    NimfVKeyboardService *object,
    GDBusMethodInvocation *invocation);

void nimf_vkeyboard_service_complete_focus_out (
    NimfVKeyboardService *object,
    GDBusMethodInvocation *invocation);

void nimf_vkeyboard_service_complete_engine_change (
    NimfVKeyboardService *object,
    GDBusMethodInvocation *invocation);



/* D-Bus method calls: */
void nimf_vkeyboard_service_call_focus_in (
    NimfVKeyboardService *proxy,
    const gchar *arg_ic,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean nimf_vkeyboard_service_call_focus_in_finish (
    NimfVKeyboardService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean nimf_vkeyboard_service_call_focus_in_sync (
    NimfVKeyboardService *proxy,
    const gchar *arg_ic,
    GCancellable *cancellable,
    GError **error);

void nimf_vkeyboard_service_call_focus_out (
    NimfVKeyboardService *proxy,
    const gchar *arg_ic,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean nimf_vkeyboard_service_call_focus_out_finish (
    NimfVKeyboardService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean nimf_vkeyboard_service_call_focus_out_sync (
    NimfVKeyboardService *proxy,
    const gchar *arg_ic,
    GCancellable *cancellable,
    GError **error);

void nimf_vkeyboard_service_call_engine_change (
    NimfVKeyboardService *proxy,
    const gchar *arg_engineid,
    const gchar *arg_method,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean nimf_vkeyboard_service_call_engine_change_finish (
    NimfVKeyboardService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean nimf_vkeyboard_service_call_engine_change_sync (
    NimfVKeyboardService *proxy,
    const gchar *arg_engineid,
    const gchar *arg_method,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define NIMF_TYPE_VKEYBOARD_SERVICE_PROXY (nimf_vkeyboard_service_proxy_get_type ())
#define NIMF_VKEYBOARD_SERVICE_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), NIMF_TYPE_VKEYBOARD_SERVICE_PROXY, NimfVKeyboardServiceProxy))
#define NIMF_VKEYBOARD_SERVICE_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), NIMF_TYPE_VKEYBOARD_SERVICE_PROXY, NimfVKeyboardServiceProxyClass))
#define NIMF_VKEYBOARD_SERVICE_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NIMF_TYPE_VKEYBOARD_SERVICE_PROXY, NimfVKeyboardServiceProxyClass))
#define NIMF_IS_VKEYBOARD_SERVICE_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), NIMF_TYPE_VKEYBOARD_SERVICE_PROXY))
#define NIMF_IS_VKEYBOARD_SERVICE_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), NIMF_TYPE_VKEYBOARD_SERVICE_PROXY))

typedef struct _NimfVKeyboardServiceProxy NimfVKeyboardServiceProxy;
typedef struct _NimfVKeyboardServiceProxyClass NimfVKeyboardServiceProxyClass;
typedef struct _NimfVKeyboardServiceProxyPrivate NimfVKeyboardServiceProxyPrivate;

struct _NimfVKeyboardServiceProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  NimfVKeyboardServiceProxyPrivate *priv;
};

struct _NimfVKeyboardServiceProxyClass
{
  GDBusProxyClass parent_class;
};

GType nimf_vkeyboard_service_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (NimfVKeyboardServiceProxy, g_object_unref)
#endif

void nimf_vkeyboard_service_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
NimfVKeyboardService *nimf_vkeyboard_service_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
NimfVKeyboardService *nimf_vkeyboard_service_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void nimf_vkeyboard_service_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
NimfVKeyboardService *nimf_vkeyboard_service_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
NimfVKeyboardService *nimf_vkeyboard_service_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define NIMF_TYPE_VKEYBOARD_SERVICE_SKELETON (nimf_vkeyboard_service_skeleton_get_type ())
#define NIMF_VKEYBOARD_SERVICE_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), NIMF_TYPE_VKEYBOARD_SERVICE_SKELETON, NimfVKeyboardServiceSkeleton))
#define NIMF_VKEYBOARD_SERVICE_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), NIMF_TYPE_VKEYBOARD_SERVICE_SKELETON, NimfVKeyboardServiceSkeletonClass))
#define NIMF_VKEYBOARD_SERVICE_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NIMF_TYPE_VKEYBOARD_SERVICE_SKELETON, NimfVKeyboardServiceSkeletonClass))
#define NIMF_IS_VKEYBOARD_SERVICE_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), NIMF_TYPE_VKEYBOARD_SERVICE_SKELETON))
#define NIMF_IS_VKEYBOARD_SERVICE_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), NIMF_TYPE_VKEYBOARD_SERVICE_SKELETON))

typedef struct _NimfVKeyboardServiceSkeleton NimfVKeyboardServiceSkeleton;
typedef struct _NimfVKeyboardServiceSkeletonClass NimfVKeyboardServiceSkeletonClass;
typedef struct _NimfVKeyboardServiceSkeletonPrivate NimfVKeyboardServiceSkeletonPrivate;

struct _NimfVKeyboardServiceSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  NimfVKeyboardServiceSkeletonPrivate *priv;
};

struct _NimfVKeyboardServiceSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType nimf_vkeyboard_service_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (NimfVKeyboardServiceSkeleton, g_object_unref)
#endif

NimfVKeyboardService *nimf_vkeyboard_service_skeleton_new (void);


G_END_DECLS

#endif /* __NIMF_VKEYBOARDSERVICE_H__ */
