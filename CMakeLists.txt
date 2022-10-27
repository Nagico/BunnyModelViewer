cmake_minimum_required(VERSION 3.23)
project(model-viewer)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)

include_directories(dependencies/include)
include_directories(dependencies/include/imgui)
add_subdirectory(src/util/event/eventbus)

message("OpenGL: ${OPENGL_LIBRARIES}")
message("Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
    if(${CMAKE_CXX_COMPILER} MATCHES "MSVC")
        add_compile_definitions(_AMD64_)
        add_compile_definitions(_WINDOWS)
        link_directories(dependencies/lib/msvc_x64)
        add_custom_command( OUTPUT COPY_ASSIMP
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/dependencies/lib/msvc_x64/assimp-vc143-mt.lib ${CMAKE_BINARY_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/dependencies/lib/msvc_x64/assimp-vc143-mt.dll ${CMAKE_BINARY_DIR}
                COMMENT "Copying assimp lib"
                )
    elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        link_directories(dependencies/lib/mingw_x64)
        add_custom_command( OUTPUT COPY_ASSIMP
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/dependencies/lib/mingw_x64/libassimp.a ${CMAKE_BINARY_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/dependencies/lib/mingw_x64/libassimp-5.dll ${CMAKE_BINARY_DIR}
                COMMENT "Copying assimp lib"
                )
    else()
        message(FATAL_ERROR "Unsupported compiler")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_custom_target(assimp ALL DEPENDS COPY_ASSIMP)

add_custom_command( OUTPUT COPY_ASSETS
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets"
)

add_custom_target(assets ALL DEPENDS COPY_ASSETS)

add_executable(
        model-viewer
        src/main.cpp
        dependencies/src/glad.c
        dependencies/src/imgui.cpp
        dependencies/src/imgui_demo.cpp
        dependencies/src/imgui_draw.cpp
        dependencies/src/imgui_impl_glfw.cpp
        dependencies/src/imgui_impl_opengl3.cpp
        dependencies/src/imgui_tables.cpp
        dependencies/src/imgui_widgets.cpp

        src/util/opengl/Camera.h
        src/util/opengl/Image.cpp
        src/util/opengl/Image.h
        src/util/opengl/Mesh.cpp
        src/util/opengl/Mesh.h
        src/util/opengl/ShaderProgram.cpp
        src/util/opengl/ShaderProgram.h
        src/util/opengl/Model.cpp
        src/util/opengl/Model.h
        src/util/opengl/OpenGLRender.h
        src/util/opengl/OpenGLWindow.cpp
        src/util/opengl/OpenGLWindow.h

        src/MainRender.cpp
        src/MainRender.h
        src/MainWindow.cpp
        src/MainWindow.h

        src/util/opengl/Camera.cpp
        src/util/RayPicker.cpp
        src/util/RayPicker.h
        src/util/event/Mouse.cpp
        src/util/event/Mouse.h
        src/util/event/Event.h
        src/util/event/enum.h
        src/util/opengl/Polygon.cpp
        src/util/opengl/Polygon.h
        src/util/opengl/PolygonPoint.cpp
        src/util/opengl/PolygonPoint.h
        src/util/opengl/PolygonTriangle.cpp
        src/util/opengl/PolygonTriangle.h
        src/util/event/enum.cpp
        src/util/event/Keyboard.cpp
        src/util/event/Keyboard.h
        src/util/nfd/nfd.h
        src/util/nfd/nfd_common.c
        src/util/nfd/nfd_common.h
        src/util/nfd/nfd_win.cpp
        src/util/nfd/simple_exec.h
)

target_compile_options(model-viewer
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
    if(${CMAKE_CXX_COMPILER} MATCHES "MSVC")
        target_link_libraries(model-viewer glfw3 opengl32 assimp-vc143-mt Dexode::EventBus)
    elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        target_link_libraries(model-viewer glfw3 opengl32 libassimp Dexode::EventBus)
    else()
        message(FATAL_ERROR "Unsupported compiler")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_dependencies(model-viewer assets assimp)