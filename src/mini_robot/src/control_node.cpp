
#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class ControlNode : public rclcpp::Node
{
public:
    ControlNode()
        : Node("control_node")
    {
        // 接收规划速度
        subscription_ =
            this->create_subscription<geometry_msgs::msg::Twist>(
                "/planned_cmd_vel",
                10,
                std::bind(
                    &ControlNode::planning_callback,
                    this,
                    std::placeholders::_1));

        // 向机器人发布速度
        publisher_ =
            this->create_publisher<geometry_msgs::msg::Twist>(
                "/cmd_vel",
                10);

        last_plan_time_ = this->now();

        // 每200ms发送一次控制指令
        timer_ = this->create_wall_timer(
            200ms,
            std::bind(&ControlNode::timer_callback, this));

        RCLCPP_INFO(
            this->get_logger(),
            "Control Node started!");
    }

private:
    void planning_callback(
        const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        // 保存最新的规划速度
        desired_speed_ = msg->linear.x;

        // 更新最后一次收到规划消息的时间
        last_plan_time_ = this->now();
    }

    void timer_callback()
    {
        double elapsed =
            (this->now() - last_plan_time_).seconds();

        geometry_msgs::msg::Twist cmd;

        // 如果规划结果超时，发送零速度
        if (elapsed > 1.0)
        {
            cmd.linear.x = 0.0;
        }
        else
        {
            cmd.linear.x = desired_speed_;
        }

        cmd.angular.z = 0.0;

        publisher_->publish(cmd);
    }

    double desired_speed_ = 0.0;

    rclcpp::Time last_plan_time_;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
        subscription_;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr
        publisher_;

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<ControlNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
