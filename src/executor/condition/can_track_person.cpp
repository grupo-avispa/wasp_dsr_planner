// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Alberto J. Tudela Roldán
// Copyright (c) 2024 José Galeas Merchán
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
#include "wasp_dsr_planner/executor/condition/can_track_person.hpp"
#include "wasp_dsr_planner/json_messages.hpp"

CanTrackPerson::CanTrackPerson(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf), current_person_node_id_(0)
{
  config().blackboard->set<uint64_t>("current_person_node_id", current_person_node_id_);
}

void CanTrackPerson::getInputsOnTick()
{
  getInput<uint64_t>("node_id", person_id_);
}

BT::NodeStatus CanTrackPerson::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;
  auto robot_node = G_->get_node(robot_name_);
  auto tracking_node = G_->get_node("tracking");
  auto person_node = G_->get_node(person_id_);
  if (person_node.has_value() && person_node.value().type() == "person") {
    auto tracking_enable = G_->get_attrib_by_name<tracking_enable_att>(person_node.value());
    if (tracking_enable.has_value() && tracking_enable.value()) {
      std::cout << "The person is enable to be tracked" << std::endl;
      if (robot_node.has_value() && tracking_node.has_value()) {
        DSR::replace_edge<is_performing_edge_type>(
          G_, robot_node.value().id(), tracking_node.value().id(), "wants_to", robot_name_);
      }
      success = BT::NodeStatus::SUCCESS;
    } else {
      if (robot_node.has_value() && tracking_node.has_value()) {
        DSR::replace_edge<finished_edge_type>(
          G_, robot_node.value().id(), tracking_node.value().id(), "wants_to", robot_name_);
      }
      std::cout << "The person is NOT enable to be tracked" << std::endl;
    }
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<CanTrackPerson>(name, "can_track_person", config);
    };

  factory.registerBuilder<CanTrackPerson>("CanTrackPerson", builder);
}
