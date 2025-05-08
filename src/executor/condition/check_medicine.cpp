// Copyright (c) 2025 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2025 Jose M. Galeas Merchan
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

#include "wasp_dsr_planner/executor/condition/check_medicine.hpp"
#include "../../include/wasp_dsr_planner/json_messages.hpp"

CheckMedicine::CheckMedicine(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf)
{
}

void CheckMedicine::getInputsOnTick()
{

}

BT::NodeStatus CheckMedicine::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;
  // Check edge robot -> has -> medicine to return success
  auto has_edges = G_->get_edges_by_type("has");
  for (const auto & edge: has_edges) {
    auto from_node = G_->get_node(edge.from());
    auto to_node = G_->get_node(edge.to());
    if (from_node.has_value() && from_node.value().name() == executor_name_) {
      if (to_node.has_value() && to_node.value().name() == "medicine") {
        std::cout << "Edge has between robot and medicine found" << std::endl;
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
      return std::make_unique<CheckMedicine>(name, "check_medicine", config);
    };

  factory.registerBuilder<CheckMedicine>("CheckMedicine", builder);
}
