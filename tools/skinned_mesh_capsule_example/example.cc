#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <exception>
#include <libsatsuma/Problems/BiMDF.hh>
#include <libsatsuma/Extra/Highlevel.hh>

struct Position {
  double x, y, z;
};

double distance(const Position& p1, const Position& p2) {
    return std::sqrt((p2.x - p1.x) * (p2.x - p1.x) +
                     (p2.y - p1.y) * (p2.y - p1.y) +
                     (p2.z - p1.z) * (p2.z - p1.z));
}

std::string node_to_string(const Satsuma::BiMDF::Node &node) {
  std::ostringstream oss;
  oss << reinterpret_cast<const void *>(&node);
  return oss.str();
}

int main(int argc, char *argv[]) {
  using namespace Satsuma;
  BiMDF bimdf;
  std::map<std::string, BiMDF::Edge> edges;
  std::map<BiMDF::Node, Position> positions;

  auto a = bimdf.add_node(); // Bone 1
  auto b = bimdf.add_node(); // Bone 2

  positions[a] = {0.0, 0.0, 0.0};
  positions[b] = {2.0, 0.0, 0.0};

  std::vector<BiMDF::Node> vertices;
  for (int i = 0; i < 4; ++i) {
    vertices.push_back(bimdf.add_node());
  }

  std::vector<Position> basePositions = {
      {1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {2.0, 1.0, 0.0}, {1.0, 1.0, 0.0}};

  for (int i = 0; i < 4; ++i) {
    positions[vertices[i]] = basePositions[i];
  }

  for (auto &vertex : vertices) {
    double distA = distance(positions[a], positions[vertex]);
    double distB = distance(positions[b], positions[vertex]);

    for (int j = 0; j < 8; ++j) {
      std::string edgeKeyA = node_to_string(a) + "_" + node_to_string(vertex) + "_" + std::to_string(j);
      std::string edgeKeyB = node_to_string(b) + "_" + node_to_string(vertex) + "_" + std::to_string(j);
      edges[edgeKeyA] = bimdf.add_edge({
          .u = a,
          .v = vertex,
          .u_head = true,
          .v_head = true,
          .cost_function = CostFunction::AbsDeviation{.target = distA, .weight = 1.0 / 8},
          .lower = 0, .upper = 10});
      edges[edgeKeyB] = bimdf.add_edge({
          .u = b,
          .v = vertex,
          .u_head = true,
          .v_head = true,
          .cost_function = CostFunction::AbsDeviation{.target = distB, .weight = 1.0 / 8},
          .lower = 0, .upper = 10});
    }
  }

  // Print the graph structure
  std::cout << "Graph Nodes and Positions:\n";
  for (const auto &[node, pos] : positions) {
    std::cout << "Node " << node_to_string(node) << ": Position (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
  }

  std::cout << "\nGraph Edges:\n";
  for (const auto &[name, edge] : edges) {
    size_t underscorePos = name.find('_');
    std::string fromNodeStr = name.substr(0, underscorePos);
    size_t secondUnderscorePos = name.find('_', underscorePos + 1);
    std::string toNodeStr = name.substr(underscorePos + 1, secondUnderscorePos - underscorePos - 1);
    std::cout << "Edge " << name << ": From " << fromNodeStr << " to " << toNodeStr << "\n";
  }

  auto config = BiMDFSolverConfig{.matching_solver = MatchingSolver::Lemon};
  try {
    auto result = solve_bimdf(bimdf, config);
    std::cout << "Total cost: " << result.cost << std::endl;

    for (const auto &[name, edge] : edges) {
      std::cout << "Flow on " << name << ": " << (*result.solution)[edge] << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
