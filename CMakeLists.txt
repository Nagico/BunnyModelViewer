cmake_minimum_required(VERSION 3.23)
project(test)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)

include_directories(dependencies/include)

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
        test
        src/main.cpp
        dependencies/src/glad.c

        src/util/Camera.h
        src/util/Image.cpp
        src/util/Image.h
        src/util/Mesh.cpp
        src/util/Mesh.h
        src/util/ShaderProgram.cpp
        src/util/ShaderProgram.h
        src/util/Model.cpp
        src/util/Model.h
        
#        src/util/OpenGLRender.h
#        src/util/OpenGLWindow.cpp
#        src/util/OpenGLWindow.h
#        src/MainRender.cpp
#        src/MainRender.h
#        src/MainWindow.cpp
#        src/MainWindow.h
        src/util/Camera.cpp)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
    if(${CMAKE_CXX_COMPILER} MATCHES "MSVC")
        target_link_libraries(test glfw3 opengl32 assimp-vc143-mt)
    elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        target_link_libraries(test glfw3 opengl32 libassimp)
    else()
        message(FATAL_ERROR "Unsupported compiler")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_dependencies(test assets assimp)