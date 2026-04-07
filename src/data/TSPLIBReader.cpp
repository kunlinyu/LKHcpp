#include <plog/Log.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "data/Problem.h"
#include "data/TSPLIBReader.h"
#include "type.h"
#include "utils/EnumConversion.h"

DECLARE_ENUM_CONVERSION(ProblemType, {
                                         {TSP, "TSP"},
                                         {ATSP, "ATSP"},
                                         {HCP, "HCP"},
                                         {STTSP, "STTSP"},
                                     });

DECLARE_ENUM_CONVERSION(EdgeWeightTypes, {
                                             {UNSET_TYPE, "UNSET_TYPE"},
                                             {EXPLICIT, "EXPLICIT"},
                                             {EUC_2D, "EUC_2D"},
                                             {EUC_3D, "EUC_3D"},
                                             {MAX_2D, "MAX_2D"},
                                             {MAX_3D, "MAX_3D"},
                                             {MAN_2D, "MAN_2D"},
                                             {MAN_3D, "MAN_3D"},
                                             {CEIL_2D, "CEIL_2D"},
                                             {CEIL_3D, "CEIL_3D"},
                                             {GEO, "GEO"},
                                             {GEOM, "GEOM"},
                                             {GEO_MEEUS, "GEO_MEEUS"},
                                             {GEOM_MEEUS, "GEOM_MEEUS"},
                                             {ATT, "ATT"},
                                             {XRAY1, "XRAY1"},
                                             {XRAY2, "XRAY2"},
                                             {SPECIAL, "SPECIAL"},
                                         });

DECLARE_ENUM_CONVERSION(EdgeWeightFormat,
                        {
                            {UNSET_FORMAT, "UNSET_FORMAT"},
                            {FUNCTION, "FUNCTION"},
                            {FULL_MATRIX, "FULL_MATRIX"},
                            {UPPER_ROW, "UPPER_ROW"},
                            {LOWER_ROW, "LOWER_ROW"},
                            {UPPER_DIAG_ROW, "UPPER_DIAG_ROW"},
                            {LOWER_DIAG_ROW, "LOWER_DIAG_ROW"},
                            {UPPER_COL, "UPPER_COL"},
                            {LOWER_COL, "LOWER_COL"},
                            {UPPER_DIAG_COL, "UPPER_DIAG_COL"},
                            {LOWER_DIAG_COL, "LOWER_DIAG_COL"},
                        });

DECLARE_ENUM_CONVERSION(CoordTypes, {
                                        {TWOD_COORDS, "TWOD_COORDS"},
                                        {THREED_COORDS, "THREED_COORDS"},
                                        {NO_COORDS, "NO_COORDS"},
                                    });

DECLARE_ENUM_CONVERSION(DisplayDataType, {
                                             {COORD_DISPLAY, "COORD_DISPLAY"},
                                             {TWOD_DISPLAY, "TWOD_DISPLAY"},
                                             {NO_DISPLAY, "NO_DISPLAY"},
                                         });

DECLARE_ENUM_CONVERSION(EdgeDataFormat, {
                                            {NO_FORMAT, "NO_FORMAT"},
                                            {EDGE_LIST, "EDGE_LIST"},
                                            {ADJ_LIST, "ADJ_LIST"},
                                        });

TSPLIB TSPLIBReader::Read(std::istream& is) {
  TSPLIB pb;
  std::string line;
  auto field_funcs = FieldFuncs();
  auto section_funcs = SectionFuncs();
  std::string current_section;
  while (std::getline(is, line)) {
    std::string trim_line = trim(line);
    std::string::size_type colonPos = line.find(':');

    // case 0: EOF
    if (trim_line == "EOF") {
      break;

      // case 1: section header
    } else if (section_funcs.find(trim_line) != section_funcs.end()) {
      current_section = trim_line;

      // case 2: key value pair
    } else if (colonPos != std::string::npos) {
      auto key = trim(line.substr(0, colonPos));
      auto value = trim(line.substr(colonPos + 1));

      if (field_funcs.find(key) != field_funcs.end())
        if (not field_funcs.at(key)(value, pb)) {
          PLOGE << "read field \"" << key << "\" error";
          throw std::invalid_argument("parse pb file error in line: " + line);
        }

      // case 3: unknown section
    } else if (trim_line.find("SECTION") != std::string::npos) {
      PLOGE << "unknown section: " << trim_line << std::endl;
      current_section = "";

      // case 4: data of section
    } else {
      if (section_funcs.find(current_section) != section_funcs.end())
        section_funcs.at(current_section)(line, pb);
    }
  }

  Check(pb);
  return pb;
}

std::string TSPLIBReader::trim(const std::string& str) {
  auto start = str.find_first_not_of(" \t\r\n");
  auto end = str.find_last_not_of(" \t\r\n");
  if (start != std::string::npos && end != std::string::npos)
    return str.substr(start, end - start + 1);
  else
    return "";
}

std::vector<std::string> TSPLIBReader::split(const std::string& str,
                                              const std::string& delimiters) {
  std::vector<std::string> components;
  size_t start = 0;
  size_t end = 0;
  while (end != std::string::npos) {
    end = str.find_first_of(delimiters, start);
    std::string token = str.substr(start, end - start);
    if (!token.empty()) components.push_back(token);
    if (end != std::string::npos) start = end + 1;
  }
  return components;
}

const std::map<std::string, TSPLIBReader::FieldFunc>&
TSPLIBReader::FieldFuncs() {
  static std::map<std::string, FieldFunc> field_funcs = {
      {"NAME",
       [](const std::string& v, TSPLIB& pb) {
         pb.name = v;
         return true;
       }},
      {"TYPE",  // ^_^
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.type);
       }},
      {"COMMENT",
       [](const std::string& v, TSPLIB& pb) {
         pb.comments.emplace_back(v);
         return true;
       }},
      {"DIMENSION",
       [](const std::string& v, TSPLIB& pb) {
         try {
           pb.dimension = std::stoll(v);
           return true;
         } catch (const std::exception& e) {
           return false;
         }
       }},
      {"EDGE_DATA_FORMAT",
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.edge_data_format);
       }},
      {"CAPACITY",
       [](const std::string& v, TSPLIB& pb) {
         try {
           pb.capacity = std::stoll(v);
           return true;
         } catch (const std::exception& e) {
           return false;
         }
       }},
      {"CAPACITY",
       [](const std::string& v, TSPLIB& pb) {
         try {
           pb.capacity = std::stoll(v);
           return true;
         } catch (const std::exception& e) {
           return false;
         }
       }},
      {"EDGE_WEIGHT_TYPE",
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.edge_weight_type);
       }},
      {"EDGE_WEIGHT_FORMAT",
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.edge_weight_format);
       }},
      {"EDGE_WEIGHT_FORMAT",
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.edge_weight_format);
       }},
      {"NODE_COORD_TYPE",
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.node_coord_type);
       }},
      {"DISPLAY_DATA_TYPE",
       [](const std::string& v, TSPLIB& pb) {
         return from_string(v, &pb.display_data_type);
       }},
  };

  return field_funcs;
}

const std::map<std::string, TSPLIBReader::SectionFunc>&
TSPLIBReader::SectionFuncs() {
  static std::map<std::string, SectionFunc> section_funcs = {
      {"NODE_COORD_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         auto components = split(line, " \t\r\n");
         if (components.size() < 3 || components.size() > 4) return false;

         Coordinate node_coord{};
         try {
           NodeIdType id = std::stoll(components[0]);
           node_coord.X = std::stod(components[1]);
           node_coord.Y = std::stod(components[2]);
           if (components.size() == 4)
             node_coord.Z = std::stod(components[3]);
           else
             node_coord.Z = 0;
           pb.node_coord_section[id] = (node_coord);
         } catch (const std::exception& e) {
           return false;
         }
         return true;
       }},
      {"DEPOT_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         NodeIdType node_id;
         try {
           auto value = std::stoll(line);
           if (value < 0) return false;
           node_id = value;
         } catch (const std::exception& e) {
           return false;
         }
         pb.depot_section.emplace_back(node_id);
         return true;
       }},
      {"DEMAND_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         auto components = split(line, " \t\r\n");
         if (components.size() < 2) return false;

         NodeIdType node_id;
         std::vector<DemandType> demands;
         try {
           node_id = std::stoll(components[0]);
           for (size_t i = 1; i < components.size(); i++)
             demands.emplace_back(std::stoll(components[i]));
         } catch (const std::exception& e) {
           return false;
         }

         pb.demand_section[node_id] = demands;
         return true;
       }},
      {"EDGE_DATA_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         auto components = split(line, " \t\r\n");
         if (components.size() < 2) return false;
         EdgeData edge_data;
         std::pair<NodeIdType, std::vector<NodeIdType>> pair;

         if (pb.edge_data_format == EDGE_LIST) {
           edge_data.from = std::stoll(components[0]);
           edge_data.to = std::stoll(components[1]);
           if (components.size() > 2)
             edge_data.weight = std::stoll(components[2]);
         } else if (pb.edge_data_format == ADJ_LIST) {
           PLOGE << "unsported ADJ_LIST";
           return false;
         } else {
           PLOGE << "unknown EDGE_DATA_FORMAT";
           return false;
         }

         pb.edge_data_section.emplace_back(edge_data);
         return true;
       }},
      {"FIXED_EDGES_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         auto components = split(line, " \t\r\n");
         if (components.size() != 2) return false;
         std::pair<NodeIdType, NodeIdType> pair;

         try {
           pair.first = std::stoll(components[0]);
           pair.second = std::stoll(components[1]);
         } catch (const std::exception& e) {
           return false;
         }

         pb.fixed_edges_section.emplace_back(pair);
         return true;
       }},
      {"DISPLAY_DATA_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         auto components = split(line, " \t\r\n");
         if (components.size() != 3) return false;

         Coordinate node_coord;
         try {
           NodeIdType id = std::stoll(components[0]);
           node_coord.X = std::stod(components[1]);
           node_coord.Y = std::stod(components[2]);
           node_coord.Z = 0;
           pb.display_data_section[id] = node_coord;
         } catch (const std::exception& e) {
           return false;
         }
         return true;
       }},
      {"TOUR_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         NodeIdType node_id;
         try {
           auto value = std::stoll(line);
           if (value < 0) return false;
           node_id = value;
         } catch (const std::exception& e) {
           return false;
         }
         pb.tour_section.emplace_back(node_id);
         return true;
       }},
      {"EDGE_WEIGHT_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         std::vector<WeightType> edge_weight_line;
         try {
           for (const auto& component : split(line, " \t\r\n"))
             edge_weight_line.emplace_back(std::stoll(component));
         } catch (const std::exception& e) {
           return false;
         }
         pb.edge_weight_section.emplace_back(edge_weight_line);
         return true;
       }},
      {"REQUIRED_NODES_SECTION",
       [](const std::string& line, TSPLIB& pb) {
         NodeIdType node_id;
         try {
           auto value = std::stoll(line);
           if (value < 0) return false;
           node_id = value;
         } catch (const std::exception& e) {
           return false;
         }
         pb.required_nodes_section.insert(node_id);
         return true;
       }},
  };
  return section_funcs;
}

void TSPLIBReader::Check(TSPLIB& pb) {
  if (pb.type == UNKNOW_PROBLEM) throw std::invalid_argument("TYPE is missing");
  if (pb.dimension < 3)
    throw std::invalid_argument("DIMENSION < 3 or not specified");
  if (pb.edge_weight_type == UNSET_TYPE && !pb.IsAsymmetric() &&
      pb.type != HCP && pb.type != HPP && pb.type != STTSP)
    throw std::invalid_argument("EDGE_WEIGHT_TYPE is missing");
  if (pb.edge_weight_type == EXPLICIT && pb.edge_weight_format == UNSET_FORMAT)
    throw std::invalid_argument("EDGE_WEIGHT_FORMAT is missing");
  if (pb.edge_weight_type == EXPLICIT && pb.edge_weight_format == FUNCTION)
    throw std::invalid_argument(
        "Conflicting EDGE_WEIGHT_TYPE and EDGE_WEIGHT_FORMAT");
  if (pb.edge_weight_type != EXPLICIT &&
      pb.edge_weight_format != UNSET_FORMAT &&
      pb.edge_weight_format != FUNCTION)
    throw std::invalid_argument(
        "Conflicting EDGE_WEIGHT_TYPE and EDGE_WEIGHT_FORMAT");
  if (pb.type == ATSP && pb.edge_weight_type != EXPLICIT &&
      pb.edge_weight_type != UNSET_TYPE)
    throw std::invalid_argument("Conflicting TYPE and EDGE_WEIGHT_TYPE");
}