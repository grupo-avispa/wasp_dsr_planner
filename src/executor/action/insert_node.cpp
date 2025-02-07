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

#include "wasp_dsr_planner/executor/action/insert_node.hpp"

InsertNode::InsertNode(
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

BT::NodeStatus InsertNode::tick()
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

BT::NodeStatus InsertNode::checkResult()
{
  auto success = BT::NodeStatus::FAILURE;
  getInput<DSR::Node>("node", new_node_);
  getInput<std::map<std::string, DSR::Attribute>>("attributes", attributes_);

  if (auto id = G_->insert_node(new_node_); id.has_value()) {
    // Modify the attributes of the new node
    for (const auto & [att_name, att_value] : attributes_) {
      new_node_.attrs().insert_or_assign(att_name, att_value);
    }
    G_->update_node(new_node_);
    success = BT::NodeStatus::SUCCESS;
  }
  return success;
}


#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<InsertNode>(name, "insert_node", config);
    };

  factory.registerBuilder<InsertNode>("InsertNode", builder);
}
