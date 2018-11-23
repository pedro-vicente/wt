#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WStringListModel.h>
#include <Wt/WTemplate.h>
#include <Wt/WDateTime.h>
#include "extensions/WPlotly.hh"
#include <Wt/WContainerWidget.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTimer.h>
#include <fstream>
using namespace Wt;

//./test_chart.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. -d ../../../examples/test_extensions/data/DJI_2018_minor.3600.txt

std::string file_plotly;

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
  model_t()
  {};
  std::vector<time_price_t> m_tp;
  int read(const std::string &file_name);
  int m_interval; //seconds
  std::string m_period;
  std::string m_ticker;
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_plotly
///////////////////////////////////////////////////////////////////////////////////////

class Application_plotly : public WApplication
{
public:
  Application_plotly(const WEnvironment& env) : WApplication(env)
  {
    setTitle("Chart");
    model_t reader;
    std::string js;
    if (reader.read(file_plotly) < 0)
    {
      assert(0);
      return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // time/price
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    js += "var x_time = [];";
    js += "var y_price = [];";
    js += "var x_wave = [];";
    js += "var y_wave = [];";
    js += "var y_wave_label = [];";
    size_t nbr_rows = reader.m_tp.size();
    for (size_t idx = 0; idx < nbr_rows; idx++)
    {
      time_price_t tp = reader.m_tp.at(idx);
      WDateTime date;
      date.setTime_t(std::time_t(tp.time));
      WString str_time = date.toString("yyyy-MM-dd-hh:mm:ss");
      js += "x_time.push('";
      js += str_time.toUTF8();
      js += "');";
      js += "y_price.push(";
      js += std::to_string(tp.value);
      js += ");";
      if (tp.wave != "-")
      {
        js += "x_wave.push('";
        js += str_time.toUTF8();
        js += "');";
        js += "y_wave.push(";
        js += std::to_string(tp.value);
        js += ");";
        js += "y_wave_label.push(";
        js += tp.wave;
        js += ");";
      }
    }

    js += "var trace_price = {";
    js += "x: x_time,";
    js += "y: y_price,";
    js += "mode: 'lines',";
    js += "type: 'scatter'";
    js += "};";

    js += "var trace_wave = {";
    js += "x: x_wave,";
    js += "y: y_wave,";
    js += "text: y_wave_label,";
    js += "mode: 'markers+text',";
    js += "type: 'scatter',";
    js += "marker: {size: 15, color:'rgb(0,255,0)'}";
    js += "};";

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // "data", "layout" objects
    // Note: data object MUST named 'data', layout object MUST named 'layout'
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    js += "var data = [trace_price, trace_wave];";
    js += "var layout = {";
    js += "yaxis: {";
    js += "tickformat: '.0'";
    js += "}";
    js += "};";

    WVBoxLayout *hbox;
    WText *text;
    hbox = root()->setLayout(cpp14::make_unique<WVBoxLayout>());
    text = hbox->addWidget(cpp14::make_unique<WText>(Wt::asString("Dow")));
    plotly = hbox->addWidget(cpp14::make_unique<WPlotly>(js));
    plotly->clicked().connect([=](WPlotly::Coordinate c)
    {
      std::cerr << "Clicked at coordinate (" << c.x() << "," << c.y() << ")";
    });
  }

  WPlotly *plotly;
};

///////////////////////////////////////////////////////////////////////////////////////
//create_application
///////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<WApplication> create_application(const WEnvironment& env)
{
  return cpp14::make_unique<Application_plotly>(env);
}

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  std::string data_file;
  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 'd':
        data_file = argv[i + 1];
        i++;
        break;
      }
    }
  }
  std::cout << data_file << std::endl;
  file_plotly = data_file;
  return WRun(argc, argv, &create_application);
}

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