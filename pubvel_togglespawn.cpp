// This program toggles between rotation and translation
// commands, based on calls to a service.
// Code was a collaboration of Javier Perez Jr. and Eric Todd
#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <stdlib.h>
#include <turtlesim/Spawn.h>
using namespace std;

bool forward = true;
bool toggleForward(
  std_srvs::Empty::Request &req,
  std_srvs::Empty::Response &resp
) {
  forward = !forward;
  ROS_INFO_STREAM("Now sending " << (forward ? 
    "forward" : "rotate") << " commands.");
  return true;
}

int main(int argc, char **argv) {

  ros::init(argc, argv, "pubvel_togglespawn" );
  ros::NodeHandle nh;
  ros::Rate rate(1);
  // Create a client object for the spawn service.  This
  // needs to know the data type of the service and its
  // name.
  ros::ServiceClient spawnClient
    = nh.serviceClient<turtlesim::Spawn>("spawn");

  // Create the request and response objects.
  turtlesim::Spawn::Request req;
  turtlesim::Spawn::Response resp;

int turts =0;
cout<<"Please enter number of Turtles: ";
cin>>turts;
cout<<"Number of Turtles = "<<turts<<"\n"; // Size needed for array
int i = 1;

ros::Publisher* tpub = NULL;   // Pointer to int, initialize to nothing.          
tpub = new ros::Publisher[turts];
// Allocate n ints and save ptr in a.

while(turts>0 && i<turts){
int n = i+1;
char name [10];
srand(time(NULL));//(time.tv_sec * 1000) + (time.tv_usec / 1000));//srand(time(NULL));
sprintf(name, "turtle%d", n);
rate.sleep();

  // Fill in the request data members.  
  req.x = rand()%11+1;
  req.y = rand()%11+1;
  req.theta = 6.47*double(rand())/double(RAND_MAX);
  req.name = name;

  // Actually call the service.  This won't return until
  // the service is complete.
  bool success = spawnClient.call(req, resp);

  // Check for success and use the response.
  if(success) {
    ROS_INFO_STREAM("Spawned a turtle named "
      << resp.name);
  } else {
    ROS_ERROR_STREAM("Failed to spawn.");
  }
  i++;
  rate.sleep();
  
}

  // Register our service with the master.
  ros::ServiceServer server = nh.advertiseService(
    "toggle_forward", &toggleForward);

  // Publish commands, using the latest value for forward,
  // until the node shuts down. 
  
  i = 0;
  while(turts>0 && i<turts){
      char name [10];
      int l = i + 1;
      //char pub [10];
      sprintf(name, "turtle%d/cmd_vel", l);   
      tpub[i] = nh.advertise<geometry_msgs::Twist>(name, 1000);    //"%s/cmd_vel", 1000, name name"/cmd_vel", 1000
     
      i++;
    }

  while(ros::ok()) {
    geometry_msgs::Twist msg;
    msg.linear.x = forward ? 1.0 : 0.0;
    msg.angular.z = forward ? 0.0 : 1.0;
    
      i = 0;
      while(turts>0 && i<turts){
        tpub[i].publish(msg);
        i++;
      }
      
    //pub.publish(msg);
    ros::spinOnce();
    rate.sleep();
  }
  delete[] tpub;
}
