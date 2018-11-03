#ifndef _SENSOR_RECEIVE_H
#define _SENSOR_RECEIVE_H

#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>
#include <ctime>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//sensor_receive_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class sensor_receive_t
{
public:
  sensor_receive_t(std::string _name, int _id, double _lat, double _lon, std::time_t _time, int _level) :
    name(_name),
    id(_id),
    lat(_lat),
    lon(_lon),
    time(_time),
    level(_level)
  {
  }
  std::string name;
  int id;
  double lat;
  double lon;
  int level;
  std::time_t time;
};

#endif

