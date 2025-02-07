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

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <boost/algorithm/string.hpp>

#include "plannerAgent/behavior_tree_engine.hpp"

// Trim the string
std::string trim(const std::string & str)
{
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first) {
    return str;
  }
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

// Parse the configuration file
std::map<std::string, std::string> parseConfig(const std::string & configFilePath)
{
  std::map<std::string, std::string> configParams;
  std::ifstream configFile(configFilePath);
  std::string line;

  if (configFile.is_open()) {
    while (getline(configFile, line)) {
      // Ignore comments and empty lines
      if (line[0] == '#' || line.empty()) {continue;}

      size_t delimiterPos = line.find('=');
      if (delimiterPos != std::string::npos) {
        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));
        configParams[key] = value;
      }
    }
    configFile.close();
  } else {
    std::cerr << "Unable to open config file: " << configFilePath << std::endl;
  }

  return configParams;
}

int main(int argc, char * argv[])
{
  QCoreApplication app(argc, argv);

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
    return 1;
  }

  // Get the configuration parameters
  std::string configFilePath = argv[1];
  auto config = parseConfig(configFilePath);
  auto agent_name = config["agent_name"];
  auto agent_id = config["agent_id"].empty() ? 0 : std::stoi(config["agent_id"]);
  auto robot_name = config["robot_name"];
  auto tree_filename = config["bt_tree_file"];
  auto log_filepath = config["bt_log_path"];
  auto publisher_port =
    config["groot_publisher_port"].empty() ? 1666 : std::stoi(config["groot_publisher_port"]);
  auto use_dsr = config["use_dsr"].empty() ? true : (config["use_dsr"] == "true" ? true : false);
  auto plugin_libs = config["plugin_libs"];

  // Split the plugin libraries
  std::vector<std::string> user_libs;
  boost::trim(plugin_libs);
  if (!plugin_libs.empty()) {
    boost::split(user_libs, plugin_libs, boost::is_any_of(";"));
  }

  std::cout << "Configuration parameters for the plannerAgent:" << std::endl;
  std::cout << "Agent name: " << agent_name << std::endl;
  std::cout << "Agent id: " << agent_id << std::endl;
  std::cout << "Robot name: " << robot_name << std::endl;
  std::cout << "Tree filename: " << tree_filename << std::endl;
  std::cout << "Log filepath: " << log_filepath << std::endl;
  std::cout << "Groot2 publisher port: " << publisher_port << std::endl;
  std::cout << "Use DSR: " << (use_dsr ? "true" : "false") << std::endl;
  std::cout << "Plugin libraries: " << plugin_libs.size() << std::endl;

  // Create the behavior tree engine
  auto tree = std::make_shared<BehaviorTreeEngine>(agent_name, agent_id, robot_name, use_dsr);
  // Add Groot 2 publisher to publish BT status changes
  tree->setGrootMonitoring(publisher_port);
  // Initialize the behavior tree
  tree->initBehaviorTree(tree_filename, log_filepath, user_libs);

  return app.exec();
}
