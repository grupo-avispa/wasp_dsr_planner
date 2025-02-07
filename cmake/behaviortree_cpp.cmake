
find_package(ament_cmake QUIET)
if(ament_cmake_FOUND)
    message(STATUS "Found behaviortree_cpp_v3: 3.8 (${behaviortree_cpp_v3_DIR})")
    find_package(behaviortree_cpp_v3 REQUIRED)
    set(ROS_DISTRO $ENV{ROS_DISTRO})
    include_directories(/opt/ros/${ROS_DISTRO}/include)
    link_directories(/opt/ros/${ROS_DISTRO}/lib)
else()
    if(EXISTS "/usr/local/include/behaviortree_cpp_v3")
        message(STATUS "Found behaviortree_cpp_v3: 3.8 (/usr/local/include/behaviortree_cpp_v3)")
        include_directories(/usr/local/include/behaviortree_cpp_v3)
        set(behaviortree_cpp_v3_FOUND TRUE)
    endif()
    if(NOT behaviortree_cpp_v3_FOUND)
        message(STATUS "Downloading BehaviorTree.CPP")
        include(FetchContent)

        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
        set(BUILD_TOOLS OFF CACHE BOOL "Build commandline tools" FORCE)
        set(BUILD_EXAMPLES    OFF CACHE BOOL "Build tutorials and examples" FORCE)
        set(BUILD_UNIT_TESTS  OFF CACHE BOOL "Build the unit tests" FORCE)

        set(FETCHCONTENT_QUIET ON)
        FetchContent_Declare(behaviortree_cpp
            GIT_REPOSITORY https://github.com/BehaviorTree/BehaviorTree.CPP.git
            GIT_TAG         v3.8
            GIT_PROGRESS    TRUE
            GIT_SHALLOW     TRUE
            SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/behaviortree_cpp-src"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/behaviortree_cpp-build"
        )
        #FetchContent_MakeAvailable(behaviortree_cpp)
        FetchContent_Populate(behaviortree_cpp)
        add_subdirectory(${behaviortree_cpp_SOURCE_DIR} ${behaviortree_cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
        include_directories(${CMAKE_CURRENT_BINARY_DIR}/behaviortree_cpp-src/include)
    endif()
endif()