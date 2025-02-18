// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Jose Galeas Merchan
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

#include "wasp_dsr_planner/executor/bt_utils.hpp"
#include "wasp_dsr_planner/executor/action/insert_edge.hpp"

InsertEdge::InsertEdge(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: CoroActionNode(xml_tag_name, conf), action_name_(action_name)
{
  // Get the DSR graph from the blackboard (thread-safe)
  auto g_lock = config().blackboard->getAnyLocked("dsr_graph");
  G_ = g_lock.get()->cast<std::shared_ptr<DSR::DSRGraph>>();
  // Get the executor node name from input or blackboard
  getInputOrBlackboard("executor_name", executor_name_);
}

BT::NodeStatus InsertEdge::tick()
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

BT::NodeStatus InsertEdge::checkResult()
{
  auto success = BT::NodeStatus::FAILURE;
  getInput<std::string>("parent", parent_);
  getInput<std::string>("child", child_);
  getInput<std::string>("type", edge_type_);

  // Insert edge
  DSR::Edge new_edge;
  auto parent_node = G_->get_node(parent_);
  auto child_node = G_->get_node(child_);
  if (parent_node.has_value() && child_node.has_value()) {
    new_edge.from(parent_node.value().id());
    new_edge.to(child_node.value().id());
    new_edge.type(edge_type_);
    if (G_->insert_or_assign_edge(new_edge)) {
      success = BT::NodeStatus::SUCCESS;
    }
  }
  return success;
}


#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<InsertEdge>(name, "insert_edge", config);
    };

  factory.registerBuilder<InsertEdge>("InsertEdge", builder);
}
