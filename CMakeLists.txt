cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(cardboard C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs) # Default module for install paths

################ Options ###############

option(BUILD_EDITOR "Include the built-in editor when building" OFF)
if(NOT ${BUILD_EDITOR})
	set(OPTIONAL_DEFINES "DNO_EDITOR")
endif()

################# Flags ################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

################ Files ################

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${SOURCE_DIR}/shared" "${SOURCE_DIR}/engine" "${SOURCE_DIR}/fpsgame" "${SOURCE_DIR}/include" "${SOURCE_DIR}/enet/include")
set(SHARED_SOURCE_DIR "${SOURCE_DIR}/shared")

set(COMMON_SOURCE_FILES
	"${SHARED_SOURCE_DIR}/crypto.cpp"
	"${SHARED_SOURCE_DIR}/stream.cpp"
	"${SHARED_SOURCE_DIR}/tools.cpp"
	"${SHARED_SOURCE_DIR}/microtar.c"
	"${SHARED_SOURCE_DIR}/cJSON.c"
	"${SHARED_SOURCE_DIR}/b64/buffer.c"
	"${SHARED_SOURCE_DIR}/b64/encode.c"
	"${SHARED_SOURCE_DIR}/b64/decode.c"
)

set(CLIENT_SOURCE_FILES
	${COMMON_SOURCE_FILES}
	"src/shared/geom.cpp"
	"src/shared/glemu.cpp"
	"src/shared/zip.cpp"
	"src/engine/3dgui.cpp"
	"src/engine/bih.cpp"
	"src/engine/blend.cpp"
	"src/engine/blob.cpp"
	"src/engine/client.cpp"
	"src/engine/command.cpp"
	"src/engine/console.cpp"
	"src/engine/decal.cpp"
	"src/engine/dynlight.cpp"
	"src/engine/gamepad.cpp"
	"src/engine/glare.cpp"
	"src/engine/grass.cpp"
	"src/engine/lightmap.cpp"
	"src/engine/main.cpp"
	"src/engine/material.cpp"
	"src/engine/menus.cpp"
	"src/engine/normal.cpp"
	"src/engine/octa.cpp"
	"src/engine/octaedit.cpp"
	"src/engine/octarender.cpp"
	"src/engine/physics.cpp"
	"src/engine/pvs.cpp"
	"src/engine/raw.cpp"
	"src/engine/rendergl.cpp"
	"src/engine/rendermodel.cpp"
	"src/engine/renderparticles.cpp"
	"src/engine/rendersky.cpp"
	"src/engine/rendertext.cpp"
	"src/engine/renderva.cpp"
	"src/engine/server.cpp"
	"src/engine/serverbrowser.cpp"
	"src/engine/shader.cpp"
	"src/engine/shadowmap.cpp"
	"src/engine/sound.cpp"
	"src/engine/texture.cpp"
	"src/engine/translate.cpp"
	"src/engine/water.cpp"
	"src/engine/world.cpp"
	"src/engine/worldio.cpp"
	"src/fpsgame/ai.cpp"
	"src/fpsgame/client.cpp"
	"src/fpsgame/discord.cpp"
	"src/fpsgame/entities.cpp"
	"src/fpsgame/fps.cpp"
	"src/fpsgame/fragmessages.cpp"
	"src/fpsgame/render.cpp"
	"src/fpsgame/scoreboard.cpp"
	"src/fpsgame/server.cpp"
	"src/fpsgame/weaponstats.cpp"
	"src/fpsgame/waypoint.cpp"
	"src/fpsgame/weapon.cpp"
)

set(SERVER_SOURCE_FILES
	${COMMON_SOURCE_FILES}
	"src/engine/command.cpp"
	"src/engine/server.cpp"
	"src/engine/worldio.cpp"
	"src/fpsgame/entities.cpp"
	"src/fpsgame/server.cpp"
)

set(MASTER_SOURCE_FILES
	${COMMON_SOURCE_FILES}
	"src/engine/command.cpp"
	"src/engine/master.cpp"
)

############# Dependencies #############

find_package(OpenGL REQUIRED)
find_library(enet NAMES enet REQUIRED)
if(WIN32)
	# enet needs sockets, and that means winsock on win32
	# enet also needs winmm on win32
	set(socks "wsock32" "ws2_32" "winmm")
endif()
find_library(SDL2 NAMES SDL2 REQUIRED)
find_library(SDL2_image NAMES SDL2_image REQUIRED)
find_library(SDL2_mixer NAMES SDL2_mixer REQUIRED)
#find_library(SDL2_ttf NAMES SDL2_ttf REQUIRED)
find_library(z NAMES z zlib REQUIRED)
find_library(curl NAMES curl REQUIRED)

############# Build targets ############

add_executable(native_client WIN32 ${CLIENT_SOURCE_FILES})
target_include_directories(native_client PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(native_client PUBLIC CLIENT ${OPTIONAL_DEFINES})
target_link_libraries(native_client PRIVATE ${OPENGL_LIBRARIES} ${enet} ${socks} ${SDL2} ${SDL2_image} ${SDL2_mixer} ${z} ${curl})
target_sources(native_client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/vcpp/cardboard.rc")

add_executable(native_server WIN32 ${SERVER_SOURCE_FILES})
target_include_directories(native_server PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(native_server PUBLIC STANDALONE ${OPTIONAL_DEFINES})
target_link_libraries(native_server PRIVATE ${enet} ${socks} ${curl} ${z})
target_sources(native_server PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/vcpp/cardboard.rc")

add_executable(native_master ${MASTER_SOURCE_FILES})
target_include_directories(native_master PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(native_master PUBLIC STANDALONE ${OPTIONAL_DEFINES})
target_link_libraries(native_master PRIVATE ${enet} ${socks} ${curl} ${z})
target_sources(native_master PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/vcpp/cardboard.rc")

################ Install targets ################

set(cardboard_dir "${PROJECT_SOURCE_DIR}/bin_unix")
install(TARGETS native_client native_server native_master RUNTIME DESTINATION ${cardboard_dir})
