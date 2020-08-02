cmake_minimum_required(VERSION 3.4.1)

set(BUILD_SHARED_LIBS true)

#set(FT_DIR ../../../../../freetype2)
#add_subdirectory(${FT_DIR} freetype)
#
#set(PIXMAN_DIR ../../../../../pixman)
#file(GLOB PIXMAN_SRC
#        "${PIXMAN_DIR}/pixman/*.c"
#        "${PIXMAN_DIR}/pixman/*.S"
#        "${PIXMAN_DIR}/pixman/*.cpp")
#
#set(CAIRO_DIR ../../../../../cairo)
#file(GLOB CAIRO_SRC
#        "${CAIRO_DIR}/src/*.c"
#        "${CAIRO_DIR}/src/*.cpp")
#
#set(SKIA_DIR ../../../../../skia)
#file(GLOB_RECURSE SKIA_SRC
#        "${SKIA_DIR}/src/*.c"
#        "${SKIA_DIR}/src/*.cpp")
#
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*pixman-region.c.*)
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*sse2.*)
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*sse3.*)
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*vmx.*)
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*pixman-mips-dspr2.c.*)
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*pixman-arm-simd.*)
#list(FILTER PIXMAN_SRC EXCLUDE REGEX .*pixman-arm-neon.*)
#
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-cogl.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-directfb.*)
##list(FILTER CAIRO_SRC EXCLUDE REGEX .*-ft.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-glx.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*cairo-image-mask-compositor.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*cairo-gl-surface-legacy.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-os2.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-pdf.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-png.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-quartz.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-ps.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-script.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-vg.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-xcb.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-xlib.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*check-has-hidden-symbols.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*xml.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*wgl.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*beos.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*qt.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*svg.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*test-base-compositor-surface.c.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*cairo-type3-glyph-surface.c.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-tee.*)
##list(FILTER CAIRO_SRC EXCLUDE REGEX .*egl.*)
##list(FILTER CAIRO_SRC EXCLUDE REGEX .*-gl-.*)
#
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/codec/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/gpu/d3d/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/gpu/dawn/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/gpu/gl/glfw/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/gpu/gl/glx/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/gpu/vk/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/images/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/ports/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*/xml/.*)
#list(FILTER SKIA_SRC EXCLUDE REGEX .*Lua.*)
#
#
##message(WARNING "PIXMAN_SRC ${PIXMAN_SRC}")
##message(WARNING "CAIRO_SRC ${CAIRO_SRC}")
#
#include_directories(${PIXMAN_DIR}/pixman ${CAIRO_DIR}/src include-extra ${FT_DIR}/include ${SKIA_DIR}/include ${SKIA_DIR}/src ${SKIA_DIR})
#
##-D_USE_MATH_DEFINES -DPIXMAN_NO_TLS -DPACKAGE=\"android-cairo\" -DUSE_ARM_NEON -DUSE_ARM_SIMD -include \"limits.h\""
##-DPACKAGE_VERSION="\"android-cairo\"" -DPACKAGE_BUGREPORT="\"http://github.com/anoek/android-cairo\"" -DCAIRO_NO_MUTEX=1 -DHAVE_STDINT_H -DHAVE_UINT64_T
#
#set(PIXMAN_FLAGS "-DPACKAGE=\"android-cairo\" -DPIXMAN_API=\"\" -DHAVE_STDINT_H -DHAVE_PTHREADS -include \"limits.h\"")
#set(CAIRO_FLAGS "-DPACKAGE=\"android-cairo\" -DPIXMAN_API=\"\" -DHAVE_STDINT_H -DHAVE_UINT64_T -DCAIRO_NO_MUTEX=1")
#
#
##add_library(pixman SHARED ${PIXMAN_SRC})
#add_library(pixman SHARED ${PIXMAN_SRC})
#set_target_properties(pixman PROPERTIES COMPILE_FLAGS ${PIXMAN_FLAGS})
#
#
##add_library(cairo SHARED ${CAIRO_SRC})
#add_library(cairo SHARED ${CAIRO_SRC})
#set_target_properties(cairo PROPERTIES COMPILE_FLAGS ${CAIRO_FLAGS})
#target_link_libraries(cairo
#        freetype
#        pixman
#        EGL
#        GLESv2)


#add_library(skia SHARED ${SKIA_SRC})


#set(LIBFONT_DIR ../../../../../libfont)
#file(GLOB LIBFONT_SRC
#        "${LIBFONT_DIR}/jni/*.cpp")

#message(WARNING "LIBFONT_SRC ${LIBFONT_SRC}")


#add_library(libfont SHARED ${LIBFONT_SRC})
#target_include_directories(libfont PUBLIC
#        ${LIBFONT_DIR}/jni
#        ${FT_DIR}/include)
#target_link_libraries(libfont
#        freetype
#        GLESv1_CM
#        log)


add_library(main
        SHARED
        main.cpp
        Canvas.cpp
        Container.cpp
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

target_include_directories(main PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

#set_target_properties(main PROPERTIES COMPILE_FLAGS "-Wno-c++11-narrowing")

target_link_libraries(main
        android
#        cairo
        EGL
#        GLESv2
        GLESv1_CM
        log)