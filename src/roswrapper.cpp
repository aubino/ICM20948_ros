#include "ICM20948.h"
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/MagneticField.h>
#include <tf2/LinearMath/Quaternion.h>
#include <string>
#include <boost/program_options.hpp>
#define PI 3.14159265359
#define G 9.81

namespace po = boost::program_options ; 

int main(int argc, char* argv[])
{
	po::options_description desc("Mandatory options");
	double frequency = 0 ; 
	desc.add_options ()
    ("help,h","This node will launch and publish data froim the ICM20948 from i2c port.")
    ("frequency,f",boost::program_options::value<double>(),"Acquisition frequency of the the data . Mandatory option") ; 
	po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if(vm.count("help,h") || !vm.count("frequency"))
    {
        std::cout<<desc ;
        return 0 ;
    }
    else
    {
        frequency = vm["frequency"].as<double>() ; 
    }
	IMU_EN_SENSOR_TYPE enMotionSensorType;
	IMU_ST_ANGLES_DATA stAngles;
	IMU_ST_SENSOR_DATA stGyroRawData;
	IMU_ST_SENSOR_DATA stAccelRawData;
	IMU_ST_SENSOR_DATA stMagnRawData;
	ros::init(argc, argv, "icm20948_node");
	ros::NodeHandle nh;
	std::string ns = nh.getNamespace();
	ros::Publisher imu_data_pub = nh.advertise<sensor_msgs::Imu>(ns+"/imu/data_raw", 100);
	ros::Publisher mag_data_pub = nh.advertise<sensor_msgs::MagneticField>(ns+"/imu/mag", 100);
	sensor_msgs::Imu data;
	sensor_msgs::MagneticField mag;
	data.header.frame_id="icm20948_frame";
	mag.header.frame_id="icm20948_frame";

	imuInit(&enMotionSensorType);
	if(IMU_EN_SENSOR_TYPE_ICM20948 == enMotionSensorType)
	{
		printf("Motion sersor is ICM-20948\n" );
		printf("\r\n Sending imu infos to topic : /imu/data_raw ");
		printf("\r\n Sending magnetometer infos to topic : /imu/mag");
		printf("\r\n /-------------------------------------------------------------/ \r\n");
	}
	else
	{
		printf("Motion sersor NULL\n");
	}

	while(ros::ok())
	{
		imuDataGet( &stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);
		tf2::Quaternion q; 
		q.setRPY(stAngles.fRoll*PI/180,stAngles.fPitch*PI/180,stAngles.fYaw*PI/180);
		data.orientation.x = q.getX() ;
		data.orientation.y = q.getY() ; 
		data.orientation.z = q.getZ() ;
		data.orientation.w = q.getZ() ; 
		data.angular_velocity.x = stGyroRawData.fX*PI/180;
		data.angular_velocity.y = stGyroRawData.fY*PI/180;
		data.angular_velocity.z = stGyroRawData.fZ*PI/180;
		data.linear_acceleration.x = stAccelRawData.fX*G;
		data.linear_acceleration.y = stAccelRawData.fY*G;
		data.linear_acceleration.z = stAccelRawData.fZ*G;
		mag.magnetic_field.x=stMagnRawData.fX;
		mag.magnetic_field.y=stMagnRawData.fY;
		mag.magnetic_field.z=stMagnRawData.fZ;
		data.header.stamp=ros::Time::now();
		imu_data_pub.publish(data);
		mag.header.stamp=ros::Time::now();
		mag_data_pub.publish(mag);
		double period = 1/frequency ; 
		usleep(period*1.e06);
	}
	return 0;
}
