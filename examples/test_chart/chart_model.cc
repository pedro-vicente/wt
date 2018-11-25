#include "chart_model.hh"
#include <fstream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int model_t::read(const std::string &file_name)
{
  std::string str;
  std::ifstream ifs(file_name);
  if (!ifs.is_open())
  {
    return -1;
  }

  std::getline(ifs, str);
  m_ticker = str;
  std::getline(ifs, str);
  m_period = str;
  std::getline(ifs, str);
  m_interval = std::stoi(str);

  std::getline(ifs, str);
  size_t nbr_rows = std::stoull(str);
  for (size_t idx = 0; idx < nbr_rows; idx++)
  {
    std::getline(ifs, str);
    size_t pos_c1 = str.find(",");
    size_t pos_c2 = str.find(",", pos_c1 + 1);
    size_t pos_c3 = str.find(",", pos_c2 + 1);
    std::string str_time = str.substr(0, pos_c1);
    std::string str_value = str.substr(pos_c1 + 1, pos_c2 - pos_c1 - 1);
    std::string str_wave;
    std::string str_trade;
    if (pos_c3 != std::string::npos)
    {
      str_wave = str.substr(pos_c2 + 1, pos_c3 - pos_c2 - 1);
      str_trade = str.substr(pos_c3 + 1);
    }
    else
    {
      str_wave = str.substr(pos_c2 + 1);
    }
    time_t time = std::stoull(str_time);
    float value = std::stof(str_value);
    time_price_t tp(time, value, str_wave);
    if (pos_c3 != std::string::npos)
    {
      tp.trade = str_trade;
    }
    m_tp.push_back(tp);
  }
  ifs.close();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //calculate
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  ratios();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::write
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::write(const std::string &file_name)
{
  std::ofstream ofs(file_name, std::ios::out | std::ios::binary);
  ofs << m_ticker << "\n";
  ofs << m_period << "\n";
  ofs << m_interval << "\n";
  size_t size = m_tp.size();
  ofs << size << "\n";
  for (size_t idx = 0; idx < size; idx++)
  {
    ofs << m_tp.at(idx).time << ","
      << m_tp.at(idx).value << ","
      << m_tp.at(idx).wave << ","
      << m_tp.at(idx).trade << "\n";
  }
  ofs.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::set_wave
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::set_wave(const std::string &wave)
{
  m_tp.at(m_idx_curr).wave = wave;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::set_index
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::set_index(time_t time)
{
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    time_price_t tp = m_tp.at(idx);
    if (tp.time > time)
    {
      m_idx_curr = idx;
      break;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::reset_wave
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::reset_wave(const std::string &wave)
{
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    if (m_tp.at(idx).wave == wave)
    {
      m_tp.at(idx).wave = "-";
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::reset_waves
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::reset_waves()
{
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    m_tp.at(idx).wave = "-";
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::reset_trades
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::reset_trades()
{
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    m_tp.at(idx).trade = "-";
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::find_wave
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t model_t::find_wave(const std::string &wave)
{
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    if (m_tp.at(idx).wave == wave)
    {
      return idx;
    }
  }
  return (size_t)-1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::get_max_value
/////////////////////////////////////////////////////////////////////////////////////////////////////

time_price_t model_t::get_max_value()
{
  float value = 1; //price, y scale
  size_t index = 0;
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    if (m_tp.at(idx).value > value)
    {
      value = m_tp.at(idx).value;
      index = idx;
    }
  }
  return m_tp.at(index);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::ratios
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::ratios()
{
  float retracement_0500 = 0.5f;
  float retracement_1000 = 1.f;

  float fibonnaci_0382 = 0.382f;
  float fibonnaci_0618 = 0.618f;
  float fibonnaci_0786 = 0.786f;
  float fibonnaci_1618 = 1.618f;

  float value_0 = -1;
  float value_1 = -1;
  float value_2 = -1;
  float value_3 = -1;
  float value_4 = -1;
  float value_5 = -1;
  size_t idx_0 = find_wave("0");
  size_t idx_1 = find_wave("1");
  size_t idx_2 = find_wave("2");
  size_t idx_3 = find_wave("3");
  size_t idx_4 = find_wave("4");
  size_t idx_5 = find_wave("5");

  time_price_t max_tp = get_max_value();

  size_t duration = (size_t)-1;
  size_t duration_0_3 = (size_t)-1;
  size_t duration_0_1 = (size_t)-1;
  expected_t exp;
  float value;

  if (idx_0 != -1)
  {
    value_0 = m_tp.at(idx_0).value;
  }
  else return;

  if (idx_1 != -1)
  {
    value_1 = m_tp.at(idx_1).value;
    duration = idx_1 - idx_0;
    duration_0_1 = duration;
    m_tp.at(idx_1).duration = duration;
  }
  else return;

  int UP = 1;
  int DOWN = 0;
  if (value_1 < value_0)
  {
    UP = 0;
    DOWN = 1;
  }

  if (idx_2 != -1)
  {
    value_2 = m_tp.at(idx_2).value;
    duration = idx_2 - idx_1;
    m_tp.at(idx_2).duration = duration;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //project wave 2
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  else
  {
    float amplitude_wave_1 = value_1 - value_0;
    exp.wave = "2";

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 0.382
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_1 * fibonnaci_0382);
    if (DOWN)
    {
      exp.price = value_1 + value;
    }
    else
    {
      exp.price = value_1 - value;
    }

    //duration
    duration = duration_0_1 * retracement_0500;
    exp.index = idx_1 + duration;

    exp.ratio = "1*0.382";
    if (DOWN && exp.price > value_1) //must be higher than wave 1)
    {
      m_expected.push_back(exp);
    }
    else if (UP && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 0.5
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_1 * retracement_0500);
    if (DOWN)
    {
      exp.price = value_1 + value;
    }
    else
    {
      exp.price = value_1 - value;
    }

    //duration
    duration = duration_0_1 * fibonnaci_0618;
    exp.index = idx_1 + duration;

    exp.ratio = "1*0.5";
    if (DOWN && exp.price > value_1) //must be higher than wave 1)
    {
      m_expected.push_back(exp);
    }
    else if (UP && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 0.618
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_1 * fibonnaci_0618);
    if (DOWN)
    {
      exp.price = value_1 + value;
    }
    else
    {
      exp.price = value_1 - value;
    }

    //duration
    duration = duration_0_1 * fibonnaci_0786;
    exp.index = idx_1 + duration;

    exp.ratio = "1*0.618";
    if (DOWN && exp.price > value_1) //must be higher than wave 1)
    {
      m_expected.push_back(exp);
    }
    else if (UP && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 0.786
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_1 * fibonnaci_0786);
    if (DOWN)
    {
      exp.price = value_1 + value;
    }
    else
    {
      exp.price = value_1 - value;
    }
    //duration
    duration = duration_0_1 * retracement_1000;
    exp.index = idx_1 + duration;

    exp.ratio = "1*0.786";

    if (DOWN && exp.price > value_1) //must be higher than wave 1)
    {
      m_expected.push_back(exp);
    }
    else if (UP && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 1.0
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_1 * retracement_1000);
    if (DOWN)
    {
      exp.price = value_1 + value;
    }
    else
    {

    }
    //duration
    duration = duration_0_1 * fibonnaci_1618;
    exp.index = idx_1 + duration;

    exp.ratio = "1*1.0";

    if (DOWN && exp.price > value_1) //must be higher than wave 1)
    {
      m_expected.push_back(exp);
    }


    return;
  }

  if (idx_3 != -1)
  {
    value_3 = m_tp.at(idx_3).value;
    duration = idx_3 - idx_2;
    m_tp.at(idx_3).duration = duration;
    duration_0_3 = idx_3 - idx_0;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //project wave 3
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  else
  {
    float amplitude_wave_1 = value_1 - value_0;
    exp.wave = "3";

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 1.618
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = amplitude_wave_1 * fibonnaci_1618;
    exp.price = value_2 + value;
    //duration
    duration = duration_0_1 * fibonnaci_1618;
    exp.index = idx_2 + duration;

    if (DOWN && exp.price < value_1) //must be lower than wave 1)
    {
      m_expected.push_back(exp);
    }
    else if (UP && exp.price > value_1) //must be higehr than wave 1)
    {
      m_expected.push_back(exp);
    }


    //no more waves
    return;

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //project wave 4
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (idx_4 != -1)
  {
    value_4 = m_tp.at(idx_4).value;
    duration = idx_4 - idx_3;
    m_tp.at(idx_4).duration = duration;
  }
  else
  {
    float amplitude_wave_3 = value_3 - value_2;
    exp.wave = "4";

    //must be higher/lower than wave 3
    //must be higher/lower than wave 1

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 3 * 0.382
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_3 * fibonnaci_0382);
    if (UP)
    {
      exp.price = value_3 - value;
    }
    else
    {
      exp.price = value_3 + value;
    }
    //duration
    duration = duration_0_3 * retracement_0500;
    exp.index = idx_3 + duration;
    if (UP && exp.price < value_3 && exp.price > value_1)
    {
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price > value_3 && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 3 * 0.5
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_3 * retracement_0500);
    if (UP)
    {
      exp.price = value_3 - value;
    }
    else
    {
      exp.price = value_3 + value;
    }
    //duration
    duration = duration_0_3 * fibonnaci_0618;
    exp.index = idx_3 + duration;
    if (UP && exp.price < value_3 && exp.price > value_1)
    {
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price > value_3 && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 3 * 0.618
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = fabs(amplitude_wave_3 * fibonnaci_0618);
    if (UP)
    {
      exp.price = value_3 - value;
    }
    else
    {
      exp.price = value_3 + value;
    }
    //duration
    duration = duration_0_3 * fibonnaci_0786;
    exp.index = idx_3 + duration;
    if (UP && exp.price < value_3 && exp.price > value_1)
    {
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price > value_3 && exp.price < value_1)
    {
      m_expected.push_back(exp);
    }

    return;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //project wave 5
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (idx_5 != -1)
  {
    value_5 = m_tp.at(idx_5).value;
    duration = idx_5 - idx_4;
    m_tp.at(idx_5).duration = duration;
  }
  else
  {
    float amplitude_wave_3 = value_3 - value_2;
    float amplitude_wave_1 = value_1 - value_0;
    exp.wave = "5";

    //duration
    duration = duration_0_3 * fibonnaci_1618;
    exp.index = idx_0 + duration;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 3 * 0.618
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = amplitude_wave_3 * fibonnaci_0618;
    exp.price = value_4 + value;

    if (UP && exp.price > value_3)
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price < value_3) //must be lower than wave 3
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 3 * 1.000
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = amplitude_wave_3 * retracement_1000;
    exp.price = value_4 + value;

    if (UP && exp.price > value_3)
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price < value_3) //must be lower than wave 3
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 0.618
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = amplitude_wave_1 * fibonnaci_0618;
    exp.price = value_4 + value;

    if (UP && exp.price > value_3)
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price < value_3) //must be lower than wave 3
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 1.000
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = amplitude_wave_1 * retracement_1000;
    exp.price = value_4 + value;

    if (UP && exp.price > value_3)
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price < value_3) //must be lower than wave 3
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //wave 1 * 1.618
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //price
    value = amplitude_wave_1 * fibonnaci_1618;
    exp.price = value_4 + value;

    if (UP && exp.price > value_3)
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }
    else if (DOWN && exp.price < value_3) //must be lower than wave 3
    {
      if (exp.index < m_tp.size()) exp.index = m_tp.size();
      m_expected.push_back(exp);
    }
  }
}

