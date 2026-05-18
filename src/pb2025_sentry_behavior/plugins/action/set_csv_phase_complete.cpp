#include "pb2025_sentry_behavior/plugins/action/set_csv_phase_complete.hpp"

namespace pb2025_sentry_behavior
{

BT::NodeStatus SetCsvPhaseCompleteAction::tick()
{
  setOutput("csv_phase_complete", true);
  return BT::NodeStatus::SUCCESS;
}

BT::PortsList SetCsvPhaseCompleteAction::providedPorts()
{
  return {
    BT::OutputPort<bool>(
      "csv_phase_complete", "{decision_rmuc_csv_phase_complete}", "Set CSV phase complete")};
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<pb2025_sentry_behavior::SetCsvPhaseCompleteAction>(
    "SetCsvPhaseComplete");
}
