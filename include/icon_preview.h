#pragma once

#include "log.h"
#include <gtkmm-3.0/gtkmm.h>
#include <string>
#include <memory>

namespace sway
{

using std::string;

class gtk_control;

class icon_preview : public Gtk::IconView
{

struct columns : Gtk::TreeModel::ColumnRecord
{
    columns()
    {
        add(pixbuf);
        add(name);
    }
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> pixbuf;
    Gtk::TreeModelColumn<string> name;
};
public:
    friend class gtk_control;

    icon_preview(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
        : Gtk::IconView(cobject)
    {
        _store = Gtk::ListStore::create(_col);
        set_model(_store);

        static std::vector<std::string> icons = {
            "folder",
            "application-x-executable",
            "audio-x-generic",
            "font-x-generic",
            "image-x-generic",
            "text-x-generic",
            "text-x-script",
            "video-x-generic",
            "x-office-document"};

        auto theme = Gtk::IconTheme::get_default();
        for (const auto &icon : icons)
            if (theme->has_icon(icon))
                (*(_store->append()))[_col.name] = icon;

        signal_style_updated().connect(sigc::mem_fun(*this, &icon_preview::on_icon_updated));
        set_pixbuf_column(_col.pixbuf);

        on_icon_updated();
    }

    void on_icon_updated()
    {
        auto theme = Gtk::IconTheme::get_default();
        auto children = _store->children();
        for (auto &row : children)
        {
            std::string name = row[_col.name];
            Glib::RefPtr<Gdk::Pixbuf> pix = row[_col.pixbuf];
            if (theme->has_icon(name))
                row[_col.pixbuf] = theme->load_icon(static_cast<std::string>(row[_col.name]), 100, Gtk::ICON_LOOKUP_FORCE_SIZE);
        }
    }

private:
    Glib::RefPtr<Gtk::ListStore> _store;
    columns _col;
};

} // namespace sway