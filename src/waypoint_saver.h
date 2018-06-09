/**
 * @file waypoint_saver.h
 * @brief Waypointを保存・管理するクラス
 * @author Yosuke Tashiro
 * @date 2018/06/07
 * @detail
 */

#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Pose.h>
#include <vector>

class Waypoint_data{
  public:
    void Initialize();
    void Reset();
    void set_start_node(int value){ start_node_ = value; }
    void set_end_node(int value){ end_node_ = value; }
    void push_Point(geometry_msgs::PoseStamped point);
    void pop_Point(){ waypoints_.pop_back(); }
    void push_PoseArray(geometry_msgs::Pose pose); 
    void pop_PoseArray(){ wp_array_.poses.pop_back(); }
    void pop_PoseArrayBack(){ wp_array_back_.poses.pop_back(); }
    void push_PoseArrayBack(geometry_msgs::Pose pose); 
    void set_PointState(bool value) { is_point_set_ = value; }
    bool get_PointState() { return is_point_set_; }
    geometry_msgs::PoseArray get_PoseArray() { return wp_array_; }
    geometry_msgs::PoseArray get_PoseArrayBack() { return wp_array_back_; }
    void save_waypoint();
    std::string get_filename(){ return fname_; }
    void set_filename(std::string value){ fname_=value; }
    std::string point_to_text(geometry_msgs::PoseStamped p);
    std::string point_to_text_rotate(geometry_msgs::PoseStamped p);
  
  private:
    int start_node_;
    int end_node_;
    bool is_point_set_;
    geometry_msgs::PoseArray wp_array_;
    geometry_msgs::PoseArray wp_array_back_;
    std::vector<geometry_msgs::PoseStamped> waypoints_;
    std::string fname_;
};
