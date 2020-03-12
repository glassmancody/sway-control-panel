#include "main_window.h"
#include "config.h"
#include "util.h"
#include "log.h"

#include <memory>
#include <iostream>

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, sway::app.gtk_id);

  // Load the GtkBuilder file and instantiate its widgets
  auto builder = Gtk::Builder::create();
  try
  {
    builder->add_from_resource(sway::resource("ui/main_window.glade"));
  }
  catch (const Gio::ResourceError &e)
  {
    sway::log.fatal_error(e.what());
    return 1;
  }
  catch (const Glib::FileError &e)
  {
    sway::log.fatal_error(e.what());
    return 1;
  }
  catch (const Glib::MarkupError &e)
  {
    sway::log.fatal_error(e.what());
    return 1;
  }
  catch (const Gtk::BuilderError &e)
  {
    sway::log.fatal_error(e.what());
    return 1;
  }

  sway::main_window *window = nullptr;

  builder->get_widget_derived("root", window);

  app->run(*window);

  if (window)
    delete window;
  return 0;
}
