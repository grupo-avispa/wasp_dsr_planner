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

#ifndef PLANNER_AGENT__EXECUTOR__CONDITION__SENTENCE_BUILDER_HPP_
#define PLANNER_AGENT__EXECUTOR__CONDITION__SENTENCE_BUILDER_HPP_

#include <string>

#include "behaviortree_cpp/decorator_node.h"
#include "dsr/api/dsr_api.h"

/**
 * @class SentenceBuilder
 * @brief This decorator will build a sentence based on the current use case and the person.
 */
class SentenceBuilder : public BT::DecoratorNode
{
public:
  SentenceBuilder(
    const std::string & xml_tag_name, const std::string & decorator_name,
    const BT::NodeConfiguration & conf);

  SentenceBuilder() = delete;

  ~SentenceBuilder()
  {
    std::cout << "[" << decorator_name_ << "]: Destroyed DSR-BT node" << std::endl;
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
      BT::InputPort<uint64_t>("node_id", "Identifier node of the person you are interacting with"),
      BT::InputPort<std::string>("use_case", "menu", "Use case that is running at the moment"),
      BT::OutputPort<std::string>("text", "Text to say")
    };
  }

private:
  // DSR graph
  std::shared_ptr<DSR::DSRGraph> G_;
  std::string robot_name_;
  std::string decorator_name_;

  std::optional<uint64_t> current_action_id_;
};

#endif  // PLANNER_AGENT__EXECUTOR__CONDITION__SENTENCE_BUILDER_HPP_
