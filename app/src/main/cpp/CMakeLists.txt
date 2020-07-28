cmake_minimum_required(VERSION 3.4.1)

set(PIXMAN_DIR ../../../../../../pixman)
file(GLOB PIXMAN_SRC
        "${PIXMAN_DIR}/pixman/*.c"
        "${PIXMAN_DIR}/pixman/*.S"
        "${PIXMAN_DIR}/pixman/*.cpp")

set(CAIRO_DIR ../../../../../../cairo)
file(GLOB CAIRO_SRC
        "${CAIRO_DIR}/src/*.c"
        "${CAIRO_DIR}/src/*.cpp")

list(FILTER CAIRO_SRC EXCLUDE REGEX .*-cogl.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-directfb.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-ft.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-glx.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*cairo-image-mask-compositor.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*cairo-gl-surface-legacy.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-os2.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-pdf.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-png.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-quartz.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-ps.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-script.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-vg.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-xcb.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*-xlib.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*check-has-hidden-symbols.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*xml.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*wgl.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*beos.*)
list(FILTER CAIRO_SRC EXCLUDE REGEX .*qt.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*egl.*)
#list(FILTER CAIRO_SRC EXCLUDE REGEX .*-gl-.*)

#message(WARNING "CAIRO_SRC aaaaaaa ${CAIRO_SRC}")

include_directories(${PIXMAN_DIR}/pixman ${CAIRO_DIR}/src include-extra)

#-D_USE_MATH_DEFINES -DPIXMAN_NO_TLS -DPACKAGE=\"android-cairo\" -DUSE_ARM_NEON -DUSE_ARM_SIMD -include \"limits.h\""
#-DPACKAGE_VERSION="\"android-cairo\"" -DPACKAGE_BUGREPORT="\"http://github.com/anoek/android-cairo\"" -DCAIRO_NO_MUTEX=1 -DHAVE_STDINT_H -DHAVE_UINT64_T

set(PIXMAN_FLAGS "-DPACKAGE=\"android-cairo\" -DPIXMAN_API=\"\" -DHAVE_STDINT_H -DHAVE_PTHREADS -include \"limits.h\"")
set(CAIRO_FLAGS "-DPACKAGE=\"android-cairo\" -DPIXMAN_API=\"\" -DHAVE_STDINT_H -DHAVE_UINT64_T -DCAIRO_NO_MUTEX=1")

add_library(pixman SHARED ${PIXMAN_SRC})
set_target_properties(pixman PROPERTIES COMPILE_FLAGS ${PIXMAN_FLAGS})

add_library(cairo SHARED ${CAIRO_SRC})
set_target_properties(cairo PROPERTIES COMPILE_FLAGS ${CAIRO_FLAGS})

add_library(main
        SHARED
        main.cpp
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

target_include_directories(main PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(main
        android
        EGL
        GLESv1_CM
        log)