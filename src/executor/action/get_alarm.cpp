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

#include "wasp_dsr_planner/executor/action/get_alarm.hpp"

GetAlarm::GetAlarm(
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

BT::NodeStatus GetAlarm::tick()
{
  // Set to running
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  while (result == BT::NodeStatus::RUNNING) {
    setStatusRunningAndYield();
    result = checkResult();
  }
  return result;
}

BT::NodeStatus GetAlarm::checkResult()
{
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  // Check if there is menu_selection node in the graph
  auto wants_to_edge = G_->get_edges_by_type("wants_to");
  for (const auto & edge: wants_to_edge) {
    auto from_node = G_->get_node(edge.from());
    auto to_node = G_->get_node(edge.to());
    if (from_node.has_value() && from_node.value().name() == executor_name_) {
      if (to_node.has_value() && to_node.value().name() == "alarm") {
        std::cout << "Edge wants_to between robot and alarm found" << std::endl;
        // Get goal and alarm attribute
        auto goal_x = G_->get_attrib_by_name<goal_x_att>(to_node.value());
        auto goal_y = G_->get_attrib_by_name<goal_y_att>(to_node.value());
        auto goal_angle = G_->get_attrib_by_name<goal_angle_att>(to_node.value());
        auto alarm = G_->get_attrib_by_name<alarm_att>(to_node.value());
        if (goal_x.has_value() && goal_y.has_value() && goal_angle.has_value()) {
          Goal goal;
          goal.x = goal_x.value();
          goal.y = goal_y.value();
          goal.yaw = goal_angle.value();
          setOutput<Goal>("goal", goal);
        }
        if (alarm.has_value()) {
          setOutput<std::string>("alarm", alarm.value());
        }
        // And replace edge
        DSR::replace_edge<is_performing_edge_type>(
          G_, from_node.value().id(), to_node.value().id(), "wants_to", executor_name_);
        result = BT::NodeStatus::SUCCESS;
      }
    }
  }
  return result;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<GetAlarm>(name, "get_alarm", config);
    };

  factory.registerBuilder<GetAlarm>("GetAlarm", builder);
}
