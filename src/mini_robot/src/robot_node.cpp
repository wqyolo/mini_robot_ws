#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float64.hpp"

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

        // 记录初始时间
        last_cmd_time_ = this->now();

        // 订阅速度指令
        subscriber_ =
            this->create_subscription<geometry_msgs::msg::Twist>(
                "/cmd_vel",
                10,
                std::bind(
                    &RobotNode::cmd_vel_callback,
                    this,
                    std::placeholders::_1));
        position_publisher_ =
            this->create_publisher<std_msgs::msg::Float64>(
                "/robot_position",
                10);

        // 每 500ms 更新一次机器人状态
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
        // 保存最新速度
        velocity_ = msg->linear.x;

        // 记录最后一次收到 cmd_vel 的时间
        last_cmd_time_ = this->now();

        RCLCPP_INFO(
            this->get_logger(),
            "Received velocity: %.2f m/s",
            velocity_);
    }

    void timer_callback()
    {
        rclcpp::Time current_time = this->now();

        double timeout =
            (current_time - last_cmd_time_).seconds();

        if (timeout > 1.0)
        {
            if (velocity_ != 0.0)
            {
                RCLCPP_WARN(
                    this->get_logger(),
                    "cmd_vel timeout! Stop robot.");
            }

            velocity_ = 0.0;
        }

        double dt = 0.5;

        x_ = x_ + velocity_ * dt;

        RCLCPP_INFO(
            this->get_logger(),
            "Robot position: x = %.2f m, velocity = %.2f m/s",
            x_,
            velocity_);

        // 创建位置 Message
        std_msgs::msg::Float64 position_msg;

        // 把机器人的真实位置放进 Message
        position_msg.data = x_;

        // 发布
        position_publisher_->publish(position_msg);
    }
    double x_ = 0.0;

    double velocity_ = 0.0;

    rclcpp::Time last_cmd_time_;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
        subscriber_;

    rclcpp::TimerBase::SharedPtr timer_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr
        position_publisher_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobotNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}