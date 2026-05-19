# RMUC 2026 测试说明

## 概述

`rmuc_test` 行为树用于 RMUC 2026 比赛，基于裁判系统串口数据做自主决策。

## 涉及文件

| 文件 | 用途 |
|------|------|
| `behavior_trees/rmuc_test.xml` | 行为树主文件 |
| `params/rmuc_waypoints.csv` | 主导航路径点 (16个) |
| `params/rmuc_patrol_waypoints.csv` | 友方半区巡逻路径点 (2个) |
| `params/sentry_behavior.yaml` | 真车参数文件 (含 `rmuc` 配置段) |
| `params/sentry_behavior_loopback.yaml` | 仿真参数文件 |

## YAML 配置

```yaml
decision:
  rmuc:
    csv_waypoints_file: "params/rmuc_waypoints.csv"      # 主导航CSV
    patrol_csv_file: "params/rmuc_patrol_waypoints.csv"  # 巡逻CSV
    retreat_hp_threshold: 150    # 撤退血量阈值
    endgame_time_threshold: 180  # 终局时间阈值(秒)
    supply_point:                # 补给点坐标
      x: -4.30
      y: -2.00
      z: 0.0
```

## 决策逻辑 (P0-P3)

`decision_rmuc_referee` 通过 `ReactiveFallback` 按优先级执行：

| 优先级 | 触发条件 | 行为 |
|--------|---------|------|
| **P0** | HP < `retreat_hp_threshold` | 返回补给点回血（最高优先级，随时打断） |
| **P1** | CSV阶段完成 且 (剩余时间 ≤ `endgame_time_threshold` 或 前哨站被推) | 友方半区巡逻 |
| **P2** | CSV阶段完成 且 不满足 P1 条件 | 原地待命 |
| **P3** | CSV阶段未完成 | 按 CSV 路径点依次导航 |

## 运行方式

### 真车模式

```bash
ros2 launch pb2025_sentry_behavior pb2025_sentry_behavior_launch.py \
  params_file:=src/pb2025_sentry_behavior/params/sentry_behavior.yaml
```

### Loopback 仿真

```bash
ros2 launch pb2025_sentry_bringup loopback_decision_sim.launch.py \
  map:=src/loopback_sim/maps/rmuc_2026_test.yaml \
  publish_referee_inputs:=True
```

## CSV 路径点格式

```csv
# 注释行以 # 开头
x, y, z
0.009700,0.006570,0.000004
0.054540,1.278440,0.000000
```

- 每行一个路径点，格式为 `x, y, z`
- `#` 开头的行为注释，空行被跳过
- `LoadCsvWaypoints` 插件在首次 tick 时解析并缓存，后续 tick 直接复用

## 更新路径点

1. 在 Groot2 中编辑坐标
2. 运行 `src/loopback_sim/test/snap_waypoints.py` 吸附到走廊中心
3. 运行 `src/loopback_sim/test/validate_rmuc_test.py` 验证合法性
