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

#include "wasp_dsr_planner/executor/action/help.hpp"

Help::Help(
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

BT::NodeStatus Help::tick()
{
  std::cout << "Starting Help Use Case\n";
  // Set to running
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  while (result == BT::NodeStatus::RUNNING) {
    setStatusRunningAndYield();
    result = checkResult();
  }
  return result;
}

BT::NodeStatus Help::checkResult()
{
  auto alarm_node = G_->get_node("person_alarm");
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  if (alarm_node.has_value()) {
    auto goal_x = G_->get_attrib_by_name<goal_x_att>(alarm_node.value());
    auto goal_y = G_->get_attrib_by_name<goal_y_att>(alarm_node.value());
    auto goal_angle = G_->get_attrib_by_name<goal_angle_att>(alarm_node.value());
    if (!std::isnan(goal_x.value()) && !std::isnan(goal_y.value()) && !std::isnan(
        goal_angle.value()))
    {
      Goal goal;
      goal.x = goal_x.value();
      goal.y = goal_y.value();
      goal.yaw = goal_angle.value();
      setOutput<Goal>("goal", goal);
      result = BT::NodeStatus::SUCCESS;
    }
  }
  return result;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<Help>(name, "help", config);
    };

  factory.registerBuilder<Help>("Help", builder);
}
