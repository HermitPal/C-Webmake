cmake_minimum_required(VERSION 3.14)
project(MyMD4CProject C)

# Add the MD4C submodule
add_subdirectory(external/md4c)

# Add your executable
add_executable(my_md4c_app main.c)

# Link MD4C library to your executable
target_link_libraries(my_md4c_app md4c)

# Include the MD4C headers
target_include_directories(my_md4c_app PRIVATE external/md4c/src)
