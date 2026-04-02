cmake_minimum_required(VERSION 3.10)
project(eniac_opengl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB ELEMENTS_SOURCES "elements/*.cpp")

add_executable(eniac_opengl 
				main.cpp
				${ELEMENTS_SOURCES}
				${CMAKE_SOURCE_DIR}/3rds/glad/src/glad.c)

target_include_directories(eniac_opengl PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(eniac_opengl PRIVATE ${CMAKE_SOURCE_DIR}/3rds)
target_include_directories(eniac_opengl PRIVATE ${CMAKE_SOURCE_DIR}/3rds/glfw/include)
target_include_directories(eniac_opengl PRIVATE ${CMAKE_SOURCE_DIR}/3rds/glad/include)
target_include_directories(eniac_opengl PRIVATE ${CMAKE_SOURCE_DIR}/3rds/glm)
target_include_directories(eniac_opengl PRIVATE ${CMAKE_SOURCE_DIR}/3rds/assimp/include)


set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-mingw-w64")
set(GLFW_LIB_FILE "${GLFW_LIB_DIR}/libglfw3.a")
set(ASSIMP_LIB_FILE "${CMAKE_SOURCE_DIR}/3rds/assimp/lib/libassimp.dll.a")
set(ASSIMP_UNIT_LIB_FILE "${CMAKE_SOURCE_DIR}/3rds/assimp/lib/libunit.dll.a")

target_link_libraries(eniac_opengl PRIVATE
	"${GLFW_LIB_FILE}"
	opengl32
	gdi32
	user32
	shell32
	"${ASSIMP_LIB_FILE}"
	"${ASSIMP_UNIT_LIB_FILE}"
)

if(EXISTS "${GLFW_LIB_DIR}/glfw3.dll")
	add_custom_command(TARGET eniac_opengl POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${GLFW_LIB_DIR}/glfw3.dll"
			"$<TARGET_FILE_DIR:eniac_opengl>"
	)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/3rds/assimp/lib/libassimp-6.dll")
	add_custom_command(TARGET eniac_opengl POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_SOURCE_DIR}/3rds/assimp/lib/libassimp-6.dll"
			"$<TARGET_FILE_DIR:eniac_opengl>"
	)
endif()

add_custom_command(TARGET eniac_opengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/glsl"
        "$<TARGET_FILE_DIR:eniac_opengl>/glsl"
)

add_custom_command(TARGET eniac_opengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:eniac_opengl>/res"
)