#pragma once

#include "log.h"
#include <memory>
#include <gtkmm-3.0/gtkmm.h>
#include <glibmm-2.4/glibmm.h>

namespace sway
{

using Glib::ustring;

class keybinding : public Glib::Object
{
protected:
  keybinding(const std::vector<guint> &keys)
      : Glib::ObjectBase(typeid(keybinding)), _property_text(*this, "label"), _keys(keys)
  {
    int len = _keys.size();
    ustring text;
    for (auto i = 0; i < len; i++)
    {
      if (!(i % 2) && (i != len - 1))
        text.append(" + ");
      else
        text.append(gdk_keyval_name(_keys[i]));
    }
    //_property_text.set_value("HELLO WORLD");
  }

public:
  
  static Glib::RefPtr<keybinding> create(const std::vector<guint> &keys)
  {
    return Glib::RefPtr<keybinding>(new keybinding(keys));
  }
  
  Glib::PropertyProxy<ustring> property_text() { return _property_text.get_proxy();}

private:
  std::vector<guint> _keys;
  Glib::Property<ustring> _property_text;
};

class keybinding_tree : public Gtk::TreeView
{

  struct columns : Gtk::TreeModel::ColumnRecord
  {
    columns()
    {
      add(cmd);
      add(key);
      add(modes);
      add(test);
    }
    Gtk::TreeModelColumn<ustring> cmd;
    Gtk::TreeModelColumn<Glib::RefPtr<Gtk::TreeModel>> modes;
    Gtk::TreeModelColumn<Glib::RefPtr<keybinding>> key;
    Gtk::TreeModelColumn<ustring>test;
  };

public:
  keybinding_tree(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
      : Gtk::TreeView(cobject)
  {
    _store = Gtk::ListStore::create(_col);
    set_model(_store);
    set_fixed_height_mode(false); // TODO: FIX
    auto it = _store->append();
    (*it)[_col.cmd] = "swaybar -b";
    
    std::vector<guint> test{1,2,3,4,5,6,7};
    auto key = keybinding::create(test);
    (*it)[_col.key] = key;

    auto col_key = Gtk::make_managed<Gtk::TreeView::Column>("Keybindings");
    auto col_mode = Gtk::make_managed<Gtk::TreeView::Column>("Mode");
  
    auto cell_key = Gtk::make_managed<Gtk::CellRendererText>();
    auto cell_mode = Gtk::make_managed<Gtk::CellRendererCombo>();

    col_key->pack_start(*cell_key);
    col_mode->pack_start(*cell_mode);

    cell_key->set_property("editable", true);
    
    log.warning("TARGET: ", cell_key->property_text());
    log.warning("SOURCE: ", key->property_text());
    Glib::Binding::bind_property(cell_key->property_text(), key->property_text()); // this isn't working why? 
    //col_key->add_attribute(cell_key->property_text(), _col.test);
    log.warning("TARGET: ", cell_key->property_text());
    log.warning("SOURCE: ", key->property_text());

    append_column(*col_mode);
    append_column(*col_key);
    append_column_editable("Command", _col.cmd);

    key->property_text().set_value("AFTER");
  }

private:
  Glib::RefPtr<Gtk::ListStore> _store;
  columns _col;
};

} // namespace sway
