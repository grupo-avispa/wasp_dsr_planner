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

#include "nlohmann/json.hpp"

#include "wasp_dsr_planner/executor/condition/person_found.hpp"
#include "wasp_dsr_planner/json_messages.hpp"

PersonFound::PersonFound(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf), current_person_node_id_(0)
{
  config().blackboard->set<uint64_t>("current_person_node_id", current_person_node_id_);
}

void PersonFound::getInputsOnTick()
{
  getInput<std::string>("filter_case", filter_case_);
}

BT::NodeStatus PersonFound::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;
  // Get the current person node id stored in the blackboard
  current_person_node_id_ = config().blackboard->get<uint64_t>("current_person_node_id");
  // Get all the edges of type 'is_with'
  auto is_with_edges = G_->get_edges_by_type("is_with");
  for (const auto & edge: is_with_edges) {
    auto from_node = G_->get_node(edge.from());
    auto to_node = G_->get_node(edge.to());
    if (from_node.has_value() && from_node.value().type() == "person") {
      if (to_node.has_value() && to_node.value().name() == executor_name_) {
        // Filter the person node depending on the use case
        // We want person with empty menu
        if (filter_case_ == "menu") {
          auto menu = G_->get_attrib_by_name<menu1_att>(from_node.value());
          if (!menu.value().empty()) {
            continue;
          }
          // We want person with neuron = true
        } else if (filter_case_ == "neuron") {
          auto neuron = G_->get_attrib_by_name<neuron_att>(from_node.value());
          if (!neuron.value()) {
            continue;
          }
        }
        // Get the identifier of the person
        auto person_id = G_->get_attrib_by_name<identifier_att>(from_node.value());
        if (person_id.has_value()) {
          // Check if the person node id is initialized
          if (current_person_node_id_ == 0) {
            // Set the current person node id
            current_person_node_id_ = from_node.value().id();
            // Save the current person node id in the blackboard
            config().blackboard->set<uint64_t>("current_person_node_id", current_person_node_id_);
          }
          // Check if the person is the same as the last one found
          if (from_node.value().id() == current_person_node_id_) {
            std::cout << "[" << condition_name_ << "]: ";
            std::cout << "Person found with id: " << person_id.value() << std::endl;
            // Set the output port with the node id of the person
            setOutput("node_id", from_node.value().id());
            // Set the output port with the pose of the person
            auto pose_x = G_->get_attrib_by_name<pose_x_att>(from_node.value());
            auto pose_y = G_->get_attrib_by_name<pose_y_att>(from_node.value());
            auto pose_yaw = G_->get_attrib_by_name<pose_angle_att>(from_node.value());
            if (pose_x.has_value() && pose_y.has_value() && pose_yaw.has_value()) {
              setOutput("pose", Goal(pose_x.value(), pose_y.value(), pose_yaw.value()));
            }
            // Set the output port with the safe distance to approach the person
            // auto safe_distance = G_->get_attrib_by_name<safe_distance_att>(from_node.value());
            // if (safe_distance.has_value()) {
            //   setOutput("safe_distance", safe_distance.value());
            // }
            // Set the output port with the volume to say the text
            // auto comm_parameters = G_->get_attrib_by_name<comm_parameters_att>(from_node.value());
            // if (comm_parameters.has_value()) {
            //   CommParameters parameters(nlohmann::json::parse(comm_parameters.value()));
            //   setOutput("volume", parameters.volume);
            // }
            setOutput("safe_distance", 0.75);
            setOutput("volume", 50.0);
            success = BT::NodeStatus::SUCCESS;
            break;
          }
        }
      }
    }
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<PersonFound>(name, "person_found", config);
    };

  factory.registerBuilder<PersonFound>("PersonFound", builder);
}
