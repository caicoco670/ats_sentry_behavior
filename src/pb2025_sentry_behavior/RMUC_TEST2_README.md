# RMUC 2026 Test2 行为树 — 使用说明

## 概述

`rmuc_test2.xml` 是 RMUC 2026 哨兵行为树变体，与 `rmuc_test.xml` 并行存在。核心区别：

- **简化决策**：仅保留 P0 血量回补 + 己方半区 CSV 巡逻
- **多点巡逻**：使用 9 个路径点在己方半区做持续循环巡逻
- **比赛状态检测**：沿用 `rmul_2026.xml` 的 `IsHpBand` / `IsGameStatus` 模式

根行为树根据 `input_source` 分流：

- **`referee`**（主路径）→ `decision_rmuc_test2_referee`，使用裁判系统数据
- **`simulation`**（兜底分支）→ `decision_simulation`，沿用 `rmul_2026.xml` 的 Patrol/Anchor 逻辑

## 决策优先级

| 优先级 | 触发条件 | 行为 |
|--------|---------|------|
| **P0** | HP == critical（危急） | 返回补给点回血（`decision_safe_point`，复用 rmul_2026） |
| **默认** | 以上均不满足 | 己方半区 CSV 多点巡逻（`decision_rmuc_test2_patrol`） |

`ReactiveFallback` 保证 P0 随时可以打断巡逻，血量恢复后自动切回巡逻。

## 涉及文件

### 新建

```
behavior_trees/rmuc_test2.xml              # 行为树 XML
params/rmuc_test2_patrol.csv              # 己方半区 9 个巡逻路径点
```

### 修改

```
behavior_trees/Project.btproj             # 新增 <include path="rmuc_test2.xml"/>
src/pb2025_sentry_behavior_server.cpp     # 新增 decision.rmuc_test2.patrol_csv_file 参数
params/sentry_behavior_vision_test.yaml   # 新增 rmuc_test2 配置段
```

## CSV 路径点格式

`params/rmuc_test2_patrol.csv`，格式 `x, y, z`，`#` 开头行为注释：

```
# RMUC test2 — friendly half patrol waypoints
# Format: x, y, z
0.000000,0.000000,0.000000
2.261520,0.805560,0.000000
4.644250,1.979320,0.000000
...
```

共 9 个路径点，在己方半区形成巡逻回路。

## 构建

```bash
cd /home/tchaikovsky/Desktop/ATS_2026_snetry_test
source /opt/ros/humble/setup.bash
source install/setup.bash

colcon build --symlink-install --packages-select pb2025_sentry_behavior \
  --cmake-args -DCMAKE_BUILD_TYPE=Release
```

## 运行

### 切换到 rmuc_test2 行为树

运行时动态切换（无需重启）：

```bash
ros2 param set /pb2025_sentry_behavior_client target_tree rmuc_test2
```

切回 `rmuc_test`：

```bash
ros2 param set /pb2025_sentry_behavior_client target_tree rmuc_test
```

### Loopback 离线仿真

```bash
source install/setup.bash

ros2 launch pb2025_sentry_bringup loopback_decision_sim.launch.py \
  map:=src/loopback_sim/maps/rmuc_2026_test.yaml \
  publish_referee_inputs:=True \
  use_rviz:=True
```

启动后切换行为树到 `rmuc_test2`。

### 真车 / 比赛模式

```bash
ros2 launch pb2025_sentry_behavior pb2025_sentry_behavior_launch.py \
  params_file:=src/pb2025_sentry_behavior/params/sentry_behavior.yaml
```

> 比赛时需要 YAML 中 `input_source: referee` 且 `target_tree: rmuc_test2`。

## 动态调参

```bash
# 修改危急血量阈值（依赖 IsHpBand 的 HP 分级配置）
ros2 param set /pb2025_sentry_behavior_server decision.hp_thresholds.normal.low 200

# 修改自旋速度
ros2 param set /pb2025_sentry_behavior_server decision.motion.default_spin_speed 5.0
```

## 更新巡逻路径点

1. 编辑 `params/rmuc_test2_patrol.csv`，按行写入新坐标 `x, y, z`
2. （可选）用 `snap_waypoints.py` 吸附到空闲空间
3. 重新构建或直接替换已安装的 CSV 文件

## 和 rmuc_test 的区别

| | rmuc_test | rmuc_test2 |
|------|-----------|------------|
| 决策模式 | P0-P3 四级优先级（含撤退/末段/CSV 阶段 / 前哨站检测） | P0 + 默认巡逻，两级简化 |
| 巡逻方式 | 单次 CSV 导航 → 两巡逻点 | 9 点 CSV 持续循环巡逻 |
| 血量检测 | `IsHpLessThan`（固定阈值） | `IsHpBand`（动态分级，随比赛阶段收紧） |
| 比赛开始 | `check_game_start`（复用 rmul_2026） | 同 |
| 仿真兜底 | `decision_simulation`（复用 rmul_2026） | 同 |
