#include "main_window.h"
#include "log.h"
#include "util.h"

#include <gdk/gdkwayland.h>

namespace sway
{

main_window::main_window(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
    : Gtk::Window(cobject), _gtk_control(gtk_control::get(builder))
{
#ifdef GDK_WINDOWING_WAYLAND
    if (!(GDK_IS_WAYLAND_DISPLAY(Gdk::Display::get_default()->gobj()) && std::getenv("SWAYSOCK")))
    {
        Gtk::MessageDialog{
            *this, "Please make sure you are running from a sway session",
            false,
            Gtk::MessageType::MESSAGE_ERROR,
            Gtk::ButtonsType::BUTTONS_OK}
            .run();
        throw std::runtime_error("GDK wayland backend not found");
    }
#else
    Gtk::MessageDialog{
        *this, "GDK-Wayland not found",
        false,
        Gtk::MessageType::MESSAGE_ERROR,
        Gtk::ButtonsType::BUTTONS_OK}
        .run();
    throw std::runtime_error("GDK wayland backend not found");
#endif

    Gtk::MenuItem *menu_quit, *menu_about;
    builder->get_widget("book", _book);
    builder->get_widget("main_menu_quit", menu_quit);
    builder->get_widget("main_menu_about", menu_about);
    builder->get_widget_derived("about", _about);
    builder->get_widget_derived("key_view",_key_view);

    menu_quit->signal_activate().connect(sigc::mem_fun(*this, &main_window::on_quit));
    menu_about->signal_activate().connect(sigc::mem_fun(*this, &main_window::on_about));

    _book->add_events(Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
    _book->signal_scroll_event().connect(sigc::mem_fun(*this, &main_window::on_scroll_event));


    show();
}

bool main_window::on_scroll_event(GdkEventScroll *e)
{
    Gtk::Widget *child = _book->get_nth_page(_book->get_current_page());

    if (!child)
        return false;

    // since the tabs are guaranteed to be on the left side, we
    // can simply compare the x components of the mouse and the 
    // active page 
    auto allocation = child->get_allocation();
    if (e->x > allocation.get_x())
        return false;

    switch (e->direction)
    {
    case GdkScrollDirection::GDK_SCROLL_RIGHT:
    case GdkScrollDirection::GDK_SCROLL_DOWN:
    {
        if (_book->get_current_page() == _book->get_n_pages() - 1)
            _book->set_current_page(0);
        else
            _book->next_page();
        break;
    }
    case GdkScrollDirection::GDK_SCROLL_LEFT:
    case GdkScrollDirection::GDK_SCROLL_UP:
    {
        if (_book->get_current_page() == 0)
            _book->set_current_page(-1);
        else
            _book->prev_page();
        break;
    }
    }
    return true;
}

void main_window::on_quit()
{
    log.info("Quitting peacefully.");
    hide();
}

void main_window::on_about()
{    
    _about->run();
}

} // namespace sway