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

#include "plannerAgent/executor/condition/use_case_finished.hpp"

UseCaseFinished::UseCaseFinished(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf)
{
}

void UseCaseFinished::getInputsOnTick()
{
  getInput<std::string>("result_code", result_code_);
}

BT::NodeStatus UseCaseFinished::checkCondition()
{
  auto success = BT::NodeStatus::SUCCESS;
  // Replace the 'is_performing' edge with a 'finished' edge between robot and use_case
  if (auto robot_node = G_->get_node(robot_name_); robot_node.has_value()) {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      // Delete 'is_performing' edge between robot and use_case
      if (G_->delete_edge(robot_node.value().id(), use_case_node.value().id(), "is_performing")) {
        // Add 'finished' edge between robot and use_case
        auto new_edge = DSR::create_edge_with_priority<finished_edge_type>(
          G_, robot_node.value().id(), use_case_node.value().id(), 0, robot_name_);
        // Add result_code attribute
        if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
          G_->add_or_modify_attrib_local<result_code_att>(use_case_node.value(), result_code_);
          G_->update_node(use_case_node.value());
        }
        // Insert the new edge
        if (!G_->insert_or_assign_edge(new_edge)) {
          std::cout << "[" << condition_name_ << "]: ";
          std::cout << "Error inserting 'finished' edge between robot and use_case" << std::endl;
          success = BT::NodeStatus::FAILURE;
        }
      }
    }
  }
  return success;
}

#include "behaviortree_cpp_v3/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<UseCaseFinished>(name, "use_case_finished", config);
    };

  factory.registerBuilder<UseCaseFinished>("UseCaseFinished", builder);
}
