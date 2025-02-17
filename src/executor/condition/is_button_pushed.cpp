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
#include "wasp_dsr_planner/executor/condition/is_button_pushed.hpp"
#include "wasp_dsr_planner/json_messages.hpp"

IsButtonPushed::IsButtonPushed(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf), current_person_node_id_(0)
{
  config().blackboard->set<uint64_t>("current_person_node_id", current_person_node_id_);
}

void IsButtonPushed::getInputsOnTick()
{
}

BT::NodeStatus IsButtonPushed::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;
  auto finished_edges = G_->get_edges_by_type("finished");
  for (const auto & edge: finished_edges) {
    auto from_node = G_->get_node(edge.from());
    auto to_node = G_->get_node(edge.to());
    if (from_node.has_value() && from_node.value().name() == executor_name_) {
      if (to_node.has_value() && to_node.value().name() == "tracking") {
        std::cout << "Edge finished between robot and tracking found" << std::endl;
        // And delete the node
        if (!G_->delete_node("tracking")) {
          std::cout << "The node [";
          std::cout << to_node.value().name();
          std::cout << "] couldn't be deleted" << std::endl;
        }
        success = BT::NodeStatus::SUCCESS;
      }
    }
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<IsButtonPushed>(name, "is_button_pushed", config);
    };

  factory.registerBuilder<IsButtonPushed>("IsButtonPushed", builder);
}
