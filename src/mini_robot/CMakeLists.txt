cmake_minimum_required(VERSION 3.8)
project(mini_robot)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(robot_node src/robot_node.cpp)
add_executable(control_node src/control_node.cpp)
add_executable(sensor_node src/sensor_node.cpp)
add_executable(localization_node src/localization_node.cpp)
add_executable(
  planning_node
  src/planning_node.cpp
)


ament_target_dependencies(
  control_node
  rclcpp
  geometry_msgs
)

ament_target_dependencies(
  robot_node
  rclcpp
  geometry_msgs
  std_msgs
)

ament_target_dependencies(
  sensor_node
  rclcpp
  std_msgs
)

ament_target_dependencies(
  localization_node
  rclcpp
  std_msgs
)

ament_target_dependencies(
  planning_node
  rclcpp
  std_msgs
  geometry_msgs
  example_interfaces
)

install(
  TARGETS
  robot_node
  control_node
  sensor_node
  localization_node
  planning_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
