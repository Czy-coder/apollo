/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#ifndef MODULES_PLANNING_COMMON_REFERENCE_LINE_INFO_H_
#define MODULES_PLANNING_COMMON_REFERENCE_LINE_INFO_H_

#include <limits>
#include <list>
#include <string>
#include <vector>

#include "modules/common/proto/pnc_point.pb.h"
#include "modules/planning/proto/planning.pb.h"

#include "modules/map/pnc_map/pnc_map.h"
#include "modules/planning/common/path/path_data.h"
#include "modules/planning/common/path_decision.h"
#include "modules/planning/common/speed/speed_data.h"
#include "modules/planning/common/trajectory/discretized_trajectory.h"

namespace apollo {
namespace planning {

class ReferenceLineInfo {
 public:
  explicit ReferenceLineInfo(const hdmap::PncMap* pnc_map,
                             const ReferenceLine& reference_line);

  /**
   * Check if xy_point is on the left side lane of current reference line.
   */
  bool IsOnLeftLane(const common::math::Vec2d& xy_point);
  /**
   * Check if xy_point is on the right side lane of current reference line.
   */
  bool IsOnRightLane(const common::math::Vec2d& xy_point);

  bool AddObstacles(const std::vector<const Obstacle*>& obstacles);
  PathObstacle* AddObstacle(const Obstacle* obstacle);

  // FIXME(all) this interface is temp. solution to make the code work.
  // remove this interface when ready.
  PathDecision* path_decision() { return &path_decision_; }
  const PathDecision& path_decision() const { return path_decision_; }
  const ReferenceLine& reference_line() const { return reference_line_; }

  // TODO remove this inteface when ready.
  void SetTrajectory(const DiscretizedTrajectory& trajectory) {
    discretized_trajectory_ = trajectory;
  }

  const DiscretizedTrajectory& trajectory() const;

  double Cost() const { return cost_; }

  std::unique_ptr<Obstacle> CreateVirtualObstacle(
      const std::string& obstacle_id, const common::math::Vec2d& position,
      const double length, const double width, const double height) const;

  /**
   * @brief check if current reference line is started from another reference
   *line info line. The method is to check if the start point of current
   *reference line is on previous reference line info.
   * @return returns true if current reference line starts on previous reference
   *line, otherwise false.
   **/
  bool IsStartFrom(const ReferenceLineInfo& previous_reference_line_info) const;

  bool CombinePathAndSpeedProfile(const double time_resolution,
                                  const double relative_time);
  planning_internal::Debug* mutable_debug() { return &debug_; }
  const planning_internal::Debug& debug() const { return debug_; }
  LatencyStats* mutable_latency_stats() { return &latency_stats_; }
  const LatencyStats& latency_stats() const { return latency_stats_; }

  const PathData& path_data() const;
  const SpeedData& speed_data() const;
  PathData* mutable_path_data();
  SpeedData* mutable_speed_data();
  // aggregate final result together by some configuration
  bool CombinePathAndSpeedProfile(
      const double time_resolution, const double relative_time,
      DiscretizedTrajectory* discretized_trajectory);
  std::string PathSpeedDebugString() const;

 private:
  std::unique_ptr<PathObstacle> CreatePathObstacle(const Obstacle* obstacle);
  bool InitPerceptionSLBoundary(PathObstacle* path_obstacle);

  const hdmap::PncMap* pnc_map_ = nullptr;

  /**
   * @brief this is the number that measures the goodness of this reference
   * line.
   * The lower the better.
   * TODO: implement trajectory cost calculation
   */
  double cost_ = 0.0;

  const ReferenceLine reference_line_;
  PathDecision path_decision_;

  PathData path_data_;
  SpeedData speed_data_;

  DiscretizedTrajectory discretized_trajectory_;

  planning_internal::Debug debug_;
  LatencyStats latency_stats_;
};

}  // namespace planning
}  // namespace apollo

#endif  // MODULES_PLANNING_COMMON_REFERENCE_LINE_INFO_H_
