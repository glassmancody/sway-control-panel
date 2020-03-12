#pragma once

#include "config.h"
#include "util.h"
#include "log.h"
#include <memory>
#include <gtkmm-3.0/gtkmm.h>

namespace sway  
{

class about : public Gtk::AboutDialog
{
public:
  about(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
      : Gtk::AboutDialog(cobject)
  {
    set_logo(Gdk::Pixbuf::create_from_resource(resource("img/icon.svg"), 200, 200));
    set_name(app.display_name);
    set_version(app.version);
  }
};

} // namespace sway