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

#include "wasp_dsr_planner/executor/condition/check_keyword.hpp"
#include "../../include/wasp_dsr_planner/json_messages.hpp"

CheckKeyword::CheckKeyword(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf)
{
}

void CheckKeyword::getInputsOnTick()
{
  getInput<std::string>("keyword", keyword_);
  getInput<std::string>("word_to_check", word_to_check_);
}

BT::NodeStatus CheckKeyword::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;

  std::string norm1, norm2;

  for (char ch : keyword_) {
    if (std::isalpha(static_cast<unsigned char>(ch)) ||
      std::isspace(static_cast<unsigned char>(ch)))
    {
      norm1 += std::tolower(static_cast<unsigned char>(ch));
    }
  }

  for (char ch : word_to_check_) {
    if (std::isalpha(static_cast<unsigned char>(ch)) ||
      std::isspace(static_cast<unsigned char>(ch)))
    {
      norm2 += std::tolower(static_cast<unsigned char>(ch));
    }
  }

  if(norm1 == norm2) {
    success = BT::NodeStatus::SUCCESS;
  }

  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<CheckKeyword>(name, "check_keyword", config);
    };

  factory.registerBuilder<CheckKeyword>("CheckKeyword", builder);
}
