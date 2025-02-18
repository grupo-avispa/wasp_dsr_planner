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
#include "wasp_dsr_planner/executor/action/cancel_action.hpp"

CancelAction::CancelAction(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: CoroActionNode(xml_tag_name, conf), action_name_(action_name)
{
  // Get the DSR graph from the blackboard (thread-safe)
  auto g_lock = config().blackboard->getAnyLocked("@dsr_graph");
  G_ = g_lock.get()->cast<std::shared_ptr<DSR::DSRGraph>>();
  // Get the source
  source_ = config().blackboard->get<std::string>("@source");
  // Get the executor node name from input or blackboard
  getInputOrBlackboard("executor_name", executor_name_);
}

BT::NodeStatus CancelAction::tick()
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

BT::NodeStatus CancelAction::checkResult()
{
  auto success = BT::NodeStatus::FAILURE;
  getInput<std::string>("action_name", action_to_cancel_);

  auto wants_to_edge = G_->get_edge(executor_name_, action_to_cancel_, "wants_to");
  auto is_performing_edge = G_->get_edge(executor_name_, action_to_cancel_, "is_performing");
  if (wants_to_edge.has_value() || is_performing_edge.has_value()) {
    // Replace the 'wants_to' edge with a 'cancel' edge between robot and action
    if (DSR::replace_edge<cancel_edge_type>(
        G_, executor_name_, action_to_cancel_, "wants_to", source_))
    {
      // Add result_code attribute
      if (auto action_node = G_->get_node(action_to_cancel_); action_node.has_value()) {
        std::string arg = "CANCELED: by CancelAction node";
        G_->add_or_modify_attrib_local<result_code_att>(action_node.value(), arg);
        G_->update_node(action_node.value());
        success = BT::NodeStatus::SUCCESS;
      }
      // Replace the 'is_performing' edge with a 'abort' edge between robot and action
    } else if (DSR::replace_edge<abort_edge_type>(
        G_, executor_name_, action_to_cancel_, "is_performing", source_))
    {
      // Add result_code attribute
      if (auto action_node = G_->get_node(action_to_cancel_); action_node.has_value()) {
        std::string arg = "ABORTED: by CancelAction node";
        G_->add_or_modify_attrib_local<result_code_att>(action_node.value(), arg);
        G_->update_node(action_node.value());
        success = BT::NodeStatus::SUCCESS;
      }
    }
    // If the action is not in the graph, return SUCCESS
  } else {
    return BT::NodeStatus::SUCCESS;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<CancelAction>(name, "cancel_action", config);
    };

  factory.registerBuilder<CancelAction>("CancelAction", builder);
}
