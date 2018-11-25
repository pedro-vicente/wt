#include <fstream>
#include <assert.h>
#include "chart_model.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int model_t::read(const std::string &file_name)
{
  std::string str;
  std::ifstream ifs(file_name);
  if (!ifs.is_open())
  {
    assert(0);
    return -1;
  }

  std::getline(ifs, str);
  m_ticker = str;
  std::getline(ifs, str);
  m_period = str;
  std::getline(ifs, str);
  m_interval = std::stoi(str);

  std::getline(ifs, str);
  size_t nbr_rows = (size_t)std::stoull(str);
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
//model_t::set_wave
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::set_wave(const std::string &wave)
{
  m_tp.at(m_idx_curr).wave = wave;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::set_wave
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
