project('vkquake', 'c', 'cpp', default_options : ['cpp_std=c++20', 'buildtype=release'])

shaders = [
    'Shaders/alias.frag',
    'Shaders/alias.vert',
    'Shaders/alias_alphatest.frag',
    'Shaders/md5.vert',
    'Shaders/basic.frag',
    'Shaders/basic.vert',
    'Shaders/basic_alphatest.frag',
    'Shaders/basic_notex.frag',
    'Shaders/cs_tex_warp.comp',
    'Shaders/indirect.comp',
    'Shaders/indirect_clear.comp',
    'Shaders/postprocess.frag',
    'Shaders/postprocess.vert',
    'Shaders/screen_effects_10bit.comp',
    'Shaders/screen_effects_10bit_scale.comp',
    'Shaders/screen_effects_10bit_scale_sops.comp',
    'Shaders/screen_effects_8bit.comp',
    'Shaders/screen_effects_8bit_scale.comp',
    'Shaders/screen_effects_8bit_scale_sops.comp',
    'Shaders/showtris.frag',
    'Shaders/showtris.vert',
    'Shaders/sky_box.frag',
    'Shaders/sky_cube.frag',
    'Shaders/sky_cube.vert',
    'Shaders/sky_layer.frag',
    'Shaders/sky_layer.vert',
    'Shaders/update_lightmap.comp',
    'Shaders/update_lightmap_rt.comp',
    'Shaders/world.frag',
    'Shaders/world.vert',
    'Shaders/ray_debug.comp',
]

glslang = find_program('glslangValidator')
spirv_opt = find_program('spirv-opt')
spirv_remap = find_program('spirv-remap')
bintoc = executable('bintoc', ['Shaders/bintoc.c'], native: true)
if (build_machine.system() == 'darwin') or get_option('buildtype').startswith('debug')
    spirv_opt_command = [] # MoltenVK spirv-cross has a bug that breaks optimized shaders
else
    spirv_opt_command = [spirv_opt, '-Os', '@PLAINNAME@.spv', '-o', '@PLAINNAME@.spv', '&&']
endif
spirv_remap_command = get_option('buildtype').startswith('debug') ? [] : [spirv_remap, '-s', '-o', '.', '-i', '@PLAINNAME@.spv', '&&']
bintoc_command = [bintoc, '@PLAINNAME@.spv', '@PLAINNAME@_spv', '@OUTPUT@']

shaders_c = []
foreach shader : shaders
    target_env = shader.contains('sops') ? ['--target-env', 'vulkan1.1'] : []
    debug_flag = get_option('buildtype').startswith('debug') ? ['-g'] : []
    glslang_command = [glslang, '-V'] + debug_flag + target_env + ['-o', '@PLAINNAME@.spv', '@INPUT@', '--depfile', '@PLAINNAME@.deps', '&&']
    shaders_c += custom_target(shader.underscorify(),
                 input: shader,
                 output: '@PLAINNAME@.c',
                 depfile: '@PLAINNAME@.deps',
                 command: glslang_command
                          + spirv_opt_command
                          + spirv_remap_command
                          + bintoc_command)
endforeach

srcs = [
    'Quake/bgmusic.cpp',
    'Quake/cd_null.cpp',
    'Quake/cfgfile.cpp',
    'Quake/chase.cpp',
    'Quake/cl_demo.cpp',
    'Quake/cl_input.cpp',
    'Quake/cl_main.cpp',
    'Quake/cl_parse.cpp',
    'Quake/cl_tent.cpp',
    'Quake/cmd.cpp',
    'Quake/common.cpp',
    'Quake/console.cpp',
    'Quake/crc.cpp',
    'Quake/cvar.cpp',
    'Quake/gl_draw.cpp',
    'Quake/gl_fog.cpp',
    'Quake/gl_heap.cpp',
    'Quake/gl_mesh.cpp',
    'Quake/gl_model.cpp',
    'Quake/gl_refrag.cpp',
    'Quake/gl_rlight.cpp',
    'Quake/gl_rmain.cpp',
    'Quake/gl_rmisc.cpp',
    'Quake/gl_screen.cpp',
    'Quake/gl_sky.cpp',
    'Quake/gl_texmgr.cpp',
    'Quake/gl_vidsdl.cpp',
    'Quake/gl_warp.cpp',
    'Quake/host.cpp',
    'Quake/host_cmd.cpp',
    'Quake/image.cpp',
    'Quake/in_sdl.cpp',
    'Quake/keys.cpp',
    'Quake/main_sdl.cpp',
    'Quake/mathlib.cpp',
    'Quake/mdfour.cpp',
    'Quake/mem.cpp',
    'Quake/menu.cpp',
    'Quake/miniz.cpp',
    'Quake/net_bsd.cpp',
    'Quake/net_dgrm.cpp',
    'Quake/net_loop.cpp',
    'Quake/net_main.cpp',
    'Quake/net_udp.cpp',
    'Quake/palette.cpp',
    'Quake/pl_linux.cpp',
    'Quake/pr_cmds.cpp',
    'Quake/pr_edict.cpp',
    'Quake/pr_exec.cpp',
    'Quake/pr_ext.cpp',
    'Quake/r_alias.cpp',
    'Quake/r_brush.cpp',
    'Quake/r_part.cpp',
    'Quake/r_part_fte.cpp',
    'Quake/r_sprite.cpp',
    'Quake/r_world.cpp',
    'Quake/sbar.cpp',
    'Quake/snd_codec.cpp',
    'Quake/snd_dma.cpp',
    'Quake/snd_mem.cpp',
    'Quake/snd_mix.cpp',
    'Quake/snd_sdl.cpp',
    'Quake/snd_umx.cpp',
    'Quake/snd_wave.cpp',
    'Quake/strlcat.cpp',
    'Quake/strlcpy.cpp',
    'Quake/sv_main.cpp',
    'Quake/sv_move.cpp',
    'Quake/sv_phys.cpp',
    'Quake/sv_user.cpp',
	'Quake/sys_sdl.cpp',
    'Quake/sys_sdl_unix.cpp',
    'Quake/tasks.cpp',
    'Quake/view.cpp',
    'Quake/wad.cpp',
    'Quake/world.cpp',
	'Quake/hash_map.cpp',
	'Quake/embedded_pak.cpp',
]

cppflags = ['-Wall', '-Wno-trigraphs', '-Wno-unused-function', '-Werror']
cc = meson.get_compiler('cpp')
deps = [
    cc.find_library('m', required : false),
    cc.find_library('dl', required : false),
    dependency('threads'),
    dependency('sdl2'),
]

if build_machine.system() == 'darwin'
    molten_dirs = []
    if import('fs').exists('/opt/homebrew/lib/libMoltenVK.dylib')
        molten_dirs += '/opt/homebrew/lib'
    endif
    deps += cc.find_library('MoltenVK', required : true, dirs : molten_dirs)
else
    deps += dependency('vulkan')
endif

if get_option('use_codec_wave').enabled()
    cppflags += '-DUSE_CODEC_WAVE'
endif

if get_option('mp3_lib') == 'mad'
    mp3_dep = dependency('mad', required : get_option('use_codec_mp3'))
    if mp3_dep.found()
        srcs += [ 'Quake/snd_mp3.cpp', 'Quake/snd_mp3tag.cpp' ]
        deps += mp3_dep
        cppflags += '-DUSE_CODEC_MP3'
    endif
else
    mp3_dep = dependency('libmpg123', required : get_option('use_codec_mp3'))
    if mp3_dep.found()
        srcs += [ 'Quake/snd_mpg123.cpp', 'Quake/snd_mp3tag.cpp' ]
        deps += mp3_dep
        cppflags += '-DUSE_CODEC_MP3'
    endif
endif

flac_dep = dependency('flac', required : get_option('use_codec_flac'))
if flac_dep.found()
    srcs += 'Quake/snd_flac.cpp'
    deps += flac_dep
    cppflags += '-DUSE_CODEC_FLAC'
endif

ogg_dep = dependency('ogg', required : get_option('use_codec_vorbis'))
if get_option('vorbis_lib') == 'vorbis'
    vorbisfile_dep = dependency('vorbisfile', required : get_option('use_codec_vorbis'))
    vorbis_dep = dependency('vorbis', required : get_option('use_codec_vorbis'))
    if ogg_dep.found() and vorbisfile_dep.found() and vorbis_dep.found()
        srcs += 'Quake/snd_vorbis.cpp'
        deps += [ ogg_dep, vorbisfile_dep, vorbis_dep ]
        cppflags += '-DUSE_CODEC_VORBIS'
    endif
else
    vorbis_dep = dependency('vorbisidec', required : get_option('use_codec_vorbis'))
    if ogg_dep.found() and vorbis_dep.found()
        srcs += 'Quake/snd_vorbis.cpp'
        deps += [ ogg_dep, vorbis_dep ]
        cppflags += [ '-DUSE_CODEC_VORBIS', '-DVORBIS_USE_TREMOR' ]
    endif
endif

opus_dep = dependency('opus', required : get_option('use_codec_opus'))
opusfile_dep = dependency('opusfile', required : get_option('use_codec_opus'))
if opus_dep.found() and opusfile_dep.found()
    srcs += 'Quake/snd_opus.cpp'
    deps += [ opus_dep, opusfile_dep ]
    cppflags += '-DUSE_CODEC_OPUS'
endif

if get_option('buildtype').startswith('debug')
    cppflags += '-D_DEBUG'
endif

if get_option('do_userdirs').enabled()
    cppflags += '-DDO_USERDIRS=1'
endif

executable('vkquake', [srcs, shaders_c], dependencies : deps, cpp_args : cppflags, cpp_pch: ['Quake/quakedef.h', 'Quake/quakedef.cpp'])
