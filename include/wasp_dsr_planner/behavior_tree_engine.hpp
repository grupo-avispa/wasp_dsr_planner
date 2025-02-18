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

#ifndef PLANNER_AGENT__BEHAVIOR_TREE_ENGINE_HPP_
#define PLANNER_AGENT__BEHAVIOR_TREE_ENGINE_HPP_

#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/behavior_tree.h"
#include "behaviortree_cpp/loggers/groot2_publisher.h"

// DSR
#include "dsr/api/dsr_api.h"

/**
 * @class BehaviorTreeEngine
 * @brief A class to create and handle behavior trees
 */
class BehaviorTreeEngine
{
public:
  /**
   * @brief Constructs a BehaviorTreeEngine object.
   *
   * @param agent_name The name of the agent.
   * @param agent_id The ID of the agent.
   * @param executor_name The name of the executor.
   * @param source The source of the action.
   */
  BehaviorTreeEngine(
    std::string agent_name, int agent_id, std::string executor_name, std::string source);

  /**
   * @brief Destructor for the BehaviorTreeEngine object.
   */
  ~BehaviorTreeEngine();

  /**
   * @brief Initializes the behavior tree.
   *
   * @param tree_filename The filename of the behavior tree.
   * @param log_filepath The filepath for logging.
   * @param user_libs A vector of user libraries.
   */
  void initBehaviorTree(
    std::string tree_filename, std::string log_filepath,
    const std::vector<std::string> & user_libs);

  /**
   * @brief Add Groot2 publisher to publish BT status changes.
   *
   * @param publisher_port Groot2 publisher port
   */
  void setGrootMonitoring(uint16_t publisher_port = 1667);

private:
  /**
   * @brief Inserts the DSR into the blackboard of the tree.
   *
   * @param tree The behavior tree.
   */
  void insertDsrIntoBlackboard(BT::Tree & tree);

  std::shared_ptr<DSR::DSRGraph> G_;
  std::string executor_name_;
  std::string source_;

  BT::Tree tree_;
  BT::Blackboard::Ptr blackboard_;

  // Groot2 monitoring
  std::unique_ptr<BT::Groot2Publisher> groot_publisher_;
  uint16_t publisher_port_;
};


#endif  // PLANNER_AGENT__BEHAVIOR_TREE_ENGINE_HPP_
