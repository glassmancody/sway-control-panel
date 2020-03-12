#pragma once

#include <gtkmm-3.0/gtkmm.h>
#include <string>
#include <memory>

namespace sway
{

class gtk_control;

class theme_tree : public Gtk::TreeView
{
  struct columns : Gtk::TreeModel::ColumnRecord
  {
    columns()
    {
      add(name);
      add(display_name);
      add(comment);
      add(gtk2);
      add(gtk3);
    }

    Gtk::TreeModelColumn<std::string> name;
    Gtk::TreeModelColumn<std::string> display_name;
    Gtk::TreeModelColumn<std::string> comment;
    Gtk::TreeModelColumn<bool> gtk2;
    Gtk::TreeModelColumn<bool> gtk3;
  };

public:
  friend class gtk_control;

  theme_tree(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
      : Gtk::TreeView(cobject)
  {
    _store = Gtk::ListStore::create(_col);
    _store->set_sort_column(_col.display_name, Gtk::SortType::SORT_ASCENDING);
    set_model(_store);
    append_column("", _col.display_name);
  }

private:
  Glib::RefPtr<Gtk::ListStore> _store;
  columns _col;
};

} // namespace sway