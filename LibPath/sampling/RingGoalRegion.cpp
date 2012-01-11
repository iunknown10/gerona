/**
   (c) 2012 Karsten Bohlmann bohlmann@gmail.com 

   @author Karsten Bohlmann
   @date   1/11/2012
   @file   RingGoalRegion.cpp

*/ 
#include <math.h>
#include <time.h>
#include "RingGoalRegion.h"

RingGoalRegion::RingGoalRegion(const Point2d &center, double radius, double width)
  :center_(center),radius_(radius),width_(width),samples_num_(20),
    step_angle_rad_(2*M_PI/samples_num_), counter_(0)
{
}

RingGoalRegion::~RingGoalRegion()
{
  // nothing to do
}

void RingGoalRegion::init(unsigned samples_num)
{
  samples_num_=samples_num;
  step_angle_rad_=2*M_PI/samples_num;
  counter_=0;
  srand ( (unsigned)time(NULL));

}

bool RingGoalRegion::getNextGoal(Pose2d &goal)
{
  if (counter_>=samples_num_) {
    return false;
  } else {
    double r = radius_+(((double)rand()/(RAND_MAX))-0.5)*width_;
    double alpha=counter_*step_angle_rad_;
    goal.x=center_.x+r*cos(alpha);
    goal.y=center_.y+r*sin(alpha);
    goal.theta =alpha+(counter_%2-0.5)*M_PI;
    goal.theta=MathHelper::AngleClamp(goal.theta);
    return true;
  }
}





