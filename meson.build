project(
    'sway-control-panel',
    'c','cpp',
    version: '0.01',
    license: 'MIT',
    default_options: [
        'cpp_std=c++14',
        'warning_level=3'
    ],
    meson_version : '>=0.46.0'
)

add_project_arguments(
	[
        '-g',
		'-Wall',
        '-Wextra',
        '-Werror',
        '-Wno-error',
        '-Wfatal-errors'
	],
	language: 'cpp',
)

gnome = import('gnome')

resources = gnome.compile_resources(
    'gresources',
    'resources/gresource.xml',
    source_dir: 'resources'
)

display_name = 'Sway Control Panel'
name = meson.project_name()

config = configuration_data()
config.set_quoted('PROJECT_NAME', name)
config.set_quoted('PROJECT_DISPLAY_NAME', display_name)
config.set_quoted('PROJECT_VERSION', meson.project_version())
config.set_quoted('PROJECT_GTK_ID', 'org.gtkmm.' + meson.project_name())
config.set_quoted('PROJECT_RESOURCE_ROOT_PATH', '/org/gtkmm/sway-control-panel/')

config_desktop = configuration_data()
config_desktop.set('PROJECT_NAME', name)
config_desktop.set('PROJECT_DISPLAY_NAME', display_name)

configure_file(
    input: 'include/config.h.in',
    output: 'config.h',
    configuration: config
)

configure_file(
    input: 'resources/sway-control-panel.desktop.in',
    output: 'sway-control-panel.desktop',
    configuration: config_desktop 
)

include_dir = include_directories('include')

gtkmm = dependency('gtkmm-3.0')
gdkmm = dependency('gdkmm-3.0')
giomm = dependency('giomm-2.4')
gdkwayland = dependency('gdk-wayland-3.0')
wayland_client = dependency('wayland-client')
wayland_cursor = dependency('wayland-cursor')

deps = [
    gtkmm,
    gdkmm,
    giomm,
    gdkwayland,
 #   wayland_client,
    wayland_cursor
]

sources = files(
    'src/main.cpp',
    'src/gtk_control.cpp',
    'src/main_window.cpp',
    'src/config.cpp'
)

executable(
    meson.project_name(),
    [sources, resources],
    dependencies: deps, 
    include_directories: include_dir
)