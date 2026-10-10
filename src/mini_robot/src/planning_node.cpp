
#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

class PlanningNode : public rclcpp::Node
{
public:
    PlanningNode() : Node("planning_node")
    {
        // 订阅定位结果
        subscription_ =
            this->create_subscription<std_msgs::msg::Float64>(
                "/estimated_position",
                10,
                std::bind(
                    &PlanningNode::position_callback,
                    this,
                    std::placeholders::_1));

        // 发布规划速度
        publisher_ =
            this->create_publisher<geometry_msgs::msg::Twist>(
                "/planned_cmd_vel",
                10);

        // 创建设置目标的 Service Server
        goal_service_ =
            this->create_service<example_interfaces::srv::AddTwoInts>(
                "/set_goal",
                std::bind(
                    &PlanningNode::set_goal_callback,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

        RCLCPP_INFO(
            this->get_logger(),
            "Planning Node started! Goal = %.2f m",
            goal_x_);
    }

private:
    // Service 回调
    void set_goal_callback(
        const std::shared_ptr<
            example_interfaces::srv::AddTwoInts::Request> request,
        std::shared_ptr<
            example_interfaces::srv::AddTwoInts::Response> response)
    {
        // a 表示目标位置，单位厘米
        goal_x_ = static_cast<double>(request->a) / 100.0;

        // 返回目标厘米值
        response->sum = request->a;

        RCLCPP_INFO(
            this->get_logger(),
            "New goal received: %.2f m",
            goal_x_);
    }

    // 定位消息回调
    void position_callback(
        const std_msgs::msg::Float64::SharedPtr msg)
    {
        double current_x = msg->data;

        double error = goal_x_ - current_x;

        double desired_speed = 0.0;

        if (std::abs(error) < tolerance_)
        {
            desired_speed = 0.0;
        }
        else
        {
            desired_speed = std::clamp(
                kp_ * error,
                -max_speed_,
                max_speed_);
        }

        geometry_msgs::msg::Twist cmd;
        cmd.linear.x = desired_speed;
        cmd.angular.z = 0.0;

        publisher_->publish(cmd);

        RCLCPP_INFO(
            this->get_logger(),
            "Current: %.2f | Goal: %.2f | Speed: %.2f",
            current_x,
            goal_x_,
            desired_speed);
    }

    double goal_x_ = 10.0;
    double kp_ = 0.5;
    double max_speed_ = 0.5;
    double tolerance_ = 0.1;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr
        subscription_;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr
        publisher_;

    rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr
        goal_service_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<PlanningNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
