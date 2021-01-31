project('expidus-inputd', 'c', license: 'GPL-3.0-only', version: '0.1.0-prealpha')

gdk = dependency('gdk-3.0')
gdk_x11 = dependency('gdk-x11-3.0')
glib = dependency('glib-2.0')
libdevident = dependency('libdevident')
libinput = dependency('libinput')

inputd = executable('expidus-inputd', ['src/main.c', 'src/math.c'],
	install: true,
	include_directories: [include_directories('include')],
	dependencies: [gdk, gdk_x11, glib, libdevident, libinput, meson.get_compiler('c').find_library('m')])
