/**
 * @file waypoint_saver.h
 * @brief waypointを保存・管理するクラス
 * @author Yosuke TASHIRO
 * @date 2018/06/07
 */
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Quaternion.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <string>
#include <tf/tf.h>

#include "waypoint_saver.h"

/**
 * @fn
 * @brief 初期化 
 * @param 
 * @return None
 * @detail 初期化したい要素があったら追加していきましょう。
 */
void Waypoint_data::Initialize(){
  this->wp_array_.header.frame_id = "map";
  this->wp_array_back_.header.frame_id = "map";
  this->is_point_set_ = false;
}

/**
 * @fn
 * @brief リセット 
 * @param 
 * @return None
 * @detail
 */
void Waypoint_data::Reset(){
  this->wp_array_.header.frame_id = "map";
  this->wp_array_back_.header.frame_id = "map";
  this->start_node_ = -1;
  this->end_node_ = -1;
  this->waypoints_.clear();
  geometry_msgs::PoseArray newPoseArray;
  this->wp_array_ = newPoseArray;
  this->wp_array_back_ = newPoseArray;
}

/**
 * @fn
 * @brief メンバに保存候補になるwaypointを仮保存してい 
 * @param point:Rvizから送られてきたPoseStamped
 * @return None
 * @detail None
 */
void Waypoint_data::push_Point(geometry_msgs::PoseStamped point){
  this->waypoints_.push_back(point);
}

/**
 * @fn
 * @brief メンバに保存候補になるwaypointの可視化のためにPoseArrayにプッシュ 
 * @param point:Rvizから送られてきたPose
 * @return None
 * @detail None
 */
void Waypoint_data::push_PoseArray(geometry_msgs::Pose pose){
  this->wp_array_.poses.push_back(pose);
}

/**
 * @fn
 * @brief メンバに保存候補になるwaypointの可視化のためにPoseArrayにプッシュ 
 * @param point:Rvizから送られてきたPose
 * @return None
 * @detail None
 */
void Waypoint_data::push_PoseArrayBack(geometry_msgs::Pose pose){
  double roll, pitch, yaw;
  tf::Quaternion q(
    pose.orientation.x,
    pose.orientation.y,
    pose.orientation.z,
    pose.orientation.w);

  tf::Matrix3x3 m(q);
  m.getRPY(roll,pitch,yaw);

  pose.orientation = tf::createQuaternionMsgFromRollPitchYaw(0,0,yaw+M_PI);

  this->wp_array_back_.poses.push_back(pose);
}

/**
 * @fn
 * @brief waypointをファイル出力するためのデータ成形 
 * @param p: x,y,z,qx,qy,qz,qwをメンバに持っている
 * @return data: x \t y \t yaw 
 * @detail None
 */
std::string Waypoint_data::point_to_text(geometry_msgs::PoseStamped p) {
  std::string data;
  double roll, pitch, yaw;
  tf::Quaternion q(
    p.pose.orientation.x,
    p.pose.orientation.y,
    p.pose.orientation.z,
    p.pose.orientation.w);

  tf::Matrix3x3 m(q);
  m.getRPY(roll,pitch,yaw);

  data += std::to_string(p.pose.position.x) + "\t";
  data += std::to_string(p.pose.position.y) + "\t";
  data += std::to_string(yaw);
/*
  data += std::to_string(p.pose.orientation.x) + ",";
  data += std::to_string(p.pose.orientation.y) + ",";
  data += std::to_string(p.pose.orientation.z) + ",";
  data += std::to_string(p.pose.orientation.w);
*/
  return data;
}

/**
 * @fn
 * @brief waypointをファイル出力するためのデータ成形 
 * @param p: x,y,z,qx,qy,qz,qwをメンバに持っている
 * @return data: x \t y \t yaw 
 * @detail ノードを逆に走るために180度姿勢をヨー回転させる
 */
std::string Waypoint_data::point_to_text_rotate(geometry_msgs::PoseStamped p) {
  std::string data;
  double roll, pitch, yaw;
  tf::Quaternion q(
    p.pose.orientation.x,
    p.pose.orientation.y,
    p.pose.orientation.z,
    p.pose.orientation.w);
  geometry_msgs::Pose pose;

  tf::Matrix3x3 m(q);
  m.getRPY(roll,pitch,yaw);

  data += std::to_string(p.pose.position.x) + "\t";
  data += std::to_string(p.pose.position.y) + "\t";
  data += std::to_string(yaw + M_PI);

  pose.position.x = p.pose.position.x;
  pose.position.y = p.pose.position.y;
  pose.position.z = p.pose.position.z;
  pose.orientation = tf::createQuaternionMsgFromRollPitchYaw(0,0,yaw+M_PI);

  this->wp_array_back_.poses.push_back(pose);
/*
  data += std::to_string(p.pose.orientation.x) + ",";
  data += std::to_string(p.pose.orientation.y) + ",";
  data += std::to_string(p.pose.orientation.z) + ",";
  data += std::to_string(p.pose.orientation.w);
*/
  return data;
}

/**
 * @fn
 * @brief txtファイルとしてノード間のwaypointを保存
 * @param None
 * @return None
 * @detail オブジェクトに登録されているノードを行き来するためのwaypoint情報を保存
 * 保存形式: *1_*2.txt *2_*1.txt
 */
void Waypoint_data::save_waypoint(void){
  // 保存先とファイル名
  // std::string file_name1 = "/home/kenaf/catkin_ws/src/tsukuchalle2018/waypoint/" + std::to_string(this->start_node_) + "_" + std::to_string(this->end_node_) + ".txt";
  // std::string file_name2 = "/home/kenaf/catkin_ws/src/tsukuchalle2018/waypoint/" + std::to_string(this->end_node_) + "_" + std::to_string(this->start_node_) + ".txt";
  std::string file_name1 = "/home/tashiro-y/researches/programs/workspace/waypoint/" + std::to_string(this->start_node_) + "_" + std::to_string(this->end_node_) + ".txt";
  std::string file_name2 = "/home/tashiro-y/researches/programs/workspace/waypoint/" + std::to_string(this->end_node_) + "_" + std::to_string(this->start_node_) + ".txt";
  this->set_filename(file_name1);
  
  std::ofstream out_1;
  std::ofstream out_2;

  out_1.open(file_name1);
  std::string data;
  ROS_INFO("Start saveing first way");
  for(auto p = this->waypoints_.begin(); p != this->waypoints_.end(); p++) {
    data = point_to_text(*p); 
    out_1 << data << std::endl;
  }
  out_1.close();
  ROS_INFO("Has veen saved.");
  
  out_2.open(file_name2);
  ROS_INFO("Start saveing second way");
  int wp_num = this->waypoints_.size();
  for(wp_num; wp_num != 0; wp_num--){
    // std::cout << wp_num << std::endl;
    data = point_to_text_rotate(this->waypoints_[wp_num-1]);
    out_2 << data << std::endl;
  }
  out_2.close();
  ROS_INFO("Has veen saved.");
}

