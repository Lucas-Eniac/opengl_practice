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

if(WIN32)
	if(MINGW)
		set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-mingw-w64")
		set(GLFW_LIB_FILE "${GLFW_LIB_DIR}/libglfw3.a")
	elseif(MSVC)
		if(MSVC_TOOLSET_VERSION GREATER_EQUAL 143)
			set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-vc2022")
		elseif(MSVC_TOOLSET_VERSION EQUAL 142)
			set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-vc2019")
		elseif(MSVC_TOOLSET_VERSION EQUAL 141)
			set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-vc2017")
		elseif(MSVC_TOOLSET_VERSION EQUAL 140)
			set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-vc2015")
		else()
			set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/3rds/glfw/lib-vc2022")
		endif()

		set(GLFW_LIB_FILE "${GLFW_LIB_DIR}/glfw3.lib")
	else()
		message(FATAL_ERROR "Unsupported Windows compiler. Use MinGW or MSVC.")
	endif()

	if(NOT EXISTS "${GLFW_LIB_FILE}")
		message(FATAL_ERROR "GLFW library not found: ${GLFW_LIB_FILE}")
	endif()

	target_link_libraries(eniac_opengl PRIVATE
		"${GLFW_LIB_FILE}"
		opengl32
		gdi32
		user32
		shell32
	)

	if(EXISTS "${GLFW_LIB_DIR}/glfw3.dll")
		add_custom_command(TARGET eniac_opengl POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${GLFW_LIB_DIR}/glfw3.dll"
				"$<TARGET_FILE_DIR:eniac_opengl>"
		)
	endif()
else()
	message(FATAL_ERROR "This project currently ships bundled GLFW binaries for Windows only.")
endif()

# 确保在编译时自动拷贝 shader 目录到可执行文件所在目录
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