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

#include "wasp_dsr_planner/executor/condition/reset_person_id.hpp"

ResetPersonId::ResetPersonId(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf)
{
}

void ResetPersonId::getInputsOnTick()
{
  getInput<std::string>("result_code", result_code_);
}

BT::NodeStatus ResetPersonId::checkCondition()
{
  auto success = BT::NodeStatus::SUCCESS;
  // Get the current person node id
  uint64_t person_node_id = config().blackboard->get<uint64_t>("current_person_node_id");
  // Replace the 'interacting' edge with a 'is_with' edge between person and robot
  if (auto robot_node = G_->get_node(executor_name_); robot_node.has_value()) {
    if (auto person_node = G_->get_node(person_node_id);
      person_node.has_value() && person_node_id != 0)
    {
      if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
        auto use_case_id = G_->get_attrib_by_name<use_case_id_att>(use_case_node.value());
        if (use_case_id.value() == "neuron") {
          // TODO: This is a temporary solution and must be performed by the webserver
          G_->add_or_modify_attrib_local<neuron_att>(person_node.value(), false);
          G_->update_node(person_node.value());
        }
        if (use_case_id.value() == "reminder") {
          G_->add_or_modify_attrib_local<reminder_att>(person_node.value(), false);
          G_->update_node(person_node.value());
        }
      }
      // Delete 'interacting' edge between robot and person
      if (G_->delete_edge(robot_node.value().id(), person_node.value().id(), "interacting")) {
        // Add 'is_with' edge between person and robot
        auto new_edge = DSR::create_edge_with_priority<is_with_edge_type>(
          G_, person_node.value().id(), robot_node.value().id(), 0, executor_name_);
        if (!G_->insert_or_assign_edge(new_edge)) {
          std::cout << "[" << condition_name_ << "]: ";
          std::cout << "Error inserting 'is_with' edge between robot and person" << std::endl;
          success = BT::NodeStatus::FAILURE;
        }
      }
    }
  }
  // Reset the current person node id
  config().blackboard->set<uint64_t>("current_person_node_id", 0);
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<ResetPersonId>(name, "reset_person_id", config);
    };

  factory.registerBuilder<ResetPersonId>("ResetPersonId", builder);
}
