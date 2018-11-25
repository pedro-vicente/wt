#ifndef _CHART_MODEL_H_
#define _CHART_MODEL_H_ 1

#include <vector>
#include <string>

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
//m_expected_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class expected_t
{
public:
  size_t index;
  float price;
  std::string ratio;//fibonnaci ratio
  std::string wave; //name
  expected_t()
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

  //read from file
  int m_interval; //seconds
  std::string m_period;
  std::string m_ticker;

  std::vector<time_price_t> m_tp;
  int read(const std::string &file_name);
  void write(const std::string &file_name);
  void reset_wave(const std::string &wave);
  void set_wave(const std::string &wave);
  void set_index(time_t time); //set time index based on mouse input
  size_t m_idx_curr;

  std::vector<expected_t> m_expected;
  void reset_waves();
  void reset_trades();
  size_t find_wave(const std::string &wave);
  time_price_t get_max_value();
  void ratios();
};

#endif
