# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

set(libs ${CMAKE_SOURCE_DIR}/src/main/jniLibs)
set(ThirdParts ${CMAKE_SOURCE_DIR}/src/main/ThirdParts)
set(JNIPath ${CMAKE_SOURCE_DIR}/src/main/cpp)

# include opencv head
include_directories(${ThirdParts}/Opencv/include)
# include customer's head
include_directories(${JNIPath}/include)

# add opencv lib
add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3 PROPERTIES IMPORTED_LOCATION ${libs}/${ANDROID_ABI}/libopencv_java3.so)
aux_source_directory(${JNIPath}/source source_dir)


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp ${source_dir})

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )
find_library( android-lib android)

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} ${android-lib} jnigraphics libopencv_java3)