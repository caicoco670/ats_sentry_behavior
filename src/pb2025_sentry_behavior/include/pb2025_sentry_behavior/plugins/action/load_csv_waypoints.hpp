#ifndef PB2025_SENTRY_BEHAVIOR__PLUGINS__ACTION__LOAD_CSV_WAYPOINTS_HPP_
#define PB2025_SENTRY_BEHAVIOR__PLUGINS__ACTION__LOAD_CSV_WAYPOINTS_HPP_

#include <string>
#include <vector>

#include "behaviortree_cpp/action_node.h"
#include "geometry_msgs/msg/point.hpp"
#include "nav_msgs/msg/path.hpp"
#include "rclcpp/rclcpp.hpp"

namespace pb2025_sentry_behavior
{

class LoadCsvWaypointsAction : public BT::SyncActionNode
{
public:
  LoadCsvWaypointsAction(const std::string & name, const BT::NodeConfig & config);

  static BT::PortsList providedPorts();

private:
  BT::NodeStatus tick() override;
  bool parseCsv(const std::string & filepath);

  rclcpp::Logger logger_ = rclcpp::get_logger("LoadCsvWaypoints");
  std::vector<geometry_msgs::msg::Point> waypoints_;
  nav_msgs::msg::Path cached_path_;
  std::string loaded_filepath_;
  bool csv_loaded_ = false;
};

}  // namespace pb2025_sentry_behavior

#endif  // PB2025_SENTRY_BEHAVIOR__PLUGINS__ACTION__LOAD_CSV_WAYPOINTS_HPP_
