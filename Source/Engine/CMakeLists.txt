# by Daniel Prihodko
cmake_minimum_required (VERSION 3.8)
project (Engine LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 14)

# use this for https://github.com/ocornut/imgui and others, potentially
# https://stackoverflow.com/questions/38006584/how-to-clone-and-integrate-external-from-git-cmake-project-into-local-one

find_package(Vulkan REQUIRED) 
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)
find_package(ALUT REQUIRED) # FindALUT.cmake from https://github.com/rpavlik/cmake-modules/blob/master/FindALUT.cmake

find_package(TBB REQUIRED) # https://github.com/01org/tbb/tree/tbb_2018/cmake

if (TBB_FOUND)
	set(TBB_INCLUDE_DIRS ${TBB_DIR}/../include)
endif(TBB_FOUND)

file(GLOB SRC "*.cpp" "*.h") 
file(GLOB SRC_COMP "Components/*.cpp" "Components/*.h")

add_library(${PROJECT_NAME} STATIC ${SRC} ${SRC_COMP})
if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/YuCommon.h")
   set_source_files_properties("Common.cpp" PROPERTIES COMPILE_FLAGS "/YcCommon.h")
   set(EXTRA_LIBS "IPHLPAPI.lib") # for UID class
endif(MSVC)

# just to make our life a little bit more organized, group files
source_group("" FILES ${SRC})
source_group("Components" FILES ${SRC_COMP})

target_include_directories(${PROJECT_NAME}
	PUBLIC . ${GLM_INCLUDE_DIR}
	PRIVATE extra ${GLFW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR}	${Vulkan_INCLUDE_DIR} ${ALUT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Physics
	${GLFW_LIBRARIES} ${OPENGL_LIBRARY} ${GLEW_LIBRARY}	${Vulkan_LIBRARY} ${ALUT_LIBRARIES} ${TBB_IMPORTED_TARGETS} ${EXTRA_LIBS})