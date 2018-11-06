#include "Wt/WLogger.h"
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPointF.h>
#include "web/WebUtils.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>

#include "extensions/WLeaflet.hh"

bool gverbose_debug = false;

namespace Wt
{
  LOGGER("WLeaflet");

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::WLeaflet
  ///////////////////////////////////////////////////////////////////////////////////////

  WLeaflet::WLeaflet(tile_provider_t tile, double lat, double lon, int zoom, bool verbose) :
    m_tile(tile),
    m_lat(lat),
    m_lon(lon),
    m_zoom(zoom)
  {
    auto container{ new Wt::WContainerWidget };
    setImplementation(std::unique_ptr<WWidget>(container));
    container->mouseWentDown().connect(this, &WLeaflet::MouseDown);
    this->addCssRule("html", "height: 100%");
    this->addCssRule("body", "height: 100%");
    this->addCssRule("#" + id(), "position:relative; top:0; bottom:0; height: 100%");
    WApplication *app = WApplication::instance();

    std::string leaflet = "leaflet.js";
    std::string mapbox_gl = "mapbox-gl.js";
    std::string leaflet_mapbox_gl = "leaflet-mapbox-gl.js";

    if (verbose)
    {
      gverbose_debug = true;
      app->useStyleSheet("leaflet.css");
      leaflet = "leaflet.js";
      mapbox_gl = "mapbox-gl.js";
      leaflet_mapbox_gl = "leaflet-mapbox-gl.js";
    }
    else
    {
      app->useStyleSheet("https://unpkg.com/leaflet@1.0.3/dist/leaflet.css");
      leaflet = "https://unpkg.com/leaflet@1.0.3/dist/leaflet.js";
      if (m_tile == tile_provider_t::MAPBOX)
      {
        mapbox_gl = "https://api.tiles.mapbox.com/mapbox-gl-js/v0.35.1/mapbox-gl.js";
        app->useStyleSheet("https://api.tiles.mapbox.com/mapbox-gl-js/v0.35.1/mapbox-gl.css");
        leaflet_mapbox_gl = "https://rawgit.com/mapbox/mapbox-gl-leaflet/master/leaflet-mapbox-gl.js";
      }
    }
    app->require(leaflet, "leaflet");
    if (m_tile == tile_provider_t::MAPBOX)
    {
      app->require(mapbox_gl, "mapbox_gl");
      app->require(leaflet_mapbox_gl, "leaflet_mapbox_gl");
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::render
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::MouseDown(const Wt::WMouseEvent &e)
  {
    Wt::WPointF p = Wt::WPointF(e.widget().x, e.widget().y);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::render
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::render(WFlags<RenderFlag> flags)
  {
    if (flags.test(RenderFlag::Full))
    {
      Wt::WApplication * app = Wt::WApplication::instance();
      Wt::WString initFunction = app->javaScriptClass() + ".init_leaflet_" + id();
      Wt::WStringStream strm;
      std::string mapbox_key;
      bool read = Wt::WApplication::readConfigurationProperty("mapbox_api_key", mapbox_key);

      std::string str_z = std::to_string((int)m_zoom);
      std::string ll;
      ll = std::to_string((long double)m_lat);
      ll += ",";
      ll += std::to_string((long double)m_lon);

      strm
        << "{ " << initFunction.toUTF8() << " = function() {\n"
        << "  var self = " << jsRef() << ";\n"
        << "  if (!self) {\n"
        << "    setTimeout(" << initFunction.toUTF8() << ", 0);\n"
        << "  }\n";

      if (m_tile == tile_provider_t::RRZE)
      {
        strm
          << "  var layer_base = L.tileLayer(\n"
          << "  'http://{s}.osm.rrze.fau.de/osmhd/{z}/{x}/{y}.png',{\n"
          << "  attribution: '<a href=http://www.openstreetmap.org/copyright>(C) Openstreetmap Contributors</a>'\n"
          << "  });\n";
      }
      else if (m_tile == tile_provider_t::CARTODB)
      {
        strm
          << "  var layer_base = L.tileLayer(\n"
          << "  'http://cartodb-basemaps-{s}.global.ssl.fastly.net/light_all/{z}/{x}/{y}@2x.png',{\n"
          << "  opacity: 1,\n"
          << "  attribution: '&copy; <a href=http://www.openstreetmap.org/copyright>OpenStreetMap</a>, &copy; <a href=https://carto.com/attribution>CARTO</a>'\n"
          << "  });\n";
      }
      else if (m_tile == tile_provider_t::MAPBOX)
      {
        strm
          << "  var map = L.map(self).setView([" << ll << "], " << str_z << ");\n";
        strm
          << "  var gl = L.mapboxGL({\n"
          << "  accessToken:'" << mapbox_key << "',\n"
          << "  style: 'mapbox://styles/mapbox/streets-v9'\n"
          << "  }).addTo(map);\n";
      }

      if (m_tile == tile_provider_t::CARTODB || m_tile == tile_provider_t::RRZE)
      {
        strm
          << "  var map = new L.Map(self, {\n"
          << "  center: new L.LatLng(" << ll << "), \n"
          << "  zoom: " << str_z << ",\n"
          << "  scrollWheelZoom: false,\n"
          << "  layers: [layer_base]\n"
          << "  });\n";
      }

      strm << "self.map = map;";

      for (size_t idx = 0; idx < m_additions.size(); idx++)
      {
        strm << m_additions[idx];
      }

      strm
        << "  setTimeout(function(){ delete " << initFunction.toUTF8() << ";}, 0)};\n"
        << "}\n"
        << initFunction.toUTF8() << "();\n";

      if (gverbose_debug) LOG_INFO(strm.str());

      m_additions.clear();
      app->doJavaScript(strm.str(), true);
    }

    Wt::WCompositeWidget::render(flags);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const std::string &lat, const std::string &lon)
  {
    Wt::WStringStream strm;

    strm
      << " var opt = {radius: 100, stroke: false, color: '#ff0000'};\n";

    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;

    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(map);\n";

    m_additions.push_back(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const double lat, const double lon, const std::string &color)
  {
    Wt::WStringStream strm;

    strm
      << " var opt = {radius: 100, stroke: false, color: '"
      << color
      << "'};\n";

    std::string str_ll;
    str_ll = std::to_string((long double)lat);
    str_ll += ",";
    str_ll += std::to_string((long double)lon);

    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(map);\n";

    m_additions.push_back(strm.str());

  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const std::string &lat, const std::string &lon, const std::string &color)
  {
    Wt::WStringStream strm;

    strm
      << " var opt = {radius: 1000, stroke: false, color: '"
      << color
      << "'};\n";

    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;

    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(map);\n";

    m_additions.push_back(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const double lat, const double lon, const double radius, const std::string &color)
  {
    Wt::WStringStream strm;
    std::string str_clr = std::to_string((long double)radius);

    strm
      << " var opt = {radius:"
      << radius
      << ", stroke: false, color: '"
      << color
      << "'};\n";

    std::string str_ll;
    str_ll = std::to_string((long double)lat);
    str_ll += ",";
    str_ll += std::to_string((long double)lon);

    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(map);\n";

    m_additions.push_back(strm.str());

  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Polygon
  //draw a weight of 1 pixel and opacity .1 for border 
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Polygon(const std::vector<double> &lat, const std::vector<double> &lon,
    const std::string &color)
  {
    Wt::WStringStream strm;

    if (lat.size() == 0)
    {
      return;
    }

    strm
      << "var vert = [ [";

    for (size_t idx = 0; idx < lat.size() - 1; idx++)
    {
      strm
        << std::to_string((long double)lat.at(idx))
        << ","
        << std::to_string((long double)lon.at(idx))
        << "], [";
    }

    //last
    size_t idx = lat.size() - 1;
    strm
      << std::to_string((long double)lat.at(idx))
      << ","
      << std::to_string((long double)lon.at(idx))
      << "] ];";

    strm
      << "L.polygon(vert,{color:'"
      << color
      << "',opacity:.1,weight:1}).addTo(map);";

    m_additions.push_back(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Marker
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Marker(const std::string &lat, const std::string &lon, const std::string &text)
  {
    Wt::WStringStream strm;
    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;
    strm
      << " L.marker([" << str_ll << "]).addTo(map)"
      << ".bindPopup('"
      << text
      << "');\n";
    m_additions.push_back(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Marker
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Marker(const std::string &lat, const std::string &lon, const std::string &text, marker_icon_t icon)
  {
    Wt::WStringStream strm;

    strm
      << "var custom_icon = new L.Icon({"
      << "iconUrl: '"
      << icon.icon_url
      << "',"
      << "shadowUrl: '"
      << icon.shadow_url
      << "',"
      << "iconSize: [25, 41],"
      << "iconAnchor: [12, 41],"
      << "popupAnchor: [1, -34],"
      << "shadowSize: [41, 41]"
      << "});\n";

    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;
    strm
      << " L.marker([" << str_ll << "], {icon: custom_icon}).addTo(map)"
      << ".bindPopup('"
      << text
      << "');\n";
    m_additions.push_back(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Marker
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Marker(const double lat, const double lon, const std::string &text, marker_icon_t icon)
  {
    std::string ll;
    ll = std::to_string((long double)lat);
    ll += ",";
    ll += std::to_string((long double)lon);
    Wt::WStringStream strm;

    strm
      << "var custom_icon = new L.Icon({"
      << "iconUrl: '"
      << icon.icon_url
      << "',"
      << "shadowUrl: '"
      << icon.shadow_url
      << "',"
      << "iconSize: [25, 41],"
      << "iconAnchor: [12, 41],"
      << "popupAnchor: [1, -34],"
      << "shadowSize: [41, 41]"
      << "});\n";
    strm
      << " L.marker([" << ll << "], {icon: custom_icon}).addTo(map)"
      << ".bindPopup('"
      << text
      << "');\n";
    m_additions.push_back(strm.str());
  }
}
