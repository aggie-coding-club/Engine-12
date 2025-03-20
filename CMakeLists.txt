CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Name of the project
PROJECT(TBDEngine)

# Enable generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Get the list of all files.
FILE(GLOB_RECURSE SOURCES "src/*.cpp")
FILE(GLOB_RECURSE HEADERS "include/*.h")

message(STATUS "${SOURCES}")
message(STATUS "${HEADERS}")

# Set the executable.
message(STATUS "Including ${CMAKE_SOURCE_DIR}/include")
include_directories(${CMAKE_SOURCE_DIR}/include)
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

# Use FetchContent to download and build GLM, GLFW, and AngelScript

include(FetchContent)

# Fetch GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8  # Stable release
)
FetchContent_MakeAvailable(glm)
set(GLM_INCLUDE_DIRS ${glm_SOURCE_DIR})
message(STATUS "GLM include directory: ${GLM_INCLUDE_DIRS}")
include_directories(${GLM_INCLUDE_DIRS})

# Fetch GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8  # Stable release
)
FetchContent_MakeAvailable(glfw)
set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)
message(STATUS "GLFW include directory: ${GLFW_INCLUDE_DIRS}")
include_directories(${GLFW_INCLUDE_DIRS})

# Fetch GLEW
FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG glew-cmake-2.2.0  # Specify the version you want to use (e.g., "glew-2.2.0")
)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

# Fetch AngelScript
FetchContent_Declare(
        angelscript
        URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.37.0.zip
        URL_HASH MD5=175fb468bd9d115082f6143291e47301
)
FetchContent_MakeAvailable(angelscript)
set(ANGELSCRIPT_INCLUDE_DIRS ${angelscript_SOURCE_DIR})

# Dynamically collect all AngelScript core source files
file(GLOB ANGELSCRIPT_SRC "${angelscript_SOURCE_DIR}/angelscript/source/*.cpp")

# Dynamically collect all add-on source files (excluding sample folders if necessary)
file(GLOB ADDON_SRC "${angelscript_SOURCE_DIR}/add_on/scriptfile/*.cpp"
        "${angelscript_SOURCE_DIR}/add_on/scriptstdstring/*.cpp"
        "${angelscript_SOURCE_DIR}/add_on/scriptbuilder/*.cpp"
        "${angelscript_SOURCE_DIR}/add_on/scriptarray/*.cpp"
        "${angelscript_SOURCE_DIR}/add_on/datetime/*.cpp"
        "${angelscript_SOURCE_DIR}/add_on/serializer/*.cpp"
)

# Create a static library for AngelScript (including add-ons)
add_library(angelscript_static STATIC ${ANGELSCRIPT_SRC} ${ADDON_SRC})

message(STATUS "AngelScript include directory: ${ANGELSCRIPT_INCLUDE_DIRS}/angelscript/include")

# Set include directories for AngelScript and add-ons
include_directories(angelscript_static
        ${angelscript_SOURCE_DIR}/angelscript/include
        ${angelscript_SOURCE_DIR}/add_on
)
target_link_libraries(${CMAKE_PROJECT_NAME} angelscript_static)

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
    FetchContent_Populate(glew)

    add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
FetchContent_MakeAvailable(glew)
set(GLEW_INCLUDE_DIRS ${glew_SOURCE_DIR}/include)
message(STATUS "GLEW include directory: ${GLEW_INCLUDE_DIRS}")
target_link_libraries(${CMAKE_PROJECT_NAME} libglew_static)
#include_directories(${GLEW_INCLUDE_DIRS})

# Fetch fastgltf
FetchContent_Declare(
        fastgltf
        GIT_REPOSITORY https://github.com/spnda/fastgltf.git
        GIT_TAG v0.7.2  # Stable release
)
FetchContent_MakeAvailable(fastgltf)
# set(FASTGLTF_INCLUDE_DIRS ${fastgltf_SOURCE_DIR}/include)
# message(STATUS "fastgltf include directory: ${FASTGLTF_INCLUDE_DIRS}")
# include_directories(${FASTGLTF_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} fastgltf::fastgltf)

# Fetch fmt
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.2  # Stable release
)
FetchContent_MakeAvailable(fmt)
# set(FMT_INCLUDE_DIRS ${fmt_SOURCE_DIR}/include)
# message(STATUS "fmt include directory: ${FMT_INCLUDE_DIRS}")
# include_directories(${FMT_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} fmt::fmt)

# Fetch imgui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.3  # Stable release
)
FetchContent_MakeAvailable(imgui)
set(IMGUI_INCLUDE_DIRS ${imgui_SOURCE_DIR})
message(STATUS "imgui include directory: ${IMGUI_INCLUDE_DIRS}")

# Add ImGui sources manually
set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)


# Create the imgui library target
add_library(imgui STATIC ${IMGUI_SOURCES})


# Fetch stb
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)
set(STB_INCLUDE_DIRS ${stb_SOURCE_DIR})
message(STATUS "stb include directory: ${STB_INCLUDE_DIRS}")
include_directories(${STB_INCLUDE_DIRS})

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)
FetchContent_MakeAvailable(yaml-cpp)
set(YAML_INCLUDE_DIRS "${yaml-cpp_SOURCE_DIR}/include")
message(STATUS "yaml include directory: ${YAML_INCLUDE_DIRS}")
include_directories(${YAML_INCLUDE_DIRS})

# # Fetch vk_bootstrap
# FetchContent_Declare(
#         vk_bootstrap
#         GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
#         GIT_TAG v1.3.296
# )
# FetchContent_MakeAvailable(vk_bootstrap)
# target_link_libraries(${CMAKE_PROJECT_NAME} vk-bootstrap::vk-bootstrap)
#
# # Fetch VulkanMemoryAllocator
# FetchContent_Declare(
#         vma
#         GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
#         GIT_TAG v3.1.0  # Stable release
# )
# FetchContent_MakeAvailable(vma)
# set(VMA_INCLUDE_DIRS ${vma_SOURCE_DIR}/include)
# message(STATUS "vma include directory: ${VMA_INCLUDE_DIRS}")
# include_directories(${VMA_INCLUDE_DIRS})
#
# # Fetch volk
# FetchContent_Declare(
#         volk
#         GIT_REPOSITORY https://github.com/zeux/volk.git
#         GIT_TAG vulkan-sdk-1.3.296.0  # Stable release
# )
# FetchContent_MakeAvailable(volk)
# set(VOLK_INCLUDE_DIRS ${volk_SOURCE_DIR})
# message(STATUS "volk include directory: ${VOLK_INCLUDE_DIRS}")
# include_directories(${VOLK_INCLUDE_DIRS})

# Use the Vulkan package provided by the SDK
# find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

# If Vulkan is found, link it to your executable
# if (Vulkan_FOUND)
#     target_link_libraries(${CMAKE_PROJECT_NAME} Vulkan::Vulkan)
# endif()

# Include directories for ImGui
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

IF(WIN32)
    target_link_libraries(imgui PRIVATE glfw opengl32)
ELSE()
    target_link_libraries(imgui PRIVATE glfw OpenGL)
ENDIF()

# Link imgui to your main project target
target_link_libraries(${CMAKE_PROJECT_NAME} imgui)

target_link_libraries(${CMAKE_PROJECT_NAME} yaml-cpp::yaml-cpp)

# Link libraries for GLFW and Vulkan
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)

# Use C++17
SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
