#include "pb2025_sentry_behavior/plugins/condition/is_remain_time_less_than.hpp"

namespace pb2025_sentry_behavior
{

IsRemainTimeLessThanCondition::IsRemainTimeLessThanCondition(
  const std::string & name, const BT::NodeConfig & config)
: BT::SimpleConditionNode(
    name, std::bind(&IsRemainTimeLessThanCondition::tickCondition, this), config)
{
}

BT::NodeStatus IsRemainTimeLessThanCondition::tickCondition()
{
  auto game_status = getInput<pb_rm_interfaces::msg::GameStatus>("key_port");
  if (!game_status) {
    RCLCPP_DEBUG(logger_, "GameStatus message is not available");
    return BT::NodeStatus::FAILURE;
  }

  int threshold = 180;
  if (!getInput("threshold", threshold)) {
    RCLCPP_ERROR(logger_, "IsRemainTimeLessThan did not receive threshold input");
    return BT::NodeStatus::FAILURE;
  }

  return (game_status->stage_remain_time <= threshold) ? BT::NodeStatus::SUCCESS
                                                        : BT::NodeStatus::FAILURE;
}

BT::PortsList IsRemainTimeLessThanCondition::providedPorts()
{
  return {
    BT::InputPort<pb_rm_interfaces::msg::GameStatus>(
      "key_port", "{@referee_gameStatus}", "GameStatus port on blackboard"),
    BT::InputPort<int>("threshold", 180, "Remaining time threshold (seconds)")};
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<pb2025_sentry_behavior::IsRemainTimeLessThanCondition>(
    "IsRemainTimeLessThan");
}
