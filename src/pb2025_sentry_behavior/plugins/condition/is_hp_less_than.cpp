#include "pb2025_sentry_behavior/plugins/condition/is_hp_less_than.hpp"

namespace pb2025_sentry_behavior
{

IsHpLessThanCondition::IsHpLessThanCondition(
  const std::string & name, const BT::NodeConfig & config)
: BT::SimpleConditionNode(name, std::bind(&IsHpLessThanCondition::tickCondition, this), config)
{
}

BT::NodeStatus IsHpLessThanCondition::tickCondition()
{
  auto robot_status = getInput<pb_rm_interfaces::msg::RobotStatus>("robot_status");
  if (!robot_status) {
    RCLCPP_DEBUG(logger_, "RobotStatus message is not available");
    return BT::NodeStatus::FAILURE;
  }

  int threshold = 150;
  if (!getInput("threshold", threshold)) {
    RCLCPP_ERROR(logger_, "IsHpLessThan did not receive threshold input");
    return BT::NodeStatus::FAILURE;
  }

  return (robot_status->current_hp < threshold) ? BT::NodeStatus::SUCCESS
                                                 : BT::NodeStatus::FAILURE;
}

BT::PortsList IsHpLessThanCondition::providedPorts()
{
  return {
    BT::InputPort<pb_rm_interfaces::msg::RobotStatus>(
      "robot_status", "{@referee_robotStatus}", "RobotStatus port on blackboard"),
    BT::InputPort<int>("threshold", 150, "HP threshold for retreat")};
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<pb2025_sentry_behavior::IsHpLessThanCondition>("IsHpLessThan");
}
