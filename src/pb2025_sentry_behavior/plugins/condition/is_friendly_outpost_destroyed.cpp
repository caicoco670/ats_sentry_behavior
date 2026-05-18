#include "pb2025_sentry_behavior/plugins/condition/is_friendly_outpost_destroyed.hpp"

namespace pb2025_sentry_behavior
{

IsFriendlyOutpostDestroyedCondition::IsFriendlyOutpostDestroyedCondition(
  const std::string & name, const BT::NodeConfig & config)
: BT::SimpleConditionNode(
    name, std::bind(&IsFriendlyOutpostDestroyedCondition::tickCondition, this), config)
{
}

BT::NodeStatus IsFriendlyOutpostDestroyedCondition::tickCondition()
{
  auto all_robot_hp = getInput<pb_rm_interfaces::msg::GameRobotHP>("all_robot_hp");
  auto robot_status = getInput<pb_rm_interfaces::msg::RobotStatus>("robot_status");
  if (!all_robot_hp || !robot_status) {
    RCLCPP_DEBUG(logger_, "GameRobotHP or RobotStatus message is not available");
    return BT::NodeStatus::FAILURE;
  }

  // robot_id >= 100 is blue team, otherwise red team
  const bool is_blue = (robot_status->robot_id >= 100);
  const uint16_t friendly_outpost_hp =
    is_blue ? all_robot_hp->blue_outpost_hp : all_robot_hp->red_outpost_hp;

  return (friendly_outpost_hp == 0) ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

BT::PortsList IsFriendlyOutpostDestroyedCondition::providedPorts()
{
  return {
    BT::InputPort<pb_rm_interfaces::msg::GameRobotHP>(
      "all_robot_hp", "{@referee_allRobotHP}", "All robot HP port on blackboard"),
    BT::InputPort<pb_rm_interfaces::msg::RobotStatus>(
      "robot_status", "{@referee_robotStatus}", "RobotStatus port on blackboard")};
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<pb2025_sentry_behavior::IsFriendlyOutpostDestroyedCondition>(
    "IsFriendlyOutpostDestroyed");
}
