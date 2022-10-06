project('fps_recoil_auto_control', 'c',
    version : '1.0.0',
    license : 'MIT/Expat'
)

### dependency
# libhidapi
hidapi = subproject('hidapi') # 2022 10 05 master branch
libhidapi_dep = hidapi.get_variable('hidapi_build').dependency('hidapi_libusb') # libusb backend

# math
math_dep = declare_dependency(
    link_args: ['-lm']
)

### source
src_fps_recoil_auto_control = [
    'src/agp_gen.c',
    'src/auto_gun_press.c',
    'src/hid.c',
    'src/input.c',
    'src/output.c',
    'src/main.c',
]

### includes
include_fps_recoil_auto_control = [
    'src/',
]

### c_args
c_args_fps_recoil_auto_control = ''
if build_machine.endian() == 'big'
    c_args_fps_recoil_auto_control += '-DHOST_WORDS_BIGENDIAN'
endif

executable('fps_recoil_auto_control',
    src_fps_recoil_auto_control,
    dependencies : [
        libhidapi_dep,
        math_dep
    ],
    include_directories : include_fps_recoil_auto_control,
    c_args : c_args_fps_recoil_auto_control
)
