#include "ros/ros.h"
#include "robot_msgs/omoapproach.h"
#include "geometry_msgs/Twist.h"
#include "fiducial_msgs/FiducialTransformArray.h"

using namespace std;
using namespace ros;

class omo_approach
{
    private:

        ros::NodeHandle n;
        ros::ServiceServer srv1;
        ros::Subscriber sub1;
        ros::Publisher pub1;

        robot_msgs::omoapproach Omo_approach;
        geometry_msgs::Twist Omo_x_vel;

        float z_dist;
        float z_tol = 0.25;
        float lin_vel = 0.1;
        bool omo_enabler = false;
        

    public:

        


        omo_approach()
        {
            srv1 = n.advertiseService("/omo_approach", &omo_approach::is_omo_dist, this);
            sub1 = n.subscribe("/fiducial_transforms", 1, &omo_approach::omo_z_position_fun, this);
            pub1 = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
        }

        bool is_omo_dist(robot_msgs::omoapproach::Request &req,
                         robot_msgs::omoapproach::Response &res)
        {
            omo_enabler = true;
            if(omo_enabler == false)
            {
                res.is_z_reached = true;
                return true;
            }
            
            
        }

        

        void omo_z_position_fun(const fiducial_msgs::FiducialTransformArray &msg)
        {
            for(int i=0; i<msg.transforms.size(); i++)
            {
                z_dist = msg.transforms[i].transform.translation.z;

                ROS_INFO("z_dist = %f", z_dist);
                Omo_x_vel.linear.x = lin_vel;
                if(omo_enabler)
                pub1.publish(Omo_x_vel);
                ROS_INFO("publishing v = 0.1 m/s");
                ROS_INFO("curr pos = %f", z_dist);
                ROS_INFO("goal pos = %f", z_tol);

                if(z_dist < z_tol)
                {
                    Omo_x_vel.linear.x = 0;
                    pub1.publish(Omo_x_vel);
                    ROS_INFO("position reached = %f", z_tol);
                    omo_enabler = false;
                    break;
                }
            }
        }


};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "omo_approach");
    omo_approach OA_obj;

    ros::spin();
    return 0;
}