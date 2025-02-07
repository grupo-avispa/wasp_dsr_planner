// Copyright (c) 2024 Alberto J. Tudela Roldán
// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
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

#ifndef PLANNER_AGENT__JSON_MESSAGES_
#define PLANNER_AGENT__JSON_MESSAGES_

#include <iostream>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Battery State
struct BatteryState
{
  float voltage;
  float temperature;
  float current;
  float charge;
  float capacity;
  float design_capacity;
  float percentage;
  std::string power_supply_status;
  int power_supply_health;
  int power_supply_technology;
  bool present;
  std::vector<float> cell_voltage;
  std::vector<float> cell_temperature;
  std::string location;
  std::string serial_number;

  bool is_valid()
  {
    return percentage >= 0.0f && percentage <= 100.0f && !power_supply_status.empty();
  }
};

/* Check if is a valid JSON BatteryState */
inline bool isValidJsonBatteryState(const json & j)
{
  if (!j.contains("percentage")) {return false;}
  if (!j.contains("power_supply_status")) {return false;}
  return true;
}

/* Convert a BatteryState struct into a JSON file */
inline void to_json(json & j, const BatteryState & d)
{
  j["voltage"] = d.voltage;
  j["temperature"] = d.temperature;
  j["current"] = d.current;
  j["charge"] = d.charge;
  j["capacity"] = d.capacity;
  j["design_capacity"] = d.design_capacity;
  j["percentage"] = d.percentage;
  j["power_supply_status"] = d.power_supply_status;
  j["power_supply_health"] = d.power_supply_health;
  j["power_supply_technology"] = d.power_supply_technology;
  j["present"] = d.present;
  j["cell_voltage"] = d.cell_voltage;
  j["cell_temperature"] = d.cell_temperature;
  j["location"] = d.location;
  j["serial_number"] = d.serial_number;
}

/* Convert a JSON file into a BatteryState struct */
inline void from_json(const json & j, BatteryState & d)
{
        // Check if is a valid answer
  if (!isValidJsonBatteryState(j)) {return;}

        // Fill the battery struct
  j.at("voltage").get_to(d.voltage);
  j.at("temperature").get_to(d.temperature);
  j.at("current").get_to(d.current);
  j.at("charge").get_to(d.charge);
  j.at("capacity").get_to(d.capacity);
  j.at("design_capacity").get_to(d.design_capacity);
  j.at("percentage").get_to(d.percentage);
  j.at("power_supply_status").get_to(d.power_supply_status);
  j.at("power_supply_health").get_to(d.power_supply_health);
  j.at("power_supply_technology").get_to(d.power_supply_technology);
  j.at("present").get_to(d.present);
  j.at("cell_voltage").get_to(d.cell_voltage);
  j.at("cell_temperature").get_to(d.cell_temperature);
  j.at("location").get_to(d.location);
  j.at("serial_number").get_to(d.serial_number);
}

// Menu Choices
struct MenuChoices
{
  std::string primero1;
  std::string primero2;
  std::string segundo1;
  std::string segundo2;
  std::string postre1;
  std::string postre2;
};

/* Check if is a valid JSON MenuChoices */
inline bool isValidJsonMenuChoices(const json & j)
{
  if (!j.contains("primero1")) {return false;}
  if (!j.contains("primero2")) {return false;}
  if (!j.contains("segundo1")) {return false;}
  if (!j.contains("segundo2")) {return false;}
  if (!j.contains("postre1")) {return false;}
  if (!j.contains("postre2")) {return false;}
  return true;
}

/* Convert a MenuChoices struct into a JSON file */
inline void to_json(json & j, const MenuChoices & d)
{
  j["primero1"] = d.primero1;
  j["primero2"] = d.primero2;
  j["segundo1"] = d.segundo1;
  j["segundo2"] = d.segundo2;
  j["postre1"] = d.postre1;
  j["postre2"] = d.postre2;
}

/* Convert a JSON file into a MenuChoices struct */
inline void from_json(const json & j, MenuChoices & d)
{
        // Check if is a valid answer
  if (!isValidJsonMenuChoices(j)) {return;}

        // Fill the MenuChoices struct
  j.at("primero1").get_to(d.primero1);
  j.at("primero2").get_to(d.primero2);
  j.at("segundo1").get_to(d.segundo1);
  j.at("segundo2").get_to(d.segundo2);
  j.at("postre1").get_to(d.postre1);
  j.at("postre2").get_to(d.postre2);
}

// Communication parameters of the user profile
struct CommParameters
{
  bool enable;
  float volume;
  bool subtitles;
  int text_size;
  bool only_images;
};

/* Check if is a valid JSON CommParameters */
inline bool isValidJsonCommParameters(const json & j)
{
  if (!j.contains("enable")) {return false;}
  if (!j.contains("volume")) {return false;}
  if (!j.contains("subtitles")) {return false;}
  if (!j.contains("text_size")) {return false;}
  if (!j.contains("only_images")) {return false;}
  return true;
}

/* Convert a CommParameters struct into a JSON file */
inline void to_json(json & j, const CommParameters & d)
{
  j["enable"] = d.enable;
  j["volume"] = d.volume;
  j["subtitles"] = d.subtitles;
  j["text_size"] = d.text_size;
  j["only_images"] = d.only_images;
}

/* Convert a JSON file into a CommParameters struct */
inline void from_json(const json & j, CommParameters & d)
{
        // Check if is a valid answer
  if (!isValidJsonCommParameters(j)) {return;}

        // Fill the CommParameters struct
  j.at("enable").get_to(d.enable);
  j.at("volume").get_to(d.volume);
  j.at("subtitles").get_to(d.subtitles);
  j.at("text_size").get_to(d.text_size);
  j.at("only_images").get_to(d.only_images);
}

// Person profile
struct Profile
{
  int avisual;
  int ccogni;
  int cmov;
  int disco;
  int humor;
  int inro;
  int naudicion;
  int ncansancio;
};

/* Check if is a valid JSON Profile */
inline bool isValidJsonProfile(const json & j)
{
  if (!j.contains("avisual")) {return false;}
  if (!j.contains("ccogni")) {return false;}
  if (!j.contains("cmov")) {return false;}
  if (!j.contains("disco")) {return false;}
  if (!j.contains("humor")) {return false;}
  if (!j.contains("inro")) {return false;}
  if (!j.contains("naudicion")) {return false;}
  if (!j.contains("ncansancio")) {return false;}
  return true;
}

/* Convert a Profile struct into a JSON file */
inline void to_json(json & j, const Profile & d)
{
  j["avisual"] = d.avisual;
  j["ccogni"] = d.ccogni;
  j["cmov"] = d.cmov;
  j["disco"] = d.disco;
  j["humor"] = d.humor;
  j["inro"] = d.inro;
  j["naudicion"] = d.naudicion;
  j["ncansancio"] = d.ncansancio;
}

/* Convert a JSON file into a CommParameters struct */
inline void from_json(const json & j, Profile & d)
{
        // Check if is a valid answer
  if (!isValidJsonProfile(j)) {return;}

        // Fill the Profile struct
  j.at("avisual").get_to(d.avisual);
  j.at("ccogni").get_to(d.ccogni);
  j.at("cmov").get_to(d.cmov);
  j.at("disco").get_to(d.disco);
  j.at("humor").get_to(d.humor);
  j.at("inro").get_to(d.inro);
  j.at("naudicion").get_to(d.naudicion);
  j.at("ncansancio").get_to(d.ncansancio);
}

// Activities
struct Activity
{
  std::string nombre;
  std::string hora_inicio;
  std::string hora_fin;
  std::string lugar;
};

/* Check if is a valid JSON Activity */
inline bool isValidJsonActivity(const json & j)
{
  if (!j.contains("nombre")) {return false;}
  if (!j.contains("hora_inicio")) {return false;}
  if (!j.contains("hora_fin")) {return false;}
  if (!j.contains("lugar")) {return false;}
  return true;
}

/* Convert a Activity struct into a JSON file */
inline void to_json(json & j, const Activity & d)
{
  j["nombre"] = d.nombre;
  j["hora_inicio"] = d.hora_inicio;
  j["hora_fin"] = d.hora_fin;
  j["lugar"] = d.lugar;
}

/* Convert a JSON file into a Activity struct */
inline void from_json(const json & j, Activity & d)
{
        // Check if is a valid answer
  if (!isValidJsonActivity(j)) {return;}

        // Fill the Activity struct
  j.at("nombre").get_to(d.nombre);
  j.at("hora_inicio").get_to(d.hora_inicio);
  j.at("hora_fin").get_to(d.hora_fin);
  j.at("lugar").get_to(d.lugar);
}

/* Deserialize a list of answer in JSON format into a vector of Activity */
inline std::vector<Activity> getActivityfromJstring(std::string listStr)
{
  std::vector<Activity> activities;
        // Check if JSON format is right
  if (json::accept(listStr)) {
    json jActivities = json::parse(listStr);
    for (auto & jActivity : jActivities["activities"]) {
      auto activity = jActivity.get<Activity>();
      activities.push_back(activity);
    }
  }
  return activities;
}

/* Serialize a vector of Activity into JSON string */
inline std::string getJStringFromActivities(std::vector<Activity> activities, int indent = -1)
{
  json jObject;
  jObject["activities"] = activities;
  return jObject.dump(indent);
}

// BBDD changes
struct BBDDchanges
{
  std::string collection;
  std::string person;
  std::string menu_selected;
};

/* Check if is a valid JSON BBDDchanges */
inline bool isValidJsonBBDDchanges(const json & j)
{
  if (!j.contains("collection")) {return false;}
  if (!j.contains("person")) {return false;}
  if (!j.contains("menu_selected")) {return false;}
  return true;
}

/* Convert a BBDDchanges struct into a JSON file */
inline void to_json(json & j, const BBDDchanges & d)
{
  j["collection"] = d.collection;
  j["person"] = d.person;
  j["menu_selected"] = d.menu_selected;
}

/* Convert a JSON file into a BBDDchanges struct */
inline void from_json(const json & j, BBDDchanges & d)
{
        // Check if is a valid answer
  if (!isValidJsonBBDDchanges(j)) {return;}

        // Fill the BBDDchanges struct
  j.at("collection").get_to(d.collection);
  j.at("person").get_to(d.person);
  j.at("menu_selected").get_to(d.menu_selected);
}

struct EmailSender
{
  int server_port;
  std::string server_address;
  std::string sender_address;
  std::string sender_password;
  std::string sender_name;
};

/* Check if is a valid JSON EmailSender */
inline bool isValidJsonEmailSender(const json & j)
{
  if (!j["sender"].contains("server_port")) {return false;}
  if (!j["sender"].contains("server_address")) {return false;}
  if (!j["sender"].contains("sender_address")) {return false;}
  if (!j["sender"].contains("sender_password")) {return false;}
  if (!j["sender"].contains("sender_name")) {return false;}

  return true;
}

/* Convert a EmailSender struct into a JSON file */

inline void to_json(json & j, const EmailSender & d)
{
  j["sender"]["server_port"] = d.server_port;
  j["sender"]["server_address"] = d.server_address;
  j["sender"]["sender_address"] = d.sender_address;
  j["sender"]["sender_password"] = d.sender_password;
  j["sender"]["sender_name"] = d.sender_name;
}

/* Convert a JSON file into a EmailSender struct */
inline void from_json(const json & j, EmailSender & d)
{
        // Check if is a valid answer
  if (!isValidJsonEmailSender(j)) {return;}

        // Fill the EmailSender struct
  j["sender"].at("server_port").get_to(d.server_port);
  j["sender"].at("server_address").get_to(d.server_address);
  j["sender"].at("sender_address").get_to(d.sender_address);
  j["sender"].at("sender_password").get_to(d.sender_password);
  j["sender"].at("sender_name").get_to(d.sender_name);
}

struct EmailRecipient
{
  std::string name;
  std::string address;
};

/* Check if is a valid JSON EmailRecipient */
inline bool isValidJsonEmailRecipient(const json & j)
{
  if (!j["recipient"].contains("name")) {return false;}
  if (!j["recipient"].contains("address")) {return false;}
  return true;
}

/* Convert a EmailRecipient struct into a JSON file */
inline void to_json(json & j, const EmailRecipient & d)
{
  j["recipient"]["name"] = d.name;
  j["recipient"]["address"] = d.address;
}

/* Convert a JSON file into a EmailRecipient struct */
inline void from_json(const json & j, EmailRecipient & d)
{
        // Check if is a valid answer
  if (!isValidJsonEmailRecipient(j)) {return;}

        // Fill the EmailRecipient struct
  j["recipient"].at("name").get_to(d.name);
  j["recipient"].at("address").get_to(d.address);
}

// Menu Choices
struct MenuSelection
{
  std::string primero;
  std::string segundo;
};

/* Check if is a valid JSON MenuSelection */
inline bool isValidJsonMenuSelection(const json & j)
{
  if (!j.contains("primero")) {return false;}
  if (!j.contains("segundo")) {return false;}
  return true;
}

/* Convert a MenuSelection struct into a JSON file */
inline void to_json(json & j, const MenuSelection & d)
{
  j["primero"] = d.primero;
  j["segundo"] = d.segundo;
}

/* Convert a JSON file into a MenuSelection struct */
inline void from_json(const json & j, MenuSelection & d)
{
        // Check if is a valid answer
  if (!isValidJsonMenuSelection(j)) {return;}

        // Fill the MenuSelection struct
  j.at("primero").get_to(d.primero);
  j.at("segundo").get_to(d.segundo);
}


#endif  // PLANNER_AGENT__JSON_MESSAGES_
