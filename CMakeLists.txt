cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(RedArchiever)

add_executable(RedArchiever Sources/Main.cpp Sources/Archiever.cpp Sources/Core.cpp Sources/Encryptor.cpp Sources/Utils.cpp)