#pragma once

#include "util.h"

#include <gtkmm-3.0/gtkmm.h>

#include <memory>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

namespace sway
{

using Glib::ustring;

enum binding_input_type
{
  BINDING_KEYCODE,
  BINDING_KEYSYM,
  BINDING_MOUSE,
};

enum config_dpms
{
  DPMS_IGNORE,
  DPMS_ON,
  DPMS_OFF
};

/**
 * A key binding and an associated command.
 */
struct sway_binding
{
  binding_input_type type;
  int order;
  ustring input;
  uint32_t flags;
  std::vector<uint32_t> keys; // sorted in ascending order
  uint32_t modifiers;
  ustring command;
};

/**
 * Size and position configuration for a particular output.
 *
 * This is set via the `output` command.
 */
struct output_config
{
  ustring name;
  int enabled;
  int width, height;
  float refresh_rate;
  int x, y;
  float scale;
  int32_t transform;

  ustring background;
  ustring background_option;
  ustring background_fallback;
  config_dpms dpms_state;
};

/**
 * Stores size of gaps for each side
 */
struct side_gaps
{
  int top;
  int right;
  int bottom;
  int left;
};

class bar_config : public Glib::GObject
{
public:

  bar_config
    : Glib::ObjectBase(typeid(bar_config)),
  {

  }

  virtual ~bar_config() {}

  /**
	 * Id name used to identify the bar through IPC.
	 *
	 * Defaults to bar-x, where x corresponds to the position of the
	 * embedding bar block in the config file (bar-0, bar-1, ...).
	 */
  Glib::Property<ustring>                     id;
  Glib::Property<ustring>                     mode;
  Glib::Property<ustring>                     hidden_state;
  Glib::Property<ustring>                     position;
  Glib::Property<ustring>                     separator_symbol;
  Glib::Property<ustring>                     status_command;
  Glib::Property<ustring>                     swaybar_command;
  Glib::Property<ustring>                     font;
  Glib::Property<Glib::RefPtr<Gtk::ListStore<output_config>>>  outputs;
  Glib::Property<Glib::RefPtr<Gtk::ListStore<sway_binding>>>   bindings;
  bool                        visible_by_modifier;
  bool                        pango_markup;
  bool                        workspace_buttons;
  bool                        wrap_scroll;
  bool                        strip_workspace_numbers;
  bool                        strip_workspace_name;
  bool                        binding_mode_indicator;
  bool                        verbose;
  uint32_t                    modifier;
  int                         height;
  side_gaps                   gaps;
  struct colors
  {
    ustring background;
    ustring statusline;
    ustring separator;
    ustring focused_background;
    ustring focused_statusline;
    ustring focused_separator;
    ustring focused_workspace_border;
    ustring focused_workspace_bg;
    ustring focused_workspace_text;
    ustring active_workspace_border;
    ustring active_workspace_bg;
    ustring active_workspace_text;
    ustring inactive_workspace_border;
    ustring inactive_workspace_bg;
    ustring inactive_workspace_text;
    ustring urgent_workspace_border;
    ustring urgent_workspace_bg;
    ustring urgent_workspace_text;
    ustring binding_mode_border;
    ustring binding_mode_bg;
    ustring binding_mode_text;
  }
};

class sway_config
{
public:
  sway_config()
  {
  }

  void read_config(const std::string &path)
  {
    std::ifstream file{path};
    if (!file)
      return;

    std::string line;
    while (read_continued_line(file, line))
    {
      if (line.empty() || line[0] == delim_comment)
        continue;
      std::cout << line << std::endl;
    }
  }

  bool read_continued_line(std::ifstream &file, std::string &dest)
  {
    std::string next_line, line;
    if (!std::getline(file, line))
      return false;
    auto len = line.length();

    ltrim_whitespace(line);
    if (line == "\\")
      return true; // skip these

    // compare last two characters
    while (len >= 1 && line[len - 1] == delim_continue)
    {
      if (!std::getline(file, next_line))
        return false;
      ltrim_whitespace(next_line);
      line.replace(len - 1, next_line.length(), next_line);
    }
    rtrim_whitespace(line);
    dest = std::move(line);
    return true;
  }

  static constexpr char delim_comment = '#';
  static constexpr char delim_continue = '\\';

private:
};
} // namespace sway
