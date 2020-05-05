project('hescape', 'c',
    version: '0.6.12'
)

hescape_inc = include_directories('.')
hescape_lib = shared_library('hescape',
    'hescape.c',
    include_directories: hescape_inc,
    install: true
)
hescape_static_lib = static_library('hescape',
    'hescape.c',
    include_directories: hescape_inc,
    install: true
)
install_headers('hescape.h')

hescape_dep = declare_dependency(link_with: hescape_lib,
    include_directories: hescape_inc)
hescape_static_dep = declare_dependency(link_with: hescape_static_lib,
    include_directories: hescape_inc)
