
#include <deque>
#include <numeric>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

class LocalizationNode : public rclcpp::Node
{
public:
    LocalizationNode()
        : Node("localization_node")
    {
        // 订阅传感器测量值
        subscription_ =
            this->create_subscription<std_msgs::msg::Float64>(
                "/sensor_position",
                10,
                std::bind(
                    &LocalizationNode::sensor_callback,
                    this,
                    std::placeholders::_1));

        // 发布估计位置
        publisher_ =
            this->create_publisher<std_msgs::msg::Float64>(
                "/estimated_position",
                10);

        RCLCPP_INFO(
            this->get_logger(),
            "Localization Node started!");
    }

private:
    void sensor_callback(
        const std_msgs::msg::Float64::SharedPtr msg)
    {
        // 1. 读取传感器测量值
        double measurement = msg->data;

        // 2. 保存最新测量值
        measurements_.push_back(measurement);

        // 3. 最多保留最近5次测量
        if (measurements_.size() > 5)
        {
            measurements_.pop_front();
        }

        // 4. 计算平均值
        double sum = std::accumulate(
            measurements_.begin(),
            measurements_.end(),
            0.0);

        double estimated_position =
            sum / measurements_.size();

        // 5. 发布估计位置
        std_msgs::msg::Float64 estimate_msg;
        estimate_msg.data = estimated_position;

        publisher_->publish(estimate_msg);

        // 6. 打印结果
        RCLCPP_INFO(
            this->get_logger(),
            "Measured: %.3f | Estimated: %.3f",
            measurement,
            estimated_position);
    }

    // 保存最近5次测量值
    std::deque<double> measurements_;

    // 订阅器
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr
        subscription_;

    // 发布器
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr
        publisher_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<LocalizationNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
