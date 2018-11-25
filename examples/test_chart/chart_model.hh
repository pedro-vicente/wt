#ifndef _CHART_MODEL_H_
#define _CHART_MODEL_H_

#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//time_price_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class time_price_t
{
public:
  time_t time; //unix time
  float value; //price, y scale
  std::string wave; //name
  size_t duration; //interval since last wave count (computed)
  std::string trade; //trade or '-'
  time_price_t(time_t t, float v, const std::string& w) :
    time(t),
    value(v),
    wave(w),
    trade("-"),
    duration((size_t)-1)
  {
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class model_t
{
public:
  model_t() :
    m_idx_curr(0)
  {};
  std::vector<time_price_t> m_tp;
  int read(const std::string &file_name);
  void write(const std::string &file_name);
  void reset_wave(const std::string &wave);
  void set_wave(const std::string &wave);
  void set_index(time_t time); //set time index based on mouse input
  int m_interval; //seconds
  std::string m_period;
  std::string m_ticker;
  size_t m_idx_curr;
};


#endif