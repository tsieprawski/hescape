project('hescape', 'c',
    version: '0.6.12'
)

hescape_inc = include_directories('.')
hescape_lib = shared_library('hescape',
    'hescape.c',
    include_directories: hescape_inc
)

hescape_dep = declare_dependency(link_with: hescape_lib,
    include_directories: hescape_inc)
