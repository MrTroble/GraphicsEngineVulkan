set(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# update current positions
set(PROJECT_SRC_DIR ${WORKING_DIRECTORY}Src/)
set(PROJECT_INCLUDE_DIR ${WORKING_DIRECTORY}include/)
set(EXTERNAL_LIB_SRC_DIR ${WORKING_DIRECTORY}ExternalLib/)
set(SHADER_SRC_DIR ${WORKING_DIRECTORY}Resources/Shaders/)

include(${WORKING_DIRECTORY}cmake/filters/SetShaderFilters.cmake)
include(${WORKING_DIRECTORY}cmake/filters/SetProjectFilters.cmake)
include(${WORKING_DIRECTORY}cmake/filters/SetExternalLibsFilters.cmake)

include(cmake/SetTestFilters.cmake)

set (COMPILE_TEST_SUITE compileTestSuite)

add_executable( ${COMPILE_TEST_SUITE}
                # all test specific files -- start
                ${RENDERER_COMPILE_TEST_FILTER}
                # all test specific files -- end
                $<TARGET_OBJECTS:PROJECT_FILES_TO_TEST>
                $<TARGET_OBJECTS:IMGUI>
)

target_include_directories(  ${COMPILE_TEST_SUITE} PUBLIC   ${WORKING_DIRECTORY}include/renderer
                                                            ${WORKING_DIRECTORY}include/renderer/pushConstants
                                                            ${WORKING_DIRECTORY}include/renderer/accelerationStructures
                                                            ${WORKING_DIRECTORY}include/vulkan_base
                                                            ${WORKING_DIRECTORY}include/gui
                                                            ${WORKING_DIRECTORY}include/common
                                                            ${WORKING_DIRECTORY}include/scene
                                                            ${WORKING_DIRECTORY}include/window
                                                            ${WORKING_DIRECTORY}include/memory
                                                            ${WORKING_DIRECTORY}include/util
                                                            ${WORKING_DIRECTORY}include/app
                                                            ${WORKING_DIRECTORY}Resources/Shaders/hostDevice
                                                            ${WORKING_DIRECTORY}include/
)

target_include_directories(  ${COMPILE_TEST_SUITE} PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries( ${COMPILE_TEST_SUITE}  PUBLIC    gtest_main
                                                        ${CMAKE_DL_LIBS}
                                                        Threads::Threads
                                                        Vulkan::Vulkan
                                                        glfw 
                                                        imgui 
                                                        stb
                                                        glm
                                                        tinyobjloader
                                                        vma
                                                        ktx
)


# disable all warnings for our test suite
if(MSVC)
  target_compile_options(${COMPILE_TEST_SUITE} INTERFACE /w)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(${COMPILE_TEST_SUITE} INTERFACE -w)
else()
  target_compile_options(${COMPILE_TEST_SUITE} INTERFACE -w)
endif()


include(GoogleTest)
gtest_discover_tests(   ${COMPILE_TEST_SUITE})

# add test after each build
# src: https://quickturnstudio.com/running-test-in-compile-time/
add_custom_target(${COMPILE_TEST_SUITE}_Runner
                  COMMAND ${COMPILE_TEST_SUITE}
                  COMMENT "Running tests after each compilation.")

add_dependencies(${PROJECT_NAME} ${COMPILE_TEST_SUITE}_Runner)