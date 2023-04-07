cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

if(UNIX)
  set(CMAKE_CXX_FLAGS "-pthread")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -DDEBUG -pedantic -Wall -Wextra -Wconversion -Wreorder -Wuninitialized -Wtype-limits -Wno-pragmas")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wno-deprecated")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Wall /Zi /wd5045 /wd4514 /wd4820 /D_CRT_SECURE_NO_WARNINGS /D__STDC_WANT_SECURE_LIB__")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /W1 /DNDEBUG /D_CRT_SECURE_NO_WARNINGS /D__STDC_WANT_SECURE_LIB__")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -glldb")
endif()

get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "Compiler definitions:")
foreach(d ${DirDefs})
    message(STATUS "- " ${d})
endforeach()

project(automata
        VERSION 0.1
        DESCRIPTION "Cellular Automata Simulator"
        LANGUAGES CXX)

# set(BOOST_ROOT $ENV{BOOST_ROOT})
# find_package(Boost 1.81.0 REQUIRED COMPONENTS program_options)
# message(STATUS "Boost version: ${Boost_VERSION}")
# message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
# message(STATUS "Boost lib dirs: ${Boost_LIBRARY_DIRS}")
# message(STATUS "Boost libs: ${Boost_LIBRARIES}")

# SDL2
find_path(SDL2_INCLUDES
  NAMES SDL.h
  HINTS /opt/homebrew/include/SDL2 /usr/local/include/SDL2
  DOC "SDL2 header"
)
find_library(SDL2_LIBRARY
  SDL2
  HINTS /opt/homebrew /usr/local
  DOC "SDL library"
)
cmake_path(GET SDL2_LIBRARY PARENT_PATH SDL2_LIB_DIR)
find_library(SDL2_TTF_LIBRARY
  SDL2_ttf
  HINTS /opt/homebrew /usr/local
  DOC "SDL TTF library"
)

message(STATUS "SDL lib dir: ${SDL2_LIB_DIR}")
message(STATUS "SDL libraries: ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY}")
message(STATUS "SDL include dir: ${SDL2_INCLUDES}")

include_directories(${SDL2_INCLUDES})
link_directories(${SDL2_LIB_DIR})

add_executable(automata
  src/main.cpp
  src/util.cpp
  src/game-of-life.cpp
)
if(UNIX)
  set(PLATFORM_DEPENDENT_LIBRARIES, "-lpthread")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread -O0 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread -O2")
else()
  set(PLATFORM_DEPENDENT_LIBRARIES, "")
endif()

IF(CMAKE_BUILD_TYPE MATCHES Release)
  add_custom_command(TARGET automata
    POST_BUILD
    COMMAND strip automata)
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)

target_include_directories(automata
	PRIVATE ${PROJECT_INCLUDE_DIRS}
	# PUBLIC ${Boost_INCLUDE_DIRS}
)

target_link_libraries(automata
	# ${Boost_LIBRARIES}
  ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY}
)

install(TARGETS automata RUNTIME DESTINATION bin)
