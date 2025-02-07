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

#ifndef PLANNER_AGENT__EXECUTOR__BT_TYPES_HPP_
#define PLANNER_AGENT__EXECUTOR__BT_TYPES_HPP_

#include <behaviortree_cpp_v3/behavior_tree.h>
#include "../../../include/dsr_api_ext.hpp"
#include "../../../include/helpers.hpp"

struct Goal
{
  float x;
  float y;
  float yaw;
};

// Template specialization to converts a string to Goal.
namespace BT
{
template<> inline Goal convertFromString(BT::StringView str)
{
  // We expect real numbers separated by semicolons
  auto parts = splitString(str, ',');
  if (parts.size() != 3) {
    throw RuntimeError("invalid input)");
  } else {
    Goal output;
    output.x = convertFromString<double>(parts[0]);
    output.y = convertFromString<double>(parts[1]);
    output.yaw = convertFromString<double>(parts[2]);
    return output;
  }
}

template<> inline DSR::Node convertFromString(BT::StringView str)
{
  // We expect string values separated by semicolons
  auto parts = splitString(str, ',');
  if (parts.size() != 2) {
    throw RuntimeError("invalid node)");
  } else {
    auto type = convertFromString<std::string>(parts[1]);
    if (!node_types::check_type(type)) {
      throw std::runtime_error("Error, [" + type + "] is not a valid node type");
    } else {
      DSR::Node output;
      output.name(convertFromString<std::string>(parts[0]));
      output.type(type);
      return output;
    }
  }
}

template<> inline std::map<std::string, DSR::Attribute> convertFromString(BT::StringView str)
{
  // We expect string values separated by semicolons
  auto parts = splitString(str, ',');
  if (parts.size() % 3 != 0) {
    throw RuntimeError("invalid attribute)");
  } else {
    std::map<std::string, DSR::Attribute> output;
    for (unsigned int i = 0; i < parts.size(); i += 3) {
      auto name = convertFromString<std::string>(parts[i]);
      auto value = convertFromString<std::string>(parts[i + 1]);
      auto type = convertFromString<std::string>(parts[i + 2]);
      auto attr = dsr_util::helpers::string_to_attribute(value, std::stoi(type));
      output.insert({name, attr});
    }
    return output;
  }
}

} // end namespace BT

#endif  // PLANNER_AGENT__EXECUTOR__BT_TYPES_HPP_
