cmake_minimum_required(VERSION 3.11)
project(CarSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v5.0.0 # Wersja biblioteki
)
FetchContent_MakeAvailable(ftxui)
add_executable(program 
  main.cpp
  Car.hpp 
  Engine.hpp 
  Gearbox.hpp 
  Exceptions.hpp
)
target_link_libraries(program
  PRIVATE 
  ftxui::screen
  ftxui::dom
  ftxui::component
)