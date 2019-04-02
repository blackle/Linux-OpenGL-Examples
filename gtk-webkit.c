#define GL_GLEXT_PROTOTYPES why

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdint.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit2/webkit2.h>

#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080

gboolean on_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, GtkWindow* window)
{
	if (load_event == WEBKIT_LOAD_FINISHED) {
		gtk_widget_show_all(GTK_WIDGET(window));
		gtk_window_fullscreen(window);
	}
	return TRUE;
}

static char html[] = {
#include "index.html.inc"
};

void _start() { 
	asm volatile("sub $8, %rsp\n");

	typedef void (*voidWithOneParam)(int*);
	voidWithOneParam gtk_init_one_param = (voidWithOneParam)gtk_init;
	(*gtk_init_one_param)(NULL);

	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	WebKitSettings *webSettings = webkit_settings_new_with_settings("enable-webgl", TRUE, NULL);
	WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new_with_settings(webSettings));
  webkit_web_view_load_html(webView, html, NULL);

	gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(webView));

	g_signal_connect(win, "destroy", gtk_main_quit, NULL);
	g_signal_connect(webView, "load-changed", G_CALLBACK(on_load_changed), (GtkWindow*)(win));

	gtk_main();

	asm volatile(".intel_syntax noprefix");
	asm volatile("push 231"); //exit_group
	asm volatile("pop rax");
	// asm volatile("xor edi, edi");
	asm volatile("syscall");
	asm volatile(".att_syntax prefix");
	__builtin_unreachable();
}