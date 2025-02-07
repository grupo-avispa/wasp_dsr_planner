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

#include "plannerAgent/executor/action/interaction.hpp"

Interaction::Interaction(
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

BT::NodeStatus Interaction::tick()
{
  std::cout << "[" << action_name_ << "]: Ticking ..." << std::endl;

  // Set to running
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  while (result == BT::NodeStatus::RUNNING) {
    setStatusRunningAndYield();
    result = checkResult();
  }
  return result;
}

BT::NodeStatus Interaction::checkResult()
{
  auto success = BT::NodeStatus::FAILURE;
  getInput<uint64_t>("node_id", person_id_);
  // Replace the 'is_with' edge with a 'interacting' edge between robot and action
  auto robot_node = G_->get_node(robot_name_);
  auto person_node = G_->get_node(person_id_);
  if (robot_node.has_value() && person_node.has_value()) {
    if (G_->delete_edge(person_id_, robot_node.value().id(), "is_with")) {
      auto edge = DSR::create_edge_with_priority<interacting_edge_type>(
        G_, robot_node.value().id(), person_id_, 0, robot_name_);
      if (G_->insert_or_assign_edge(edge)) {
        std::cout << "[" << action_name_ << "]: Interacting edge " << std::endl;
        success = BT::NodeStatus::SUCCESS;
      }
    }
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<Interaction>(name, "interaction", config);
    };

  factory.registerBuilder<Interaction>("Interaction", builder);
}
