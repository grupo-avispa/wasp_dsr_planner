// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Alberto J. Tudela Roldán
// Copyright (c) 2024 Óscar Pons Fernandez
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "wasp_dsr_planner/executor/decorator/distance_approach.hpp"

DistanceApproach::DistanceApproach(
  const std::string & xml_tag_name,
  const std::string & decorator_name, const BT::NodeConfiguration & conf)
: BT::DecoratorNode(xml_tag_name, conf), decorator_name_(decorator_name)
{

  // Get the DSR graph from the blackboard
  G_ = config().blackboard->get<std::shared_ptr<DSR::DSRGraph>>("dsr_graph");
  // Get the robot node name from the blackboard
  robot_name_ = config().blackboard->get<std::string>("robot_name");

  std::cout << "[" << xml_tag_name << ", " << decorator_name_ << "]: ";
  std::cout << "Created DSR-BT node for the robot node '" << robot_name_ << "'" << std::endl;
}

BT::NodeStatus DistanceApproach::tick()
{
  auto success = BT::NodeStatus::FAILURE;
  std::cout << "[" << decorator_name_ << "]: Ticking ..." << std::endl;
  // Get the input ports
  float distance = 0.0;
  getInput<float>("distance", distance);
  Goal input_goal;
  getInput<Goal>("input_goal", input_goal);

  // Get the position of the robot
  if (auto robot_node = G_->get_node(robot_name_); robot_node.has_value()) {
    auto pose_x = G_->get_attrib_by_name<pose_x_att>(robot_node.value());
    auto pose_y = G_->get_attrib_by_name<pose_y_att>(robot_node.value());
    auto pose_yaw = G_->get_attrib_by_name<pose_angle_att>(robot_node.value());
    if (pose_x.has_value() && pose_y.has_value() && pose_yaw.has_value()) {
      Goal robot_pose(pose_x.value(), pose_y.value(), 0.0);
      float dx = input_goal.x - robot_pose.x;
      float dy = input_goal.y - robot_pose.y;
      float rps = std::sqrt(dx * dx + dy * dy);
      float angle_to_person = std::atan2(dy, dx);
      // Modify the goal
      robot_pose.x += (rps - distance) * cos(angle_to_person);
      robot_pose.y += (rps - distance) * sin(angle_to_person);
      robot_pose.yaw = angle_to_person;
      // Set the output port
      setOutput("output_goal", robot_pose);
      std::cout << "[" << decorator_name_ << "]: Approach at ";
      std::cout << "(" << robot_pose.x << ", " << robot_pose.y << ")" << std::endl;
      success = child_node_->executeTick();
    }
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<DistanceApproach>(name, "distance_approach", config);
    };

  factory.registerBuilder<DistanceApproach>("DistanceApproach", builder);
}
