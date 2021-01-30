project('expidus-inputd', 'c', license: 'GPL-3.0-only', version: '0.1.0-prealpha')

glib = dependency('glib-2.0')
libinput = dependency('libinput')
libudev = dependency('libudev')

inputd = executable('expidus-inputd', ['src/main.c'],
	install: true,
	dependencies: [glib, libinput, libudev])
