#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <std_msgs/Int32.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/tf.h>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "waypoint_saver.h"


enum Input_Mode {
  START_INPUT = 0,
  END_INPUT = 1,
  SAVE_STANBY = 2,
};

// グローバルメンバー宣言
std::vector<Waypoint_data> waypoint_handler;
int node_index = 0;
Waypoint_data wp_buff;
enum Input_Mode mode = START_INPUT;   //初期モードはノード番号の入力
std::string node_num;
bool is_pose_come = false;
// ここまで

/**
 * @fn 
 * @brief キーボード入力とモードに応じて動作を変更
 * @param data: key_handlerからのキー入力
 * @return None
 * @detail 
 */
void KeyCallback(std_msgs::Int32 data) {
  int key = data.data;
  switch(mode) {
    case START_INPUT: {
      if(key >= '0' && key <= '9') {
        node_num += char(key);
        std::cout << std::endl <<"Start node num = " << node_num << std::endl;
      }
      else if(key == 'c') {
        ROS_INFO("Reset buffer. Please start from start node.");
        waypoint_handler[node_index].Reset();
        waypoint_handler[node_index].Initialize();
        mode = START_INPUT;
      }
      else if(key == 0x0a && node_num != "") {
        int check = std::stoi(node_num);
        if(check < 0 || check > 150){
          node_num = "";
          ROS_INFO("Node number is out of selection.");
          ROS_INFO("Clear input buffer.");
          ROS_INFO("Please type again.");
        }
        else {
          waypoint_handler[node_index].set_start_node(check);
          node_num = "";
          mode = END_INPUT;
          ROS_INFO("Start node num has been set to %d.",check);
          ROS_INFO("Please type next node number.");
        }
      }
      else if(key == 'b'){
        ROS_INFO("TRY");
        if(waypoint_handler[node_index].get_PointState()){
          ROS_INFO("Pop the latest waypoint.");
          waypoint_handler[node_index].pop_Point();
          waypoint_handler[node_index].pop_PoseArray();
          waypoint_handler[node_index].pop_PoseArrayBack();
        }
      }
      break;
    }
    case END_INPUT: {
      if(key >= '0' && key <= '9') {
        node_num += char(key);
        std::cout << std::endl <<"End node num = " << node_num << std::endl;
      }
      else if(key == 'c') {
        ROS_INFO("Reset buffer. Please start from start node.");
        waypoint_handler[node_index].Reset();
        waypoint_handler[node_index].Initialize();
        mode = START_INPUT;
      }
      else if(key == 0x0a && node_num != "") {
        int check = std::stoi(node_num);
        if(check < 0 || check > 150){
          node_num = "";
          ROS_INFO("Node number is out of selection.");
          ROS_INFO("Clear input buffer.");
          ROS_INFO("Please type again.");
        }
        else {
          waypoint_handler[node_index].set_end_node(check);
          node_num = "";
          mode = SAVE_STANBY;
          ROS_INFO("End node num has been set to %d.",check);
          ROS_INFO("Please save waypoint with hitting 's' after send waypoint.");
        }
      }
      else if(key == 'b'){
        ROS_INFO("TRY");
        if(waypoint_handler[node_index].get_PointState()){
          ROS_INFO("Pop the latest waypoint.");
          waypoint_handler[node_index].pop_Point();
          waypoint_handler[node_index].pop_PoseArray();
          waypoint_handler[node_index].pop_PoseArrayBack();
        }
      }
      break;
    }
    case SAVE_STANBY: {
      ROS_INFO("Ready to save waypoints");
      if(key == 's' && is_pose_come){
        ROS_INFO("Save waypoint file");
        waypoint_handler[node_index].save_waypoint();
        // std::cout << "Save as " << waypoint_handler[node_index].get_filename() << std::endl;
        node_index++;
        waypoint_handler.push_back(wp_buff);
        mode = START_INPUT;
        is_pose_come = false;
      }
      else if(key == 'c') {
        ROS_INFO("Reset buffer. Please start from start node.");
        waypoint_handler[node_index].Reset();
        waypoint_handler[node_index].Initialize();
        mode = START_INPUT;
      }
      else if(key == 'b'){
        ROS_INFO("TRY");
        if(waypoint_handler[node_index].get_PointState()){
          ROS_INFO("Pop the latest waypoint.");
          waypoint_handler[node_index].pop_Point();
          waypoint_handler[node_index].pop_PoseArray();
          waypoint_handler[node_index].pop_PoseArrayBack();
        }
      }
      else {
        ROS_INFO("[c] : To reset all buffer.");
        ROS_INFO("[s] : To save the waypoint.");
        ROS_INFO("[b] : To pop the latest waypoint.");
      }
      break;
    }
  }
  ROS_INFO("[c] : To reset all buffer.");
  ROS_INFO("[s] : To save the waypoint.");
  ROS_INFO("[b] : To pop the latest waypoint.");
}

/**
 * @fn 
 * @brief Rvizから座標と姿勢を受け取る
 * @param wp: waypoint情報
 * @return None
 * @detail 
 * Waypoint_dataクラスの対象のオブジェクトのメンバ変数に格納していく
 */
void PoseCallback(geometry_msgs::PoseStamped wp) {
  std::cout << "x = " << wp.pose.position.x << "y = " << wp.pose.position.y << "z = " << wp.pose.position.z;
  std::cout << "qx = " << wp.pose.orientation.x <<  " qy = " << wp.pose.orientation.y <<  " qz = " << wp.pose.orientation.z << " qw = " << wp.pose.orientation.w << std::endl;  

  waypoint_handler[node_index].push_Point(wp);
  waypoint_handler[node_index].push_PoseArray(wp.pose);
  waypoint_handler[node_index].push_PoseArrayBack(wp.pose);
  is_pose_come = true;
  waypoint_handler[node_index].set_PointState(true);
}

/**
 * @fn 
 * @brief メイン関数 
 * @param 
 * @return None
 * @detail 
 */
int main(int argc, char **argv) {
  ros::init(argc, argv,"waypoint_saver_node");
  ros::NodeHandle handle;
  ros::Subscriber sub = handle.subscribe("/move_base_simple/goal", 100, PoseCallback);
  ros::Subscriber key_sub = handle.subscribe("key_input", 100, KeyCallback);
  ros::Publisher pose_pub = handle.advertise<geometry_msgs::PoseArray>("joined_waypoint", 10);
  ros::Publisher pose_back_pub = handle.advertise<geometry_msgs::PoseArray>("joined_waypoint_back", 10);
  ros::Rate rate(30);
  ROS_INFO("Waypoint_saver initialized.");

  // 最初のウェイポイント管理領域確保
  wp_buff.Initialize();
  waypoint_handler.push_back(wp_buff);

  while(ros::ok()) { 
    // key = getch();
    // if(key != -1) {
    //   KeyCallback(key);
    // }
    pose_pub.publish(waypoint_handler[node_index].get_PoseArray());
    pose_back_pub.publish(waypoint_handler[node_index].get_PoseArrayBack());
    ros::spinOnce();
    rate.sleep();
  }
}

