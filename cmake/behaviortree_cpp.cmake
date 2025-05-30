
find_package(ament_cmake QUIET)
if(ament_cmake_FOUND)
    message(STATUS "Found behaviortree_cpp: 4.6 (${behaviortree_cpp_DIR})")
    find_package(behaviortree_cpp REQUIRED)
    set(ROS_DISTRO $ENV{ROS_DISTRO})
    include_directories(/opt/ros/${ROS_DISTRO}/include)
    link_directories(/opt/ros/${ROS_DISTRO}/lib)
else()
    if(EXISTS "/usr/local/include/behaviortree_cpp")
        message(STATUS "Found behaviortree_cpp: 4.6 (/usr/local/include/behaviortree_cpp)")
        include_directories(/usr/local/include/behaviortree_cpp)
        set(behaviortree_cpp_FOUND TRUE)
    endif()
    if(NOT behaviortree_cpp_FOUND)
        message(STATUS "Downloading BehaviorTree.CPP")
        include(FetchContent)

        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
        set(BUILD_TOOLS OFF CACHE BOOL "Build commandline tools" FORCE)
        set(BUILD_EXAMPLES    OFF CACHE BOOL "Build tutorials and examples" FORCE)
        set(BUILD_UNIT_TESTS  OFF CACHE BOOL "Build the unit tests" FORCE)

        set(FETCHCONTENT_QUIET ON)
        FetchContent_Declare(behaviortree_cpp
            GIT_REPOSITORY https://github.com/BehaviorTree/BehaviorTree.CPP.git
            GIT_TAG         4.6.2
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