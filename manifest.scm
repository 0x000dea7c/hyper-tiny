(use-modules (guix profiles))

(specifications->manifest '("gcc-toolchain" "cmake" "sdl2" "gdb" "make" "glew" "autoconf" "automake" "autobuild" "pkg-config" "glibc:debug"))
