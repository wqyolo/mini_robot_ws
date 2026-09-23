#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class RobotNode : public rclcpp::Node
{
public:
    RobotNode()
        : Node("robot_node")
    {
        RCLCPP_INFO(
            this->get_logger(),
            "Mini Robot started!");

        subscriber_ =
            this->create_subscription<geometry_msgs::msg::Twist>(
                "/cmd_vel",
                10,
                std::bind(
                    &RobotNode::cmd_vel_callback,
                    this,
                    std::placeholders::_1));

        timer_ =
            this->create_wall_timer(
                500ms,
                std::bind(
                    &RobotNode::timer_callback,
                    this));
    }

private:
    void cmd_vel_callback(
        const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        velocity_ = msg->linear.x;

        RCLCPP_INFO(
            this->get_logger(),
            "Received velocity: %.2f m/s",
            velocity_);
    }

    void timer_callback()
    {
        double dt = 0.5;

        x_ = x_ + velocity_ * dt;

        RCLCPP_INFO(
            this->get_logger(),
            "Robot position: x = %.2f m",
            x_);
    }

    double x_ = 0.0;

    double velocity_ = 0.0;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
        subscriber_;

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobotNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}