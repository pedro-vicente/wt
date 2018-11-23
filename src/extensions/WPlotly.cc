#include <Wt/WLogger.h>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include "web/WebUtils.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>

#include "extensions/WPlotly.hh"

namespace Wt
{
  LOGGER("WPlotly");

  ///////////////////////////////////////////////////////////////////////////////////////
  //Coordinate
  ///////////////////////////////////////////////////////////////////////////////////////

  WPlotly::Coordinate::Coordinate() :
    m_x(0),
    m_y(0)
  {
  }

  WPlotly::Coordinate::Coordinate(time_t x, double y)
  {
    set_x(x);
    set_y(y);
  }

  WPlotly::Coordinate::Coordinate(const std::pair<time_t, double>& x_y)
  {
    set_x(x_y.first);
    set_y(x_y.second);
  }

  void WPlotly::Coordinate::set_x(time_t x)
  {
    m_x = x;
  }

  void WPlotly::Coordinate::set_y(double y)
  {
    m_y = y;
  }

  std::pair<time_t, double> WPlotly::Coordinate::operator ()() const
  {
    return std::make_pair(m_x, m_y);
  }

  std::istream& operator >> (std::istream& i, WPlotly::Coordinate& c)
  {
    time_t x;
    double y;
    i >> x >> std::ws >> y;
    c.set_x(x);
    c.set_y(y);
    return i;
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WPlotly::WPlotly
  ///////////////////////////////////////////////////////////////////////////////////////

  WPlotly::WPlotly(const std::string &js) :
    m_clicked(this, "click")
  {
    setImplementation(std::unique_ptr<WWidget>(new WContainerWidget()));
    this->addCssRule("html", "height: 100%");
    this->addCssRule("body", "height: 100%");
    this->addCssRule("#" + id(), "position:relative; top:0; bottom:0; height: 100%");
    WApplication *app = WApplication::instance();
#if defined (_DEBUG)
    const std::string library = "plotly-latest.min.js";
#else
    const std::string library = "https://cdn.plot.ly/plotly-latest.min.js";
#endif
    app->require(library, "plotly");
    m_javascrit = js;
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WPlotly::render
  ///////////////////////////////////////////////////////////////////////////////////////

  void WPlotly::render(WFlags<RenderFlag> flags)
  {
    if (flags.test(RenderFlag::Full))
    {
      Wt::WApplication * app = Wt::WApplication::instance();
      Wt::WString initFunction = app->javaScriptClass() + ".init_leaflet_" + id();
      Wt::WStringStream strm;

      strm
        << "{ " << initFunction.toUTF8() << " = function() {\n"
        << "  var self = " << jsRef() << ";\n"
        << "  if (!self) {\n"
        << "    setTimeout(" << initFunction.toUTF8() << ", 0);\n"
        << "  }\n";

      //chart code
      strm << m_javascrit;

      //pass 'DIV', data object MUST named 'data', layout object MUST named 'layout'
      strm
        << "  Plotly.newPlot(self, data, layout);\n";

      strm
        << "self.on('plotly_click', function(data){"
        << "var str = data.points[0].x;"
        << "var year = str.substr(0, 4);"
        << "var month = str.substr(5, 2);"
        << "var day = str.substr(8, 2);"
        << "var hour = str.substr(11, 2);"
        << "var min = str.substr(14, 2);"
        << "var d = new Date(year, month, day, hour, min, 0, 0);"
        << "var n = d.getTime();"
        << m_clicked.createCall({ "n +' '+ data.points[0].y" })
        << ";"
        << "});";

      strm
        << "  setTimeout(function(){ delete " << initFunction.toUTF8() << ";}, 0)};\n"
        << "}\n"
        << initFunction.toUTF8() << "();\n";

      if (1) LOG_INFO(strm.str());
      app->doJavaScript(strm.str(), true);
    }

    Wt::WCompositeWidget::render(flags);
  }


}
