/**
 * @file waypoint_saver.h
 * @brief Waypointを保存・管理するクラス
 * @author Yosuke Tashiro
 * @date 2018/06/07
 * @detail
 */

#include <geometry_msgs/PoseStamped.h>
#include <vector>

class Waypoint_data{
  public:
    void set_start_node(int value){ start_node_ = value; }
    void set_end_node(int value){ end_node_ = value; }
    void push_point(geometry_msgs::PoseStamped point);
    void save_waypoint();
    std::string get_filename(){ return fname_; }
    void set_filename(std::string value){ fname_=value; }
  
  private:
    int start_node_;
    int end_node_;
    std::vector<geometry_msgs::PoseStamped> waypoints_;
    std::string fname_;
};
