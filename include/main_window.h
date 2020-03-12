#pragma once

#include "gtk_control.h"
#include "keybinding_view.h"
#include "about.h"

#include <gtkmm-3.0/gtkmm.h>
#include <memory>

namespace sway  
{

class main_window : public Gtk::Window
{
public:
  main_window(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

  // circular scrolling, only scroll tabs when mouse is within the tab area (roughly)
  bool on_scroll_event(GdkEventScroll *e);

  void on_quit();
  void on_about();

private:
  Gtk::Notebook *_book;
  
  about  *_about;
  keybinding_tree *_key_view;
  gtk_control& _gtk_control;
};

} // namespace sway