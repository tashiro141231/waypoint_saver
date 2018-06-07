#include <geometry_msgs/PointStamped.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <string>

#include "waypoint_saver.h"

void Waypoint_data::push_point(geometry_msgs::PoseStamped point){
  this->waypoints_.push_back(point);
}

std::string point_to_text(geometry_msgs::PoseStamped p) {
  std::string data;

  data += std::to_string(p.pose.position.x) + ",";
  data += std::to_string(p.pose.position.y) + ",";
  data += std::to_string(p.pose.position.z) + ",";
  data += std::to_string(p.pose.orientation.x) + ",";
  data += std::to_string(p.pose.orientation.y) + ",";
  data += std::to_string(p.pose.orientation.z) + ",";
  data += std::to_string(p.pose.orientation.w);

  return data;
}

void Waypoint_data::save_waypoint(void){
  std::string file_name1 = std::to_string(this->start_node_) + "_" + std::to_string(this->end_node_) + ".csv";
  std::string file_name2 = std::to_string(this->end_node_) + "_" + std::to_string(this->start_node_) + ".csv";
  this->set_filename(file_name1);
  
  std::ofstream out_1;
  std::ofstream out_2;

  out_1.open(file_name1);
  std::string data;
  for(auto p = waypoints_.begin(); p != waypoints_.end(); p++) {
    data = point_to_text(*p); 
    out_1 << data << std::endl;
  }
  out_1.close();
}

