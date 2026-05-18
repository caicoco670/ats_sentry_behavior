#include "pb2025_sentry_behavior/plugins/condition/is_csv_phase_complete.hpp"

namespace pb2025_sentry_behavior
{

IsCsvPhaseCompleteCondition::IsCsvPhaseCompleteCondition(
  const std::string & name, const BT::NodeConfig & config)
: BT::SimpleConditionNode(
    name, std::bind(&IsCsvPhaseCompleteCondition::tickCondition, this), config)
{
}

BT::NodeStatus IsCsvPhaseCompleteCondition::tickCondition()
{
  bool csv_phase_complete = false;
  if (!getInput("csv_phase_complete", csv_phase_complete)) {
    RCLCPP_WARN(logger_, "csv_phase_complete is not available on blackboard");
    return BT::NodeStatus::FAILURE;
  }

  return csv_phase_complete ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

BT::PortsList IsCsvPhaseCompleteCondition::providedPorts()
{
  return {
    BT::InputPort<bool>(
      "csv_phase_complete", "{decision_rmuc_csv_phase_complete}", "CSV nav phase complete flag")};
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<pb2025_sentry_behavior::IsCsvPhaseCompleteCondition>(
    "IsCsvPhaseComplete");
}
