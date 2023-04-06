cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(LIBCXX_ENABLE_INCOMPLETE_FEATURES)
if(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -DDEBUG -pedantic -Wall -Wextra -Wconversion -Wreorder -Wuninitialized -Wtype-limits -Wno-pragmas")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wno-deprecated")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Wall /Zi /wd5045 /wd4514 /wd4820 /D_CRT_SECURE_NO_WARNINGS /D__STDC_WANT_SECURE_LIB__")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /W1 /DNDEBUG /D_CRT_SECURE_NO_WARNINGS /D__STDC_WANT_SECURE_LIB__")
endif()

project(automata)

set(BOOST_ROOT $ENV{BOOST_ROOT})
find_package(Boost 1.81.0 REQUIRED COMPONENTS program_options filesystem)
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost lib dirs: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")

# SDL2
find_path(SDL2_C_INCLUDES
  NAMES SDL.h
  HINTS /opt/homebrew/include/SDL2 /usr/local/include/SDL2
  DOC "SDL2 header"
)
message(STATUS "SDL2 include dir: ${SDL2_C_INCLUDES}")
find_library(SDL2_C_LIBRARIES
  NAMES SDL2
  HINTS /opt/homebrew /usr/local
  DOC "SDL2 libraries"
)
cmake_path(GET SDL2_C_LIBRARIES PARENT_PATH SDL2_LIB_DIR)
message(STATUS "SDL2 lib dir: ${SDL2_LIB_DIR}")
include_directories(${SDL2_C_INCLUDES})
link_directories(${SDL2_LIB_DIR})

add_executable(automata
  main.cpp
  util.cpp
)
if(UNIX)
  set(PLATFORM_DEPENDENT_LIBRARIES, "-lpthread")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread -O0 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread -O2")
else()
  set(PLATFORM_DEPENDENT_LIBRARIES, "")
endif()

# add_custom_command(TARGET automata
#   POST_BUILD
#   COMMAND strip automata)

target_include_directories(automata
	PRIVATE ${PROJECT_INCLUDE_DIRS}
	PUBLIC ${Boost_INCLUDE_DIRS}
)

target_link_libraries(automata
	${Boost_LIBRARIES}
  ${SDL2_C_LIBRARIES}
)

install(TARGETS automata RUNTIME DESTINATION bin)
