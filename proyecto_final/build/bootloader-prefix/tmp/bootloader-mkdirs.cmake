# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/cesar/esp/esp-idf/components/bootloader/subproject"
  "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader"
  "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader-prefix"
  "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader-prefix/tmp"
  "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader-prefix/src"
  "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Programas/C/IoT/proyecto_final/proyecto_final/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
