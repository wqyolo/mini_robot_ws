#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class ControlNode : public rclcpp::Node
{
public:
    ControlNode()
        : Node("control_node")
    {
        publisher_ =
            this->create_publisher<geometry_msgs::msg::Twist>(
                "/cmd_vel",
                10);

        timer_ =
            this->create_wall_timer(
                500ms,
                std::bind(&ControlNode::timer_callback, this));

        RCLCPP_INFO(
            this->get_logger(),
            "Control Node started!");
    }

private:
    void timer_callback()
    {
        geometry_msgs::msg::Twist cmd;

        cmd.linear.x = 0.2;
        cmd.angular.z = 0.0;

        publisher_->publish(cmd);

        RCLCPP_INFO(
            this->get_logger(),
            "Send velocity: %.2f m/s",
            cmd.linear.x);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;

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