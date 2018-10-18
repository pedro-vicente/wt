#include "Wt/WLogger.h"
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include "web/WebUtils.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>
#include "extensions/WLeafletMapbox.hh"

namespace Wt
{
  ///////////////////////////////////////////////////////////////////////////////////////
  //WMapbox::WMapbox
  ///////////////////////////////////////////////////////////////////////////////////////

  WMapbox::WMapbox()
  {
    setImplementation(std::unique_ptr<WWidget>(new WContainerWidget()));
    this->addCssRule("#" + id(), "position:relative; top:0; bottom:0; right: 0; left: 0; width: 100%; height: 100%");
    this->addCssRule("html", "height: 100%;");
    this->addCssRule("body", "height: 100%;");

    Wt::WApplication *app = Wt::WApplication::instance();
    app->useStyleSheet("https://unpkg.com/leaflet@1.0.3/dist/leaflet.css");
    const std::string leaflet = "https://unpkg.com/leaflet@1.0.3/dist/leaflet.js";
    app->useStyleSheet("https://api.tiles.mapbox.com/mapbox-gl-js/v0.35.1/mapbox-gl.css");
    const std::string mapbox_gl = "https://api.tiles.mapbox.com/mapbox-gl-js/v0.35.1/mapbox-gl.js";
    const std::string leaflet_mapbox_gl = "leaflet-mapbox-gl.js";
    app->require(leaflet, "leaflet");
    app->require(mapbox_gl, "mapbox_gl");
    app->require(leaflet_mapbox_gl, "leaflet_mapbox_gl");
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WMapbox::render
  ///////////////////////////////////////////////////////////////////////////////////////

  void WMapbox::render(WFlags<RenderFlag> flags)
  {
    if (flags.test(RenderFlag::Full))
    {
      Wt::WApplication * app = Wt::WApplication::instance();
      Wt::WString initFunction = app->javaScriptClass() + ".init_leaflet_" + id();
      std::string mapbox_key;
      bool read = Wt::WApplication::readConfigurationProperty("mapbox_api_key", mapbox_key);
      Wt::WStringStream strm;
      bool use_leaflet = true;

      strm
        << "{ " << initFunction.toUTF8() << " = function() {\n"
        << "  var self = " << jsRef() << ";\n"
        << "  if (!self) {\n"
        << "    setTimeout(" << initFunction.toUTF8() << ", 0);\n"
        << "  }\n";

      if (use_leaflet)
      {
        strm
          << "  var map = L.map(self).setView([38.9072, -77.0369], 13);\n";
        strm
          << "  var gl = L.mapboxGL({\n"
          << "  accessToken:'" << mapbox_key << "',\n"
          << "  style: 'mapbox://styles/mapbox/streets-v9'\n"
          << "  }).addTo(map);\n";
      }
      else
      {
        strm
          << "  mapboxgl.accessToken = '" << mapbox_key << "';\n"
          << "  var map = new mapboxgl.Map({\n"
          << "  container: self,\n"
          << "  style: 'mapbox://styles/mapbox/streets-v9',\n"
          << "  center: [-77.0369, 38.9072],\n"
          << "  zoom: 13\n"
          << "  });\n";
      }

      ///////////////////////////////////////////////////////////////////////////////////////
      //add promitives/data
      ///////////////////////////////////////////////////////////////////////////////////////

      for (size_t idx = 0; idx < m_additions.size(); idx++)
      {
        strm << m_additions[idx];
      }

      strm
        << "  setTimeout(function(){ delete " << initFunction.toUTF8() << ";}, 0)};\n"
        << "}\n"
        << initFunction.toUTF8() << "();\n";

      //clear primitives, already rendered
      m_additions.clear();
      app->doJavaScript(strm.str());
    }

    Wt::WCompositeWidget::render(flags);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WMapbox::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WMapbox::Circle(const double lat, const double lon, const double radius, const std::string &color)
  {
    Wt::WStringStream strm;
    std::string str_clr = std::to_string((long double)radius);

    strm
      << " var opt = {radius:"
      << radius
      << ", stroke: false, color: '"
      << color
      << "'}\n";

    std::string str_ll;
    str_ll = std::to_string((long double)lat);
    str_ll += ",";
    str_ll += std::to_string((long double)lon);

    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(map);\n";

    m_additions.push_back(strm.str());

  }


}
