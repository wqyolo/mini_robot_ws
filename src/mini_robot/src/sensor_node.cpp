#include <random>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

class SensorNode : public rclcpp::Node
{
public:
    SensorNode()
        : Node("sensor_node"),
          generator_(std::random_device{}()),
          noise_distribution_(0.0, 0.05)
    {
        subscription_ =
            this->create_subscription<std_msgs::msg::Float64>(
                "/robot_position",
                10,
                std::bind(
                    &SensorNode::position_callback,
                    this,
                    std::placeholders::_1));

        publisher_ =
            this->create_publisher<std_msgs::msg::Float64>(
                "/sensor_position",
                10);

        RCLCPP_INFO(
            this->get_logger(),
            "Sensor Node started!");
    }

private:
    void position_callback(
        const std_msgs::msg::Float64::SharedPtr msg)
    {
        // Robot Node 提供的真实位置
        double true_position = msg->data;

        // 产生随机噪声
        double noise = noise_distribution_(generator_);

        // 模拟传感器测量
        double measured_position =
            true_position + noise;

        RCLCPP_INFO(
            this->get_logger(),
            "True: %.2f m | Noise: %.3f m | Measured: %.2f m",
            true_position,
            noise,
            measured_position);

        // 创建传感器测量消息
        std_msgs::msg::Float64 sensor_msg;

        sensor_msg.data = measured_position;

        // 发布传感器测量值
        publisher_->publish(sensor_msg);
    }

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr
        subscription_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr
        publisher_;

    std::mt19937 generator_;

    std::normal_distribution<double> noise_distribution_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SensorNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}