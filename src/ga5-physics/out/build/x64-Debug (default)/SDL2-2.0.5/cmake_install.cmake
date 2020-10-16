# Install script for directory: D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/ga5-physics/out/install/x64-Debug (default)")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/ga5-physics/out/build/x64-Debug (default)/SDL2-2.0.5/SDL2.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/ga5-physics/out/build/x64-Debug (default)/SDL2-2.0.5/SDL2main.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_assert.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_atomic.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_audio.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_bits.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_blendmode.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_clipboard.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_android.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_iphoneos.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_macosx.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_minimal.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_pandora.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_psp.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_windows.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_winrt.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_config_wiz.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_copying.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_cpuinfo.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_egl.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_endian.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_error.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_events.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_filesystem.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_gamecontroller.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_gesture.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_haptic.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_hints.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_joystick.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_keyboard.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_keycode.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_loadso.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_log.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_main.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_messagebox.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_mouse.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_mutex.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_name.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengl.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengl_glext.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengles.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengles2.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengles2_gl2.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengles2_gl2ext.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengles2_gl2platform.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_opengles2_khrplatform.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_pixels.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_platform.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_power.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_quit.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_rect.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_render.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_revision.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_rwops.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_scancode.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_shape.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_stdinc.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_surface.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_system.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_syswm.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_assert.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_common.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_compare.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_crc32.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_font.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_fuzzer.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_harness.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_images.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_log.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_md5.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_test_random.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_thread.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_timer.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_touch.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_types.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_version.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/SDL_video.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/begin_code.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/include/close_code.h"
    "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/ga5-physics/out/build/x64-Debug (default)/SDL2-2.0.5/include/SDL_config.h"
    )
endif()

