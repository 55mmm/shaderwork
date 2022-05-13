cmake_minimum_required(VERSION 3.16)
project(UpworkShaders)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(Boost REQUIRED)
find_package(Freetype REQUIRED)

set(SOURCE_FILES
    UpworkShaders/App.cpp
    UpworkShaders/Camera.cpp
    UpworkShaders/Main.cpp
    UpworkShaders/Mesh.cpp
    UpworkShaders/Node.cpp
    UpworkShaders/PointLight.cpp
    UpworkShaders/QuadMesh.cpp
    UpworkShaders/Renderer.cpp
    UpworkShaders/SceneLoader.cpp
    UpworkShaders/Shader.cpp
    UpworkShaders/Texture.cpp
    UpworkShaders/TexturedMesh.cpp
)

set(HEADER_FILES
    UpworkShaders/App.h
    UpworkShaders/Camera.h
    UpworkShaders/Main.h
    UpworkShaders/Mesh.h
    UpworkShaders/Node.h
    UpworkShaders/PointLight.h
    UpworkShaders/QuadMesh.h
    UpworkShaders/Renderer.h
    UpworkShaders/SceneLoader.h
    UpworkShaders/Settings.h
    UpworkShaders/Shader.h
    UpworkShaders/stb_image.h
    UpworkShaders/TexturedMesh.h
    UpworkShaders/Texture.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${USR_INCLUDE})

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC ${FREETYPE_INCLUDE_DIRS})

target_link_directories(${PROJECT_NAME} PUBLIC "/home/michaelmoerschell/Documents/libchess/build/static/")
target_link_directories(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} glfw ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${Boost_LIBRARIES} assimp ${FREETYPE_LIBRARIES})
