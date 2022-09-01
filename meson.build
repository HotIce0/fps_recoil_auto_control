project('fps_recoil_auto_control', 'c',
    version : '1.0.0',
    license : 'MIT/Expat'
    )

libinput_dep = [dependency('libinput'), dependency('libudev')]

# source
src_fps_recoil_auto_control = [
    'src/main.c',
    'src/input.c',
    'src/output.c',
]
# includes
include_fps_recoil_auto_control = [
    'src/',
]
# c_args
c_args_fps_recoil_auto_control = ''
if build_machine.endian() == 'big'
    c_args_fps_recoil_auto_control += '-DHOST_WORDS_BIGENDIAN'
endif

executable('fps_recoil_auto_control',
    src_fps_recoil_auto_control,
    dependencies : libinput_dep,
    include_directories : include_fps_recoil_auto_control,
    c_args : c_args_fps_recoil_auto_control
    )
