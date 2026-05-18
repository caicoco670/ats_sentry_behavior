#include "pb2025_sentry_behavior/plugins/action/load_csv_waypoints.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "pb2025_sentry_behavior/decision_utils.hpp"

namespace pb2025_sentry_behavior
{

namespace
{

std::string trim(const std::string & str)
{
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
    return std::isspace(ch) != 0;
  });
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
    return std::isspace(ch) != 0;
  }).base();
  return (start < end) ? std::string(start, end) : std::string();
}

}  // namespace

LoadCsvWaypointsAction::LoadCsvWaypointsAction(
  const std::string & name, const BT::NodeConfig & config)
: BT::SyncActionNode(name, config)
{
}

BT::NodeStatus LoadCsvWaypointsAction::tick()
{
  std::string csv_file;
  if (!getInput("csv_file", csv_file) || csv_file.empty()) {
    RCLCPP_ERROR(logger_, "LoadCsvWaypoints did not receive a valid csv_file input");
    return BT::NodeStatus::FAILURE;
  }

  if (csv_loaded_ && csv_file == loaded_filepath_) {
    setOutput("path", cached_path_);
    return BT::NodeStatus::SUCCESS;
  }

  if (parseCsv(csv_file)) {
    setOutput("path", cached_path_);
    return BT::NodeStatus::SUCCESS;
  }

  return BT::NodeStatus::FAILURE;
}

bool LoadCsvWaypointsAction::parseCsv(const std::string & filepath)
{
  std::string resolved_path = filepath;

  if (!std::filesystem::path(filepath).is_absolute()) {
    try {
      const auto share_dir =
        ament_index_cpp::get_package_share_directory("pb2025_sentry_behavior");
      resolved_path = (std::filesystem::path(share_dir) / filepath).string();
    } catch (const std::exception & e) {
      RCLCPP_WARN(logger_, "Failed to resolve share directory, trying relative path: %s", e.what());
    }
  }

  std::ifstream file(resolved_path);
  if (!file.is_open()) {
    RCLCPP_ERROR(logger_, "Cannot open CSV file: %s", resolved_path.c_str());
    return false;
  }

  waypoints_.clear();
  std::string line;
  int line_number = 0;

  while (std::getline(file, line)) {
    ++line_number;
    const auto trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#') {
      continue;
    }

    std::istringstream line_stream(trimmed);
    std::string token;
    std::vector<double> values;

    while (std::getline(line_stream, token, ',')) {
      try {
        values.push_back(std::stod(trim(token)));
      } catch (const std::exception &) {
        RCLCPP_WARN(
          logger_, "Skipping malformed value in %s line %d: '%s'",
          resolved_path.c_str(), line_number, token.c_str());
        values.clear();
        break;
      }
    }

    if (values.size() == 3) {
      geometry_msgs::msg::Point point;
      point.x = values[0];
      point.y = values[1];
      point.z = values[2];
      waypoints_.push_back(point);
    } else if (!values.empty()) {
      RCLCPP_WARN(
        logger_, "Skipping line %d in %s: expected 3 values, got %zu",
        line_number, resolved_path.c_str(), values.size());
    }
  }

  if (waypoints_.empty()) {
    RCLCPP_ERROR(logger_, "CSV file %s contains no valid waypoints", resolved_path.c_str());
    return false;
  }

  cached_path_.header.frame_id = "map";
  cached_path_.poses.clear();
  cached_path_.poses.reserve(waypoints_.size());
  for (const auto & point : waypoints_) {
    cached_path_.poses.push_back(decision::makePoseStamped(point));
  }

  loaded_filepath_ = filepath;
  csv_loaded_ = true;
  RCLCPP_INFO(
    logger_, "Loaded %zu waypoints from %s", waypoints_.size(), resolved_path.c_str());
  return true;
}

BT::PortsList LoadCsvWaypointsAction::providedPorts()
{
  return {
    BT::InputPort<std::string>("csv_file", "{decision_rmuc_csv_file}", "CSV file path"),
    BT::OutputPort<nav_msgs::msg::Path>(
      "path", "{decision_csv_path}", "Path loaded from CSV")};
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<pb2025_sentry_behavior::LoadCsvWaypointsAction>("LoadCsvWaypoints");
}
