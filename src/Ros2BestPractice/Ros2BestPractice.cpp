#include "Ros2BestPractice/Ros2BestPractice.hpp"

namespace ros2_cpp_pkg
{

    //! static variable initialization
    size_t Ros2BestPractice::m_number_objects{0};

    //! Constructor Initialization List
    Ros2BestPractice::Ros2BestPractice(const std::string &t_node_name)
        : Node(t_node_name),
          m_is_cloud_received{false}, m_is_image_received{false},
          m_rate{20.0}, m_rate_waiting{1.0},
          m_point_cloud{nullptr}, m_image{nullptr},
          m_timer{nullptr},
          m_publisher_topic_lidar{}, m_publisher_topic_image{},
          m_subscriber_topic_lidar{}, m_subscriber_topic_image{},
          m_frame_id{}, m_image_width{}, m_image_height{},
          m_parameter_subscriber_topic_lidar{}, m_parameter_publisher_topic_lidar{},
          m_parameter_subscriber_topic_image{}, m_parameter_publisher_topic_image{},
          m_parameter_frame_id{}, m_parameter_image_width{}, m_parameter_image_height{}

    {
        m_point_cloud = std::make_shared<sensor_msgs::msg::PointCloud2>();

        m_image = std::make_shared<sensor_msgs::msg::Image>();

        this->declare_parameter("subscriber_topic_lidar");
        this->declare_parameter("subscriber_topic_image");
        this->declare_parameter("publisher_topic_lidar");
        this->declare_parameter("publisher_topic_image");
        this->declare_parameter("frame_id");
        this->declare_parameter("image_width");
        this->declare_parameter("image_height");

        m_parameter_subscriber_topic_lidar = this->get_parameter("subscriber_topic_lidar");
        m_parameter_subscriber_topic_image = this->get_parameter("subscriber_topic_image");
        m_parameter_publisher_topic_lidar = this->get_parameter("publisher_topic_lidar");
        m_parameter_publisher_topic_image = this->get_parameter("publisher_topic_image");
        m_parameter_frame_id = this->get_parameter("frame_id");
        m_parameter_image_width = this->get_parameter("image_width");
        m_parameter_image_height = this->get_parameter("image_height");

        m_subscriber_topic_lidar = m_parameter_subscriber_topic_lidar.get_value<std::string>();
        m_subscriber_topic_image = m_parameter_subscriber_topic_image.get_value<std::string>();
        m_publisher_topic_lidar = m_parameter_publisher_topic_lidar.get_value<std::string>();
        m_publisher_topic_image = m_parameter_publisher_topic_image.get_value<std::string>();
        m_frame_id = m_parameter_frame_id.get_value<std::string>();
        m_image_width = m_parameter_image_width.get_value<int>();
        m_image_height = m_parameter_image_height.get_value<int>();

        ++m_number_objects;

        RCLCPP_INFO(this->get_logger(), "Constructor is initializated : %s", __APP_NAME__);

        if (!readParameters())
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameters", __APP_NAME__);
            rclcpp::shutdown();
            std::exit(EXIT_FAILURE);
        }

        RCLCPP_INFO(this->get_logger(), "[%s] Subscriber Topic Lidar: %s", __APP_NAME__, m_subscriber_topic_lidar.c_str());

        RCLCPP_INFO(this->get_logger(), "[%s] Publisher Topic Lidar: %s", __APP_NAME__, m_publisher_topic_lidar.c_str());

        RCLCPP_INFO(this->get_logger(), "[%s] Frame Id: %s", __APP_NAME__, m_frame_id.c_str());

        RCLCPP_INFO(this->get_logger(), "[%s] Subscriber Topic Image: %s", __APP_NAME__, m_subscriber_topic_image.c_str());

        RCLCPP_INFO(this->get_logger(), "[%s] Publisher Topic Image: %s", __APP_NAME__, m_publisher_topic_image.c_str());

        RCLCPP_INFO(this->get_logger(), "[%s Image Width: %d", __APP_NAME__, m_image_width);

        RCLCPP_INFO(this->get_logger(), "[%s Image Height: %d", __APP_NAME__, m_image_height);

        RCLCPP_INFO(this->get_logger(), "[%s] Rate: %f", __APP_NAME__, m_rate);

        RCLCPP_INFO(this->get_logger(), "[%s] Rate Waiting: %f", __APP_NAME__, m_rate_waiting);

        displayActiveObjects();

        m_subscriptions_point_cloud[m_subscriber_topic_lidar] = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            m_subscriber_topic_lidar, 10, std::bind(&Ros2BestPractice::pointCloudCallback, this, std::placeholders::_1));

        m_subscriptions_image[m_subscriber_topic_image] = this->create_subscription<sensor_msgs::msg::Image>(
            m_subscriber_topic_image, 10, std::bind(&Ros2BestPractice::imageCallback, this, std::placeholders::_1));

        m_publishers_point_cloud[m_publisher_topic_lidar] = this->create_publisher<sensor_msgs::msg::PointCloud2>(
            m_publisher_topic_lidar, 10);

        m_publishers_image[m_publisher_topic_image] = this->create_publisher<sensor_msgs::msg::Image>(
            m_publisher_topic_image, 10);

        m_timer = this->create_wall_timer(500ms, std::bind(&Ros2BestPractice::contiuousCallback, this));

        RCLCPP_INFO(this->get_logger(), "[%s] Successfully launched.", __APP_NAME__);
    }

    //! Destructor Initialization .
    Ros2BestPractice::~Ros2BestPractice()
    {
        // Dealocate Memory Allocation

        --m_number_objects;

        RCLCPP_INFO(this->get_logger(), "[%s]  Destructor is activated. ", __APP_NAME__);

        rclcpp::shutdown();
    }
    inline bool Ros2BestPractice::readParameters()
    {
        if (m_parameter_subscriber_topic_lidar != this->get_parameter("subscriber_topic_lidar"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter subscriber_topic_lidar", __APP_NAME__);
            return false;
        }
        if (m_parameter_subscriber_topic_image != this->get_parameter("subscriber_topic_image"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter subscriber_topic_image", __APP_NAME__);
            return false;
        }
        if (m_parameter_publisher_topic_image != this->get_parameter("publisher_topic_image"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter publisher_topic_image", __APP_NAME__);
            return false;
        }
        if (m_parameter_publisher_topic_lidar != this->get_parameter("publisher_topic_lidar"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter publisher_topic_lidar", __APP_NAME__);
            return false;
        }
        if (m_parameter_frame_id != this->get_parameter("frame_id"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter frame_id", __APP_NAME__);
            return false;
        }
        if (m_parameter_image_width != this->get_parameter("image_width"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter image_width", __APP_NAME__);
            return false;
        }
        if (m_parameter_image_height != this->get_parameter("image_height"))
        {
            RCLCPP_ERROR(this->get_logger(), "[%s] Failed to read parameter image_height", __APP_NAME__);
            return false;
        }

        return true;
    }

    inline bool Ros2BestPractice::areDataReceived(const bool &is_cloud_received, const bool &is_image_received)
    {
        if (is_cloud_received && is_image_received)
        {
            RCLCPP_INFO(this->get_logger(), "[%s] Received data from all subscribers.", __APP_NAME__);
            return true;
        }
        else
            RCLCPP_INFO(this->get_logger(), "[%s] Not all subscribers are ready.", __APP_NAME__);

        return false;
    }

    inline size_t Ros2BestPractice::objectCounter()
    {
        return m_number_objects;
    }

    inline void Ros2BestPractice::displayActiveObjects() const
    {
        RCLCPP_INFO(this->get_logger(), "[%s] Number of active objects: %d", __APP_NAME__, m_number_objects);
    }

    inline void Ros2BestPractice::pointCloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr t_point_cloud)
    {
        m_is_cloud_received = true;
        m_point_cloud = t_point_cloud;
    }

    inline void Ros2BestPractice::imageCallback(const sensor_msgs::msg::Image::SharedPtr t_image)
    {
        m_is_image_received = true;
        m_image = t_image;
    }

    inline void Ros2BestPractice::contiuousCallback()
    {
        if (areDataReceived(m_is_cloud_received, m_is_image_received))
        {

            RCLCPP_INFO(this->get_logger(), "[%s] //! Build Your Algorithm Here ", __APP_NAME__);

            // If Data Suddenly Stops, Reset the Flags.
            m_is_cloud_received = false;
            m_is_image_received = false;

            m_publishers_point_cloud[m_publisher_topic_lidar]->publish(*m_point_cloud);
            m_publishers_image[m_publisher_topic_image]->publish(*m_image);
        }
    }

}
