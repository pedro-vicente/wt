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
#include "chart_model.hh"
using namespace Wt;

//./test_chart.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. -d ../../../examples/test_chart/DJI_2018_minor.3600.txt

std::string file_plotly;
const WColor color_grey = WColor();
const WColor color_green = WColor(0, 255, 0, 128);

///////////////////////////////////////////////////////////////////////////////////////
//Application_plotly
///////////////////////////////////////////////////////////////////////////////////////

class Application_plotly : public WApplication
{
public:
  WPlotly *m_plotly;
  WPushButton *wave_0;
  WPushButton *wave_1;
  model_t m_model;
  std::string m_wave;

  Application_plotly(const WEnvironment& env) :
    WApplication(env),
    m_wave("0")
  {
    setTitle("Chart");
    useStyleSheet("boxes.css");
    if (m_model.read(file_plotly) < 0)
    {
      assert(0);
      return;
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //layout
    ///////////////////////////////////////////////////////////////////////////////////////

    auto container = Wt::cpp14::make_unique<Wt::WContainerWidget>();
    container->setStyleClass("yellow-box");
    WVBoxLayout *hbox = container->setLayout(cpp14::make_unique<WVBoxLayout>());

    WText *text = hbox->addWidget(cpp14::make_unique<WText>(Wt::asString("Dow")));
    text->setStyleClass("green-box");

    ///////////////////////////////////////////////////////////////////////////////////////
    //buttons
    ///////////////////////////////////////////////////////////////////////////////////////

    wave_0 = hbox->addWidget(cpp14::make_unique<WPushButton>("0"));
    wave_0->setWidth(70);
    wave_0->clicked().connect(this, &Application_plotly::set_wave_0);
    wave_1 = hbox->addWidget(cpp14::make_unique<WPushButton>("1"));
    wave_1->setWidth(70);
    wave_1->clicked().connect(this, &Application_plotly::set_wave_1);

    wave_0->decorationStyle().setBackgroundColor(color_green);
    wave_1->decorationStyle().setBackgroundColor(color_grey);

    ///////////////////////////////////////////////////////////////////////////////////////
    //plot
    ///////////////////////////////////////////////////////////////////////////////////////

    m_plotly = hbox->addWidget(cpp14::make_unique<WPlotly>(set_data()));
    text->setStyleClass("blue-box");
    m_plotly->clicked().connect([=](WPlotly::Coordinate c)
    {
      std::cerr << "Clicked at coordinate (" << c.x() << "," << c.y() << ")";
      this->update_model(c.x() / 1000);
    });

    root()->addWidget(std::move(container));
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //update_model()
  ///////////////////////////////////////////////////////////////////////////////////////

  void update_model(time_t time)
  {
    m_model.set_index(time);
    m_model.reset_wave(m_wave);
    m_model.set_wave(m_wave);
    Wt::WStringStream strm;
    strm
      << set_data();
    strm
      << "var self = " << m_plotly->jsRef() << ";\n"
      << "Plotly.react(self, data, layout);";
    m_plotly->doJavaScript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //set_data()
  // "data", "layout" objects
  // Note: data object MUST named 'data', layout object MUST named 'layout'
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string set_data()
  {
    Wt::WStringStream strm;
    strm
      << "var x_time = []; var y_price = []; var x_wave = []; var y_wave = []; var y_wave_label = [];";
    size_t nbr_rows = m_model.m_tp.size();
    for (size_t idx = 0; idx < nbr_rows / 2; idx++)
    {
      time_price_t tp = m_model.m_tp.at(idx);
      WDateTime date;
      date.setTime_t(std::time_t(tp.time));
      WString str_time = date.toString("yyyy-MM-dd-hh:mm:ss");
      strm
        << "x_time.push('" << str_time.toUTF8() << "');"
        << "y_price.push(" << std::to_string(tp.value) << ");";
      if (tp.wave != "-")
      {
        strm
          << "x_wave.push('" << str_time.toUTF8() << "');"
          << "y_wave.push(" << std::to_string(tp.value) << ");"
          << "y_wave_label.push(" << tp.wave << ");";
      }
    }
    strm
      << "var trace_price = {"
      << "x: x_time,"
      << "y: y_price,"
      << "mode: 'lines',"
      << "type: 'scatter'"
      << "};";
    strm
      << "var trace_wave = {"
      << "x: x_wave,"
      << "y: y_wave,"
      << "text: y_wave_label,"
      << "mode: 'markers+text',"
      << "type: 'scatter',"
      << "marker: {size: 15, color:'rgb(0,255,0)'}"
      << "};";
    strm
      << "var data = [trace_price, trace_wave];"
      "var layout = {"
      "yaxis: {"
      "tickformat: '.0'"
      "}"
      "};";
    return strm.str();
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //set_wave_()
  ///////////////////////////////////////////////////////////////////////////////////////

  void set_wave_0()
  {
    wave_0->decorationStyle().setBackgroundColor(color_green);
    wave_1->decorationStyle().setBackgroundColor(color_grey);
    m_wave = "0";
  }

  void set_wave_1()
  {
    wave_0->decorationStyle().setBackgroundColor(color_grey);
    wave_1->decorationStyle().setBackgroundColor(color_green);
    m_wave = "1";
  }
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

