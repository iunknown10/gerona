#ifndef ROBOTCONTROLLER_PBR_H
#define ROBOTCONTROLLER_PBR_H

/// THIRD PARTY
#include <geometry_msgs/PointStamped.h>

/// PROJECT
#include <path_follower/utils/parameters.h>
#include <path_follower/controller/robotcontroller.h>

#include <path_follower/utils/extended_kalman_filter.h>

class RobotController_PBR: public RobotController
{
public:
    RobotController_PBR();
    virtual void stopMotion();
    virtual void start();

    virtual void setCurrentPose(const Eigen::Vector3d&);

protected:
    virtual MoveCommandStatus computeMoveCommand(MoveCommand* cmd);
    virtual void publishMoveCommand(const MoveCommand &cmd) const;

    virtual void initialize();

    void WheelVelocities(const std_msgs::Float64MultiArray::ConstPtr& array);

private:
    void findMinDistance();

private:
    struct ControllerParameters : public RobotController::ControllerParameters
    {
        P<double> k1;
        P<double> k2;
        P<double> max_angular_velocity;

        ControllerParameters():
            RobotController::ControllerParameters("PBR"),

            k1(this, "k1", 1.0, "Factor for tuning the angular velocity command."),
            k2(this, "k2", 1.0, "Factor for tuning the angular velocity command."),
            max_angular_velocity(this, "max_angular_velocity", 0.8, "Maximum angular velocity.")
        {}
    } opt_;

    const RobotController::ControllerParameters& getParameters() const
    {
        return opt_;
    }

    struct Command
    {
        RobotController_PBR *parent_;

        //! Speed of the movement.
        float speed;
        //! Direction of movement as angle to the current robot orientation.
        float direction_angle;
        //! rotational velocity.
        float rotation;


        // initialize all values to zero
        Command(RobotController_PBR *parent):
            parent_(parent),
            speed(0.0f), direction_angle(0.0f), rotation(0.0f)
        {}

        operator MoveCommand()
        {
            MoveCommand mcmd(true);
            mcmd.setDirection(direction_angle);
            mcmd.setVelocity(speed);
            mcmd.setRotationalVelocity(rotation);
            return mcmd;
        }

        bool isValid()
        {
            if ( std::isnan(speed) || std::isinf(speed)
                 || std::isnan(direction_angle) || std::isinf(direction_angle)
                 || std::isnan(rotation) || std::isinf(rotation) )
            {
                ROS_FATAL("Non-numerical values in command: %d,%d,%d,%d,%d,%d",
                          std::isnan(speed), std::isinf(speed),
                          std::isnan(direction_angle), std::isinf(direction_angle),
                          std::isnan(rotation), std::isinf(rotation));
                // fix this instantly, to avoid further problems.
                speed = 0.0;
                direction_angle = 0.0;
                rotation = 0.0;

                return false;
            } else {
                return true;
            }
        }
    };

    Command cmd_;

    EKF ekf_;

    ros::Subscriber look_at_sub_;
    ros::Subscriber look_at_cmd_sub_;

    ros::Subscriber laser_sub_front_;
    ros::Subscriber laser_sub_back_;

    ros::Subscriber wheel_vel_sub_;

    ros::Publisher ICR_pub_;
    ros::Publisher marker_pub_;

    std::vector<float> ranges_front_;
    std::vector<float> ranges_back_;

    void reset();
    void setPath(Path::Ptr path);


    //nominal velocity
    double vn_;
    //sampling time
    double Ts_;

    //index of the orthogonal projection to the path
    uint ind_;

    //velocity of the left tread
    double Vl_;
    //velocity of the right tread
    double Vr_;

    //pose estimated by the EKF
    Eigen::Vector3d pose_ekf_;
    //ICR coordinates estimated by the EKF
    Eigen::Vector3d ICR_ekf_;
    //last time step in which the prediction was made
    ros::Time last_time_;

    //cumulative curvature sum w.r.t. path
    double curv_sum_;
    //cumulative distance to goal sum w.r.t. path
    double distance_to_goal_;
    //distance to the nearest obstacle
    double distance_to_obstacle_;

    //points estimated by the EKF
    visualization_msgs::Marker ekf_path_marker_;

    //points of the augmented path
    visualization_msgs::Marker path_aug_marker_;

    //x and y components of the augmented path
    std::vector<double> x_aug_;
    std::vector<double> y_aug_;
};

#endif // ROBOTCONTROLLER_PBR_H
