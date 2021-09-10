cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(cardboard C CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif()


################# Flags ################

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(BUILD_C_FLAGS "${BUILD_C_FLAGS} -fomit-frame-pointer -Wall -fsigned-char")
set(BUILD_CXX_FLAGS "${BUILD_CXX_FLAGS} -fno-exceptions -fno-rtti")


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BUILD_TARGET_X64 1)
else()
    set(BUILD_TARGET_X86 1)
endif()

set(BUILD_DEBUG_FLAGS   "${BUILD_FLAGS} -D_DEBUG -DBUILD_DEBUG -DCLIENT")
set(BUILD_RELEASE_FLAGS "${BUILD_FLAGS} -DNDEBUG -DBUILD_NO_DEBUG")

set(BUILD_C_DEBUG_FLAGS "${BUILD_C_FLAGS} ${BUILD_DEBUG_FLAGS}")
set(BUILD_CXX_DEBUG_FLAGS "${BUILD_CXX_FLAGS} ${BUILD_DEBUG_FLAGS}")

set(BUILD_C_RELEASE_FLAGS "${BUILD_C_FLAGS} ${BUILD_RELEASE_FLAGS}")
set(BUILD_CXX_RELEASE_FLAGS "${BUILD_CXX_FLAGS} ${BUILD_RELEASE_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${BUILD_C_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${BUILD_CXX_DEBUG_FLAGS}")
message(STATUS "Set debug mode flags (${BUILD_DEBUG_FLAGS}) (Current mode: ${CMAKE_BUILD_TYPE}, compiler: ${CMAKE_CXX_COMPILER_ID})")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${BUILD_C_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BUILD_CXX_RELEASE_FLAGS}")
message(STATUS "Set release mode flags (${BUILD_RELEASE_FLAGS}) (Current mode: ${CMAKE_BUILD_TYPE}, compiler: ${CMAKE_CXX_COMPILER_ID})")



################ Files ################

include_directories(src/shared src/engine src/fpsgame src/include src/enet/include)

add_executable(native_client
        src/shared/crypto.cpp
        src/shared/geom.cpp
        src/shared/glemu.cpp
        src/shared/stream.cpp
        src/shared/tools.cpp
        src/shared/zip.cpp
        src/shared/microtar.c
        src/shared/cJSON.c
        src/shared/b64/buffer.c
        src/shared/b64/encode.c
        src/shared/b64/decode.c
        src/engine/3dgui.cpp
        src/engine/bih.cpp
        src/engine/blend.cpp
        src/engine/blob.cpp
        src/engine/client.cpp
        src/engine/command.cpp
        src/engine/console.cpp
        src/engine/decal.cpp
        src/engine/dynlight.cpp
        src/engine/gamepad.cpp
        src/engine/glare.cpp
        src/engine/grass.cpp
        src/engine/lightmap.cpp
        src/engine/main.cpp
        src/engine/material.cpp
        src/engine/menus.cpp
        src/engine/normal.cpp
        src/engine/octa.cpp
        src/engine/octaedit.cpp
        src/engine/octarender.cpp
        src/engine/physics.cpp
        src/engine/pvs.cpp
        src/engine/raw.cpp
        src/engine/rendergl.cpp
        src/engine/rendermodel.cpp
        src/engine/renderparticles.cpp
        src/engine/rendersky.cpp
        src/engine/rendertext.cpp
        src/engine/renderva.cpp
        src/engine/server.cpp
        src/engine/serverbrowser.cpp
        src/engine/shader.cpp
        src/engine/shadowmap.cpp
        src/engine/sound.cpp
        src/engine/texture.cpp
        src/engine/translate.cpp
        src/engine/water.cpp
        src/engine/world.cpp
        src/engine/worldio.cpp
        src/fpsgame/ai.cpp
        src/fpsgame/client.cpp
        src/fpsgame/discord.cpp
        src/fpsgame/entities.cpp
        src/fpsgame/fps.cpp
        src/fpsgame/fragmessages.cpp
        src/fpsgame/render.cpp
        src/fpsgame/scoreboard.cpp
        src/fpsgame/server.cpp
        src/fpsgame/weaponstats.cpp
        src/fpsgame/waypoint.cpp
        src/fpsgame/weapon.cpp
        )


target_compile_definitions(native_client PUBLIC CLIENT)

add_executable(native_server
        src/shared/crypto.cpp
        src/shared/stream.cpp
        src/shared/tools.cpp
        src/shared/cJSON.c
        src/shared/microtar.c
        src/shared/b64/buffer.c
        src/shared/b64/encode.c
        src/shared/b64/decode.c
        src/engine/command.cpp
        src/engine/server.cpp
        src/engine/worldio.cpp
        src/fpsgame/entities.cpp
        src/fpsgame/server.cpp
        )

target_compile_definitions(native_server PUBLIC STANDALONE)

add_executable(native_master
        src/shared/crypto.cpp
        src/shared/stream.cpp
        src/shared/tools.cpp
        src/shared/cJSON.c
        src/shared/microtar.c
        src/shared/b64/buffer.c
        src/shared/b64/encode.c
        src/shared/b64/decode.c
        src/engine/command.cpp
        src/engine/master.cpp
        )

target_compile_definitions(native_master PUBLIC STANDALONE)

list(APPEND cardboard_link_libraries_client "GL" "enet" "SDL2_image" "SDL2_mixer" "z" "curl" "X11" "SDL2" "rt")
list(APPEND cardboard_link_libraries_server "enet" "curl" "z")
list(APPEND cardboard_link_libraries_master "enet" "curl" "z")


target_link_libraries(native_client ${cardboard_link_libraries_client})
target_link_libraries(native_server ${cardboard_link_libraries_server})
target_link_libraries(native_master ${cardboard_link_libraries_master})


################ Install targets ################
set(cardboard_dir "${PROJECT_SOURCE_DIR}/bin_unix")

install(TARGETS native_client native_server native_master RUNTIME DESTINATION ${cardboard_dir})
