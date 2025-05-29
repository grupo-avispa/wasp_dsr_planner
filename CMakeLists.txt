cmake_minimum_required(VERSION 3.5)
project(wasp_dsr_planner)

# Default to C++20
if(NOT CMAKE_CXX_STANDARD)
  if("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(CMAKE_CXX_STANDARD 20)
  else()
    message(FATAL_ERROR "cxx_std_20 could not be found.")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Wdeprecated -fPIC -Wshadow -Wnull-dereference)
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>")
endif()

# Set variables to install
SET(COMPONENT_INSTALL_PATH "/opt/wasp" CACHE STRING "component install path")

# Find dependencies
find_package(Eigen3 3.3 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)
find_package(fastrtps REQUIRED)

include(cmake/behaviortree_cpp.cmake)

# Set include directories
get_target_property(QT_INCLUDE_DIRS Qt5::Core INTERFACE_INCLUDE_DIRECTORIES)
include_directories(
  include
  ${QT_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${fastrtps_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/gen # this find plugin_list.hpp
)

# Set the names
set(library_name ${PROJECT_NAME}_core)
set(executable_name ${PROJECT_NAME})

# Set QT libraries and DSR libraries
set(QT_LIBRARIES Qt5::Widgets Qt5::OpenGL Qt5::Core)
set(DSR_LIBRARIES dsr_api dsr_core dsr_gui fastcdr fastrtps)

set(dependencies
  ${QT_LIBRARIES}
  ${DSR_LIBRARIES}
  Eigen3::Eigen
  behaviortree_cpp
)

# Add library
add_library(${library_name} SHARED src/behavior_tree_engine.cpp)
target_link_libraries(${library_name} ${dependencies})

# Add executble
add_executable(${executable_name} src/main.cpp)
target_link_libraries(${executable_name} ${library_name})

# Add the plugins
# Actions
set(action_plugins
  cancel_action
  dock
  get_intent
  get_role
  get_task
  generate_response
  get_random_goal
  help
  insert_edge
  insert_node
  interaction
  listen
  move
  play
  say
  set_use_case
  set_volume
  sign
  show
  think
  use_case_selector
  wait_menu_selection
)

foreach(plugin ${action_plugins})
  add_library(${plugin}_action_bt_node SHARED src/executor/action/${plugin}.cpp)
  list(APPEND plugin_libs ${plugin}_action_bt_node)
endforeach()

# Conditions
set(condition_plugins
  can_track_person
  check_keyword
  check_medicine
  is_button_pushed
  is_navigating
  person_found
  reset_person_id
  safe_interaction
  use_case_finished
)

foreach(plugin ${condition_plugins})
  add_library(${plugin}_condition_bt_node SHARED src/executor/condition/${plugin}.cpp)
  list(APPEND plugin_libs ${plugin}_condition_bt_node)
endforeach()

# Control
set(control_plugins
  recovery_node
)

foreach(plugin ${control_plugins})
  add_library(${plugin}_control_bt_node SHARED src/executor/control/${plugin}.cpp)
  list(APPEND plugin_libs ${plugin}_control_bt_node)
endforeach()

# Decoratos
set(decorator_plugins
  distance_approach
  get_causes
  sentence_builder
)

foreach(plugin ${decorator_plugins})
  add_library(${plugin}_decorator_bt_node SHARED src/executor/decorator/${plugin}.cpp)
  list(APPEND plugin_libs ${plugin}_decorator_bt_node)
endforeach()

# Link all plugins
foreach(bt_plugin ${plugin_libs})
  target_compile_definitions(${bt_plugin} PRIVATE BT_PLUGIN_EXPORT)
  target_link_libraries(${bt_plugin} PRIVATE ${BTCPP_LIBRARY})
  target_link_libraries(${bt_plugin} ${QT_LIBRARIES} dsr_core dsr_api fastcdr fastrtps)
endforeach()

# Add ejecutable to generate the xml
add_executable(generate_planner_agent_tree_nodes_xml src/generate_planner_agent_tree_nodes_xml.cpp)
target_link_libraries(generate_planner_agent_tree_nodes_xml behaviortree_cpp)

# We will embed the list of plugin names inside a header file
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen/wasp_dsr_planner)
configure_file(plugins_list.hpp.in ${GENERATED_DIR}/plugins_list.hpp)

# Install in the install path
INSTALL(TARGETS ${library_name} ${plugin_libs}
  DESTINATION ${COMPONENT_INSTALL_PATH}/lib
)

install(TARGETS ${executable_name} generate_planner_agent_tree_nodes_xml
  DESTINATION ${COMPONENT_INSTALL_PATH}/bin
)

INSTALL(DIRECTORY params
  DESTINATION ${COMPONENT_INSTALL_PATH}/${PROJECT_NAME}
)

# Install the generated header file
install(FILES ${GENERATED_DIR}/plugins_list.hpp
  DESTINATION include/wasp_dsr_planner
)
