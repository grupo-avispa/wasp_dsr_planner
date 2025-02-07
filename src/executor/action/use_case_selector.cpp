// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Alberto J. Tudela Roldán
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

#include "wasp_dsr_planner/executor/action/use_case_selector.hpp"

UseCaseSelector::UseCaseSelector(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: CoroActionNode(xml_tag_name, conf), action_name_(action_name)
{
  // Get the DSR graph from the blackboard
  G_ = config().blackboard->get<std::shared_ptr<DSR::DSRGraph>>("dsr_graph");
  // Get the robot node name from the blackboard
  robot_name_ = config().blackboard->get<std::string>("robot_name");

  std::cout << "[" << xml_tag_name << ", " << action_name_ << "]: ";
  std::cout << "Created DSR-BT node for the robot node '" << robot_name_ << "'" << std::endl;
}

BT::NodeStatus UseCaseSelector::tick()
{
  std::cout << "[" << action_name_ << "]: Ticking ..." << std::endl;

  // Set to running
  BT::NodeStatus result = BT::NodeStatus::IDLE;
  while (!BT::isStatusActive(status())) {
    setStatusRunningAndYield();
    result = checkResult();
  }
  std::cout << "[" << action_name_ << "]: Result: " << result << std::endl;
  return result;
}

BT::NodeStatus UseCaseSelector::checkResult()
{
  auto robot_node = G_->get_node(robot_name_);
  auto use_case_node = G_->get_node("use_case");
  if (robot_node.has_value() && use_case_node.has_value()) {
    // Check if the robot wants_to start a new use case: robot ---(wants_to)---> use_case
    auto wants_to_edge = G_->get_edge(
      robot_node.value().id(), use_case_node.value().id(), "wants_to");
    if (wants_to_edge.has_value()) {
      // Replace the 'wants_to' edge with a 'is_performing' edge between robot and use_case
      if (DSR::replace_edge<is_performing_edge_type>(
          G_, robot_node.value().id(), use_case_node.value().id(), "wants_to", robot_name_))
      {
        auto use_case_name = G_->get_attrib_by_name<use_case_id_att>(use_case_node.value());
        use_case_ = use_case_name.has_value() ? use_case_name.value() : "";
        std::cout << "[" << action_name_ << "]: Use case started: " << use_case_ << std::endl;
        setOutput("use_case", use_case_);
        return BT::NodeStatus::SUCCESS;
      } else {
        return BT::NodeStatus::FAILURE;
      }
    }

    // Check if the robot has finished a use case: robot ---(finished)---> use_case
    auto finished_edge = G_->get_edge(
      robot_node.value().id(), use_case_node.value().id(), "finished");
    if (finished_edge.has_value()) {
      use_case_ = "";
    }
  }

  return BT::NodeStatus::IDLE;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<UseCaseSelector>(name, "use_case_selector", config);
    };

  factory.registerBuilder<UseCaseSelector>("UseCaseSelector", builder);
}
