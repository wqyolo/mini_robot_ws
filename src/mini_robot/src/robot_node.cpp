#include <chrono>

#include "rclcpp/rclcpp.hpp"

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

        timer_ = this->create_wall_timer(
            1s,
            std::bind(&RobotNode::timer_callback, this));
    }

private:
    void timer_callback()
    {
        x_ = x_ + 0.1;

        RCLCPP_INFO(
            this->get_logger(),
            "Robot position: x = %.1f",
            x_);
    }

    double x_ = 0.0;

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