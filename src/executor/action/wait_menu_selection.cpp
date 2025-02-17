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

#include "wasp_dsr_planner/executor/bt_utils.hpp"
#include "wasp_dsr_planner/executor/action/wait_menu_selection.hpp"

WaitMenuSelection::WaitMenuSelection(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: CoroActionNode(xml_tag_name, conf), action_name_(action_name)
{
  // Get the DSR graph from the blackboard (thread-safe)
  auto g_lock = config().blackboard->getAnyLocked("dsr_graph");
    G_ = g_lock.get()->cast<std::shared_ptr<DSR::DSRGraph>>();
  // Get the executor node name from input or blackboard
  getInputOrBlackboard("executor_name", executor_name_);
  // Get the source
  source_ = config().blackboard->get<std::string>("source");
}

BT::NodeStatus WaitMenuSelection::tick()
{
  // Set to running
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  while (result == BT::NodeStatus::RUNNING) {
    setStatusRunningAndYield();
    result = checkResult();
  }
  return result;
}

BT::NodeStatus WaitMenuSelection::checkResult()
{
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  // Check if there is menu_selection node in the graph
  if (auto menu_selection_node = G_->get_node("menu_selection"); menu_selection_node.has_value()) {
    // Delete menu_selection node
    if (!G_->delete_node("menu_selection")) {
      result = BT::NodeStatus::FAILURE;
      std::cout << "The node " << menu_selection_node.value().name();
      std::cout << " couldn't be deleted" << std::endl;
    }
    // Check if there is say node in the graph and abort it
    auto say_node = G_->get_node("say");
    auto robot_node = G_->get_node(executor_name_);
    if (robot_node.has_value() && say_node.has_value()) {
      if (G_->delete_edge(robot_node.value().id(), say_node.value().id(), "is_performing")) {
        auto edge = DSR::create_edge_with_priority<abort_edge_type>(
          G_, robot_node.value().id(), say_node.value().id(), 0, source_);
        if (G_->insert_or_assign_edge(edge)) {
          std::cout << "Aborted action " << say_node.value().name() << std::endl;
        }
      } else {
        result = BT::NodeStatus::FAILURE;
        std::cout << "ERROR: Trying to delete edge type is_performing" << std::endl;
      }
    }
    result = BT::NodeStatus::SUCCESS;
  }
  return result;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<WaitMenuSelection>(name, "wait_menu_selection", config);
    };

  factory.registerBuilder<WaitMenuSelection>("WaitMenuSelection", builder);
}
