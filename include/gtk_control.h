#pragma once

#include "theme_view.h"
#include "icon_preview.h"

#include <gtkmm-3.0/gtkmm.h>
#include <giomm-2.4/giomm/settings.h>
#include <iostream>
#include <memory>
#include <string>

namespace sway
{

using std::string;
using std::array;

class gtk_control
{
protected:
  gtk_control(const Glib::RefPtr<Gtk::Builder> &);
public:
  enum ThemeId
  {
    Theme = 0,
    Icon,
    Cursor,
    Font
  };

  static gtk_control& get(const Glib::RefPtr<Gtk::Builder>& builder)
  {
    static gtk_control singleton{builder};
    return singleton;
  }

  void on_theme_activated(const Gtk::ListStore::Path &, Gtk::TreeViewColumn *, ThemeId);

  void set_gtk3(const string &, ThemeId) const;
  void set_gtk2(const string &, ThemeId) const;
  string get_global(ThemeId) const;

private:
  void load_themes();
  void load_icons();
  void load_themes_from_dir(const string &);
  void load_icons_from_dir(const string &);

  Gtk::FontButton *_font;

  array<theme_tree*, 3> _theme_trees;
  icon_preview *_icon_view;

  array<string, 4>  _init_theme3_keys();
  array<string, 4>  _init_theme2_keys();

  static constexpr char _theme_schema[] = "org.gnome.desktop.interface";
  const array<string, 4> _theme3_keys =_init_theme3_keys();
  const array<string, 4> _theme2_keys = _init_theme2_keys();

};

} // namespace sway