// Copyright (c) 2021, PickNik, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
/// \author: Andy Zelenak

#pragma once

#include "eigen3/Eigen/Core"

#include "admittance_controller/ik_plugin_base.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "moveit/robot_model_loader/robot_model_loader.h"
#include "moveit/robot_state/robot_state.h"
#include "rclcpp/rclcpp.hpp"

namespace moveit_differential_ik_plugin
{

class MoveItKinematics : public admittance_controller::IKBaseClass
{
public:
  MoveItKinematics();

  /**
   * \brief Create an object which takes Cartesian delta-x and converts to joint delta-theta.
   * It uses the Jacobian from MoveIt.
   */
  bool initialize(const std::shared_ptr<rclcpp::Node> & node, const std::string & group_name);

  /**
   * \brief Convert Cartesian delta-x to joint delta-theta, using the Jacobian.
   * \param delta_x_vec input Cartesian deltas (x, y, z, rx, ry, rz)
   * \param control_frame_to_ik_base transform the requested delta_x to MoveIt's ik_base frame
   * \param delta_theta_vec output vector with joint states
   * \return true if successful
   */
  bool
  convert_cartesian_deltas_to_joint_deltas(
    std::vector<double> & delta_x_vec,
    const geometry_msgs::msg::TransformStamped & control_frame_to_ik_base,
    std::vector<double> & delta_theta_vec);

  /**
   * \brief Convert joint delta-theta to Cartesian delta-x, using the Jacobian.
   * \param[in] delta_theta_vec vector with joint states
   * \param[in] tf_ik_base_to_desired_cartesian_frame transformation to the desired Cartesian frame. Use identity matrix to stay in the ik_base frame.
   * \param[out] delta_x_vec  Cartesian deltas (x, y, z, rx, ry, rz)
   * \return true if successful
   */
  bool
  convert_joint_deltas_to_cartesian_deltas(
    std::vector<double> &  delta_theta_vec,
    const geometry_msgs::msg::TransformStamped & tf_ik_base_to_desired_cartesian_frame,
    std::vector<double> & delta_x_vec);

  bool update_robot_state(const trajectory_msgs::msg::JointTrajectoryPoint & current_joint_state)
  {
    if (current_joint_state.positions.size() != joint_model_group_->getVariableNames().size())
    {
      RCLCPP_ERROR(node_->get_logger(), "Vector size mismatch in update_robot_state()");
      return false;
    }

    kinematic_state_->setJointGroupPositions(joint_model_group_, current_joint_state.positions);
    return true;
  }

private:
  /** \brief Possibly calculate a velocity scaling factor, due to proximity of
   * singularity and direction of motion
   */
  double velocity_scaling_factor_for_singularity(const Eigen::VectorXd& commanded_velocity,
                                             const Eigen::JacobiSVD<Eigen::MatrixXd>& svd,
                                             const Eigen::MatrixXd& pseudo_inverse);

  // MoveIt setup
  const moveit::core::JointModelGroup* joint_model_group_;
  moveit::core::RobotStatePtr kinematic_state_;
  std::shared_ptr<rclcpp::Node> node_;

  // Pre-allocate for speed
  Eigen::MatrixXd jacobian_;
  Eigen::MatrixXd matrix_s_;
  Eigen::MatrixXd pseudo_inverse_;
};

}  // namespace moveit_differential_ik_plugin
