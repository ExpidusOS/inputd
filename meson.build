project('expidus-inputd', 'c', license: 'GPL-3.0-only', version: '0.1.0-prealpha')

gdk = dependency('gdk-3.0')
gdk_x11 = dependency('gdk-x11-3.0')
glib = dependency('glib-2.0')
libdevident = dependency('libdevident')
libinput = dependency('libinput')

conf_data = configuration_data()
conf_data.set('bindir', join_paths(get_option('prefix'), get_option('bindir')))
configure_file(input: 'data/expidus-inputd.desktop.in', output: 'expidus-inputd.desktop',
	configuration: conf_data,
	install: true,
	install_dir: join_paths(get_option('sysconfdir'), 'xdg/autostart'))

inputd = executable('expidus-inputd', ['src/main.c', 'src/math.c'],
	install: true,
	include_directories: [include_directories('include')],
	dependencies: [gdk, gdk_x11, glib, libdevident, libinput, meson.get_compiler('c').find_library('m')])
