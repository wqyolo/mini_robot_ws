#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

class SensorNode : public rclcpp::Node
{
public:
    SensorNode()
        : Node("sensor_node")
    {
        subscription_ =
            this->create_subscription<std_msgs::msg::Float64>(
                "/robot_position",
                10,
                std::bind(
                    &SensorNode::position_callback,
                    this,
                    std::placeholders::_1));

        RCLCPP_INFO(
            this->get_logger(),
            "Sensor Node started!");
    }

private:
    void position_callback(
        const std_msgs::msg::Float64::SharedPtr msg)
    {
        double true_position = msg->data;

        double measured_position = true_position;

        RCLCPP_INFO(
            this->get_logger(),
            "Sensor measurement: %.2f m",
            measured_position);
    }

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr
        subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SensorNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}