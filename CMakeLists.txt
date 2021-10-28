cmake_minimum_required(VERSION 3.8)
project(moveit_differential_ik_plugin)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# The base class for this plugin is in the ros2_controllers admittance_controller package
find_package(admittance_controller REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(moveit_ros_move_group REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(trajectory_msgs REQUIRED)

set(THIS_PACKAGE_INCLUDE_DEPENDS
  ${Eigen_LIBRARIES}
  admittance_controller
  geometry_msgs
  moveit_ros_move_group
  moveit_ros_planning_interface
  pluginlib
  trajectory_msgs
)

# Plugin implementation
add_library(moveit_differential_ik_plugin SHARED
  src/moveit_kinematics.cpp
)
target_include_directories(moveit_differential_ik_plugin PRIVATE
  include
)
ament_target_dependencies(moveit_differential_ik_plugin
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)
# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(moveit_differential_ik_plugin PRIVATE "MOVEIT_DIFFERENTIAL_IK_LIB_BUILDING_DLL")

# args are package name, file name
pluginlib_export_plugin_description_file(admittance_controller moveit_differential_ik_plugin.xml)

install(
  TARGETS moveit_differential_ik_plugin
  EXPORT export_${PROJECT_NAME}
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(
  DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(
  include
)
ament_export_libraries(
  moveit_differential_ik_plugin
)
ament_export_dependencies(
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

ament_package()
