// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved

#pragma once
#include <iostream>
#include <functional>
#include <iomanip>
#include <map>
#include <atomic>
#include <thread>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <ros/package.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/OccupancyGrid.h>
#include <sensor_msgs/Imu.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <ros/time.h>
#include <std_msgs/String.h>
#include <rs_sdk.h>
#include <librealsense/slam/slam.h>
#include <sensor_msgs/CameraInfo.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Eigen/Eigen>
#include "ParamTypes.h"
#include "2DMapCommon.h"
#include <opencv2/core/version.hpp>
#include <ros/ros.h>
#include <librealsense/slam/slam.h>
#include <rs_core.h>
#include <rs_utils.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <realsense_ros_camera/Extrinsics.h>
#include <realsense_ros_camera/IMUInfo.h>

namespace realsense_ros_slam
{
class SubscribeTopics
{
public:
    SubscribeTopics();
    ~SubscribeTopics();
    ros::NodeHandle l_nh;
    rs::slam::slam* l_slam;
    std::mutex mut_depth, mut_fisheye, mut_gyro_imu, mut_accel_imu;
    std::vector<void *> data_list;
    ros::Subscriber l_motion_gyro_sub, l_motion_accel_sub, l_fisheye_sub, l_depth_sub;
    void subscribeStreamMessages();
    void subscribeMotion();
    void getStreamSample(const sensor_msgs::ImageConstPtr &imageMsg, rs::core::stream_type stream);
    void depthMessageCallback(const sensor_msgs::ImageConstPtr &depthImageMsg);
    void fisheyeMessageCallback(const sensor_msgs::ImageConstPtr &fisheyeImageMsg);
    void motion_gyroCallback(const sensor_msgs::ImuConstPtr &imuMsg);
    void motion_accelCallback(const sensor_msgs::ImuConstPtr &imuMsg);
    void getMotionSample(const sensor_msgs::ImuConstPtr &imuMsg, rs::core::motion_type motionType);
    void onInit(ros::NodeHandle & nh, rs::slam::slam * slam);
};

class SNodeletSlam:public nodelet::Nodelet
{
public:
    SNodeletSlam();
    ~SNodeletSlam();
    ros::NodeHandle nh;
    std::shared_ptr< message_filters::Subscriber< sensor_msgs::CameraInfo > > sub_depthInfo, sub_fisheyeInfo;
    std::shared_ptr< message_filters::Subscriber< realsense_ros_camera::IMUInfo > > sub_accelInfo, sub_gyroInfo;
    std::shared_ptr< message_filters::Subscriber< realsense_ros_camera::Extrinsics > > sub_fe2imu, sub_fe2depth;
    std::shared_ptr< message_filters::TimeSynchronizer< sensor_msgs::CameraInfo, sensor_msgs::CameraInfo, realsense_ros_camera::IMUInfo, realsense_ros_camera::IMUInfo, realsense_ros_camera::Extrinsics, realsense_ros_camera::Extrinsics> > info_TimeSynchronizer;
    rs::core::video_module_interface::supported_module_config supported_config;
    rs::core::video_module_interface::actual_module_config actual_config;
    SubscribeTopics sub;

    void onInit()override;
private:
    void cameraInfoCallback(const sensor_msgs::CameraInfoConstPtr& depthCameraInfo,const sensor_msgs::CameraInfoConstPtr& fisheyeCameraInfo, const realsense_ros_camera::IMUInfoConstPtr& accelInfo, const realsense_ros_camera::IMUInfoConstPtr& gyroInfo, const realsense_ros_camera::ExtrinsicsConstPtr& fe2imu, const realsense_ros_camera::ExtrinsicsConstPtr& fe2depth);
    void setCalibrationData(const sensor_msgs::CameraInfoConstPtr & cameraInfoMsg, rs::core::intrinsics & cameraInfo);
    void setStreamConfigIntrin(rs::core::stream_type stream, std::map< rs::core::stream_type, rs::core::intrinsics > intrinsics);
    void setMotionData(realsense_ros_camera::IMUInfo& imu_res, rs::core::motion_device_intrinsics& motion_intrin);
    void setMotionConfigIntrin(rs::core::motion_type motion, std::map< rs::core::motion_type, rs::core::motion_device_intrinsics > motion_intrinsics);
    void setExtrinData(realsense_ros_camera::Extrinsics& fe_res, rs::core::extrinsics& extrinsics);
};//end class
}