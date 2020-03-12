#include "gtk_control.h"
#include "log.h"
#include "util.h"

#include <fstream>

namespace sway
{

using Glib::build_filename;

constexpr char gtk_control::_theme_schema[];

gtk_control::gtk_control(const Glib::RefPtr<Gtk::Builder> &builder)
{
  // retrieve our widgets from our builder
  builder->get_widget_derived("cursor_tree", _theme_trees[Cursor]);
  builder->get_widget_derived("icon_tree", _theme_trees[Icon]);
  builder->get_widget_derived("theme_tree", _theme_trees[Theme]);
  builder->get_widget_derived("icon_view", _icon_view);
  builder->get_widget("theme_font", _font);

  for (int i = 0; i < _theme_trees.size(); i++)
    _theme_trees[i]->signal_row_activated().connect(sigc::bind<ThemeId>(sigc::mem_fun(*this, &gtk_control::on_theme_activated), static_cast<ThemeId>(i)));

  Gio::Settings::create(_theme_schema)->bind("font-name", _font->property_font_name());

  log.info("Searching for GTK themes, icon themes, and cursor themes...");
  load_themes();
  load_icons();
}

void gtk_control::set_gtk3(const std::string &theme, ThemeId id) const
{
  log.info("Setting GTK3 ", _theme3_keys[id], " = ", theme);
  Gio::Settings::create(_theme_schema)->set_string(_theme3_keys[id], theme);
}

void gtk_control::set_gtk2(const std::string &theme, ThemeId id) const
{
  log.info("Setting GTK2 ", _theme2_keys[id], " = ", theme);

  // we need to set the settings.ini for gtk2 themes
  auto ini = build_filename(Glib::get_user_config_dir(), "gtk-3.0", "settings.ini");
  Glib::KeyFile kf;
  if (file_exists(ini))
  {
    try
    {
      kf.load_from_file(ini);
    }
    catch (Glib::FileError &e)
    {
      log.error(e.what());
      return;
    }
  }

  kf.set_string("Settings", _theme2_keys[id], theme);

  try
  {
    kf.save_to_file(ini);
  }
  catch (Glib::FileError &e)
  {
    log.error(e.what());
  }

  return;
}

std::string gtk_control::get_global(ThemeId id) const
{
  return Gio::Settings::create(_theme_schema)->get_string(_theme3_keys[id]);
}

void gtk_control::on_theme_activated(const Gtk::ListStore::Path &path, Gtk::TreeViewColumn *col, ThemeId id)
{
  auto it = _theme_trees[id]->_store->get_iter(path);

  bool gtk2 = (*it)[_theme_trees[id]->_col.gtk2];
  bool gtk3 = (*it)[_theme_trees[id]->_col.gtk3];

  std::string theme = (*it)[_theme_trees[id]->_col.name];
  if (!it)
    return;

  if (id == Theme)
  {
    if (gtk2)
      set_gtk2(theme, Theme);
    if (gtk3)
      set_gtk3(theme, Theme);
  }
  else if (id == Icon)
  {
    set_gtk3(theme, Icon);
    set_gtk2(theme, Icon);
  }
  else if (id == Cursor)
  {
    set_gtk3(theme, Cursor);
    set_gtk2(theme, Cursor);
  }
}

void gtk_control::load_themes_from_dir(const std::string &path)
{
  if (!dir_exists(path))
    return;

  Glib::Dir dir(path);

  for (const auto &theme_path : dir)
  {
    auto index_path = build_filename(path, theme_path, "index.theme"); // read metadata from theme if available

    bool gtk2 = false;
    bool gtk3 = false;

    auto gtk2_identifer = build_filename(path, theme_path, "gtk-2.0", "gtkrc");

    static array<string, 3> gtk3_versions = {"gtk-3.0", "gtk-3.18", "gtk-3.20"};

    // search for gtk2 theme identifier
    if (file_exists(gtk2_identifer))
      gtk2 = true;

    // search for gtk2 theme identifier
    for (const auto &version : gtk3_versions)
    {
      if (file_exists(build_filename(path, theme_path, version, "gtk.css")))
      {
        gtk3 = true;
        break;
      }
    }

    if (!gtk2 && !gtk3)
    {
      log.warning("theme folder found, but does not contain qualifying files: ",
                  build_filename(path, theme_path));
      continue;
    }

    auto row = *(_theme_trees[Theme]->_store->append());
    row[_theme_trees[Theme]->_col.name] = theme_path;
    row[_theme_trees[Theme]->_col.display_name] = theme_path;
    row[_theme_trees[Theme]->_col.gtk2] = gtk2;
    row[_theme_trees[Theme]->_col.gtk3] = gtk3;

    Glib::KeyFile kf;

    if (file_exists(index_path) && kf.load_from_file(index_path) &&
        kf.has_group("Desktop Entry"))
    {
      if (kf.has_key("Desktop Entry", "Name"))
        row[_theme_trees[Theme]->_col.display_name] = kf.get_locale_string("Desktop Entry", "Name");
      if (kf.has_key("Desktop Entry", "Comment"))
        row[_theme_trees[Theme]->_col.comment] = kf.get_locale_string("Desktop Entry", "Comment");
    }

    if (theme_path == get_global(Theme))
      _theme_trees[Theme]->get_selection()->select(row);

    log.info("adding a gtk theme <", theme_path, "> ");
  }
}

void gtk_control::load_icons_from_dir(const std::string &path)
{
  if (!dir_exists(path))
    return;

  Glib::Dir dir(path);

  for (const auto &theme_path : dir)
  {
    auto index_path = build_filename(path, theme_path, "index.theme");

    Glib::KeyFile kf;

    // index.theme required for cursor and icon themes
    if (!file_exists(index_path) || !kf.load_from_file(index_path))
      continue;

    std::string display_name = theme_path;
    std::string comment;
    bool contains_icon = false;

    if (kf.has_group("Icon Theme"))
    {
      if (kf.has_key("Icon Theme", "Name"))
        display_name = kf.get_locale_string("Icon Theme", "Name");
      if (kf.has_key("Icon Theme", "Comment"))
        comment = kf.get_locale_string("Icon Theme", "Comment");
      if (kf.has_key("Icon Theme", "Directories"))
        contains_icon = true;
    }

    auto cursor_path = build_filename(path, theme_path, "cursors");

    if (file_exists(cursor_path))
    {
      log.info("adding a cursor theme <", theme_path, ">");
      auto row = *(_theme_trees[Cursor]->_store->append());
      if (theme_path == get_global(Cursor))
        _theme_trees[Cursor]->get_selection()->select(row);
      row[_theme_trees[Cursor]->_col.name] = theme_path;
      row[_theme_trees[Cursor]->_col.display_name] = display_name;
    }

    if (contains_icon)
    {
      log.info("adding an icon theme <", theme_path, ">");
      auto row = *(_theme_trees[Icon]->_store->append());
      if (theme_path == get_global(Icon))
        _theme_trees[Icon]->get_selection()->select(row);
      row[_theme_trees[Icon]->_col.name] = theme_path;
      row[_theme_trees[Icon]->_col.display_name] = display_name;
    }
  }
}

void gtk_control::load_themes()
{
  // $HOME/.local/share/themes
  load_themes_from_dir(build_filename(Glib::get_user_data_dir(), "themes"));
  // $HOME/.themes
  load_themes_from_dir(build_filename(Glib::get_home_dir(), ".themes"));
  // /usr/share/themes
  load_themes_from_dir("/usr/share/themes");
}

void gtk_control::load_icons()
{
  auto list = Gtk::IconTheme::get_default()->get_search_path();
  for (auto path : list)
  {
    if (!dir_exists(path))
      continue;

    load_icons_from_dir(path);
  }
}

array<string, 4> gtk_control::_init_theme3_keys()
{
  array<string, 4> a;
  a[Theme] = "gtk-theme";
  a[Icon] = "icon-theme";
  a[Cursor] = "cursor-theme";
  a[Font] = "font-theme";
  return a;
}

array<string, 4> gtk_control::_init_theme2_keys()
{
  array<string, 4> a;
  a[Theme] = "gtk-theme-name";
  a[Icon] = "gtk-icon-theme-name";
  a[Cursor] = "gtk-cursor-theme-name";
  a[Font] = "gtk-font-name";
  return a;
}

} // namespace sway