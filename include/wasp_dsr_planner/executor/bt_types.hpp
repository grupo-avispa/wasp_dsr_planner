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

#include "behaviortree_cpp/behavior_tree.h"
#include "behaviortree_cpp/json_export.h"
#include "wasp_dsr_planner/dsr_api_ext.hpp"
#include "wasp_dsr_planner/helpers.hpp"

struct Goal
{
  float x;
  float y;
  float yaw;

  bool operator==(const Goal & p) const
  {
    return x == p.x && y == p.y && yaw == p.yaw;
  }
  bool operator!=(const Goal & p) const
  {
    return !(*this == p);
  }
};

// Allow bi-directional convertion to JSON
BT_JSON_CONVERTER(Goal, goal)
{
  add_field("x", &goal.x);
  add_field("y", &goal.y);
  add_field("yaw", &goal.yaw);
}

BT_JSON_CONVERTER(DSR::Node, node)
{
  add_field("name", &node.name());
  add_field("type", &node.type());
}

BT_JSON_CONVERTER(DSR::DSRGraph, graph)
{
  auto nodes_size = graph.get_nodes().size();
  std::vector<DSR::Edge> edges;
  for (const auto & node : graph.get_nodes()) {
    if (auto edges_gr = graph.get_edges(node.id()); edges_gr.has_value()) {
      for (const auto & edge_pair : edges_gr.value()) {
        edges.push_back(edge_pair.second);
      }
    }
  }
  auto edges_size = edges.size();
  add_field("nodes", &nodes_size);
  add_field("edges", &edges_size);
}

using AttrMap = std::map<std::string, DSR::Attribute>;
BT_JSON_CONVERTER(AttrMap, attributes)
{
  auto att_str = dsr_util::helpers::attributes_to_string(attributes);
  for (unsigned int i = 0; i < att_str.size(); i += 3) {
    add_field("name", &att_str[i]);
    add_field("value", &att_str[i + 1]);
    add_field("type", &att_str[i + 2]);
  }
}

// Template specialization to converts a string to custom types
namespace BT
{
template<>
[[nodiscard]] Goal convertFromString<Goal>(BT::StringView str)
{
  if (StartWith(str, "json:")) {
    str.remove_prefix(5);
    return convertFromJSON<Goal>(str);
  }

  // We expect real numbers separated by semicolons
  const auto parts = splitString(str, ',');
  if (parts.size() != 3) {
    throw BT::RuntimeError("invalid input)");
  } else {
    Goal output;
    output.x = convertFromString<float>(parts[0]);
    output.y = convertFromString<float>(parts[1]);
    output.yaw = convertFromString<float>(parts[2]);
    return output;
  }
}

template<>
[[nodiscard]] DSR::Node convertFromString<DSR::Node>(BT::StringView str)
{
  if (StartWith(str, "json:")) {
    str.remove_prefix(5);
    return convertFromJSON<DSR::Node>(str);
  }

  // We expect string values separated by semicolons: "name,type"
  auto parts = splitString(str, ',');
  if (parts.size() != 2) {
    throw BT::RuntimeError("invalid node)");
  } else {
    auto type = convertFromString<std::string>(parts[1]);
    if (!node_types::check_type(type)) {
      throw BT::RuntimeError("Error, [" + type + "] is not a valid node type");
    } else {
      DSR::Node output;
      output.name(convertFromString<std::string>(parts[0]));
      output.type(type);
      return output;
    }
  }
}

template<>
[[nodiscard]] std::map<std::string, DSR::Attribute>
convertFromString<std::map<std::string, DSR::Attribute>>(BT::StringView str)
{
  if (StartWith(str, "json:")) {
    str.remove_prefix(5);
    return convertFromJSON<std::map<std::string, DSR::Attribute>>(str);
  }

  // We expect string values separated by semicolons: "name,value,type"
  auto parts = splitString(str, ',');
  if (parts.size() % 3 != 0) {
    throw BT::RuntimeError("invalid attribute)");
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
