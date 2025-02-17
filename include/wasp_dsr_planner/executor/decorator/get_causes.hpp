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

#ifndef PLANNER_AGENT__EXECUTOR__CONDITION__GET_CAUSES_HPP_
#define PLANNER_AGENT__EXECUTOR__CONDITION__GET_CAUSES_HPP_

#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "behaviortree_cpp/decorator_node.h"
#include "dsr/api/dsr_api.h"

/**
 * @class GetCauses
 * @brief This decorator will build a sentence based on the current use case and the person.
 */
class GetCauses : public BT::DecoratorNode
{
public:
  GetCauses(
    const std::string & xml_tag_name, const std::string & decorator_name,
    const BT::NodeConfiguration & conf);

  GetCauses() = delete;

  ~GetCauses()
  {
    G_.reset();
  }

  /**
   * @brief The main override required by a BT condition
   * @return BT::NodeStatus Status of tick execution
   */
  BT::NodeStatus tick() override;

  /**
   * @brief Creates list of BT ports
   * @return BT::PortsList Containing node-specific ports
   */
  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("executor_name", "Name of the executor performing the action"),
      BT::InputPort<std::string>("intent", "Intent from RASA"),
      BT::InputPort<std::string>("entity", "Entity from RASA"),
      BT::InputPort<std::string>("question", "Question received"),
      BT::InputPort<std::string>("role", "Role of the asker"),
      BT::OutputPort<std::string>("text", "Prompt for Llama")
    };
  }

private:
  // DSR graph
  std::shared_ptr<DSR::DSRGraph> G_;
  std::string executor_name_;
  std::string decorator_name_;

  std::optional<uint64_t> current_action_id_;

  std::string csvFile_;
  std::string dictionaryCsvFile_;
};

#endif  // PLANNER_AGENT__EXECUTOR__CONDITION__GET_CAUSES_HPP_
