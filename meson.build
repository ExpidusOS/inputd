project('expidus-inputd', 'c', license: 'GPL-3.0-only', version: '0.1.0-prealpha')

glib = dependency('glib-2.0')
libinput = dependency('libinput')
libudev = dependency('libudev')
inc = include_directories('include')

inputd = executable('expidus-inputd', ['src/main.c'],
	install: true,
	include_directories: inc,
	dependencies: [glib, libinput, libudev])
