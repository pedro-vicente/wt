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
#include "test_extensions.hh"
#include "gason.h"
#include "extensions/WLeaflet.hh"
#include "extensions/WPlotly.hh"
#include "extensions/WCesium.hh"
#include "extensions/csv.hh"
#include "extensions/geojson.hh"
#include "extensions/topojson.hh"
#include "extensions/star_json.hh"
#include "extensions/star_dataset.hh"
#include "pal_rgb.h"

#include <thread>
#include <chrono>
#include <Wt/WContainerWidget.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTimer.h>
using namespace Wt;

//./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 2
//DC311 rodent complaints and DC wards geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 2 -d ../../../examples/test_extensions/data/dc_311-2016.csv.s0311.csv -g ../../../examples/test_extensions/data/ward-2012.geojson

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 3
//US states geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 3 -g ../../../examples/test_extensions/data/gz_2010_us_040_00_20m.json

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 4
//NOAA ATMS data
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 4 -d ../../../examples/test_extensions/data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json

std::vector<star_dataset_t> datasets;
star_dataset_t find_dataset(std::string name);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 5
//ep
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 5 
//-d ../../../examples/test_extensions/us_states_epilepsy_2015.csv 
//-g ../../../examples/test_extensions/gz_2010_us_040_00_20m.json 
//-u ../../../examples/test_extensions/us_states_population_2015.csv

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 6
//schools
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 6 -d ../../../examples/test_extensions/montgomery_county_schools.csv
//-g ../../../examples/test_extensions/montgomery_county_boundary.json 
//-m ../../../examples/test_extensions/wmata_stations.json
//-z ../../../examples/test_extensions/md_maryland_zip_codes_geo.min.json

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 7
//topojson 
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 7 -g ../../../examples/test_extensions/data/example.quantized.topojson
//-t 7 -g ../../../examples/test_extensions/data/czech-republic-regions.json
//-t 7 -g ../../../examples/test_extensions/data/portugal-districts.topojson
//-t 7 -g ../../../examples/test_extensions/data/us.topojson

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 8
//Celsium render
//DC311 rodent complaints 
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 8 -d ../../../examples/test_extensions/data/dc_311-2016.csv.s0311.csv

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 9
//Celsium render
//NOAA ATMS
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 9 -d ../../../examples/test_extensions/data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json 

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 10
//plot
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 10 -d ../../../examples/test_extensions/data/DJI_2018_minor.3600.txt

std::vector<school_t> schools_list;
std::vector<double> lat_montgomery;
std::vector<double> lon_montgomery;
std::vector<double> lat_wmata;
std::vector<double> lon_wmata;
std::string file_wmata_stations;
std::vector<wmata_station_t> wmata_station;
int read_schools(const std::string &file_name);
int read_json_montgomery_county(const std::string &file_name, std::vector<double> &lat, std::vector<double> &lon);
int read_json_wmata(const std::string &file_name, std::vector<double> &lat, std::vector<double> &lon);
std::string file_plotly;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//forward declarations
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_dc311(const std::string &file_name);
int read_ep_pop(const std::string &file_name_ep, const std::string &file_name_pop);
size_t find_ep(std::string state_name);
size_t find_population(std::string state_name);
std::string to_hex(int n);
std::string rgb_to_hex(int r, int g, int b);

///////////////////////////////////////////////////////////////////////////////////////
//globals
///////////////////////////////////////////////////////////////////////////////////////

std::string test;
std::vector<rgb_t> rgb_256;
geojson_t geojson;
topojson_t topojson;
std::vector<dc311_data_t> dc311_data;
std::vector<ep_data_t> ep_data;
std::vector<us_state_pop_t> us_state_pop;
std::vector<std::string> ward_color =
{ rgb_to_hex(128, 128, 0), //olive
rgb_to_hex(255, 255, 0), //yellow 
rgb_to_hex(0, 128, 0), //green
rgb_to_hex(0, 255, 0), //lime
rgb_to_hex(0, 128, 128), //teal
rgb_to_hex(0, 255, 255), //aqua
rgb_to_hex(0, 0, 255), //blue
rgb_to_hex(128, 0, 128) //purple
};


///////////////////////////////////////////////////////////////////////////////////////
//Application_test
//38.9072, -77.0369, 14 DC
//37.0902, -95.7129, 5 US
///////////////////////////////////////////////////////////////////////////////////////

class Application_test : public WApplication
{
public:
  Application_test(const WEnvironment& env) : WApplication(env)
  {
    setTitle("test");
    useStyleSheet("boxes.css");
    root()->setStyleClass("yellow-box");
    auto hbox = root()->setLayout(Wt::cpp14::make_unique<Wt::WHBoxLayout>());
    std::unique_ptr<WLeaflet> leaflet = cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13);
    leaflet->setStyleClass("green-box");
    WLength len = leaflet->height();
    leaflet->resize(800, len);
    hbox->addWidget(std::move(leaflet));
    std::unique_ptr<WText> text = Wt::cpp14::make_unique<Wt::WText>("item 2");
    text->setStyleClass("blue-box");
    hbox->addWidget(std::move(text));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_dc311
///////////////////////////////////////////////////////////////////////////////////////

class Application_dc311 : public WApplication
{
public:
  Application_dc311(const WEnvironment& env) : WApplication(env)
  {
    setTitle("dc311");
    std::unique_ptr<WLeaflet> leaflet =
      cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13);

    ///////////////////////////////////////////////////////////////////////////////////////
    //render geojson
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size_features = geojson.m_feature.size();
    for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
    {
      feature_t feature = geojson.m_feature.at(idx_fet);

      //make each feature have a unique color
      std::string color = ward_color.at(idx_fet);

      size_t size_geometry = feature.m_geometry.size();
      for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
      {
        geometry_t geometry = feature.m_geometry.at(idx_geo);
        size_t size_pol = geometry.m_polygons.size();

        for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
        {
          polygon_t polygon = geometry.m_polygons[idx_pol];
          size_t size_crd = polygon.m_coord.size();

          if (size_crd == 0)
          {
            continue;
          }

          ///////////////////////////////////////////////////////////////////////////////////////
          //render each polygon as a vector of vertices passed to Polygon
          ///////////////////////////////////////////////////////////////////////////////////////

          std::vector<double> lat;
          std::vector<double> lon;

          for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
          {
            lat.push_back(polygon.m_coord[idx_crd].m_lat);
            lon.push_back(polygon.m_coord[idx_crd].m_lon);
          }

          leaflet->Polygon(lat, lon, color);
        }  //idx_pol
      } //idx_geo
    } //idx_fet

    ///////////////////////////////////////////////////////////////////////////////////////
    //render CSV points from DC_311 database
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size = dc311_data.size();
    for (size_t idx = 0; idx < size; idx++)
    {
      dc311_data_t data = dc311_data.at(idx);
      leaflet->Circle(data.lat, data.lon);
    }

    root()->addWidget(std::move(leaflet));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_us_states
///////////////////////////////////////////////////////////////////////////////////////

class Application_us_states : public WApplication
{
public:
  Application_us_states(const WEnvironment& env) : WApplication(env)
  {
    setTitle("geojson");
    std::unique_ptr<WLeaflet> leaflet =
      cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 37.0902, -95.7129, 5);

    ///////////////////////////////////////////////////////////////////////////////////////
    //render geojson
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size_features = geojson.m_feature.size();
    for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
    {
      feature_t feature = geojson.m_feature.at(idx_fet);
      size_t nbr_palette_entries = rgb_256.size();
      double value = idx_fet;
      double value_min = 0;
      double value_range = size_features - value_min;
      size_t idx_pal = (size_t)(nbr_palette_entries * ((value - value_min) / value_range));
      idx_pal = (idx_pal < 0 ? 0 : (idx_pal >= nbr_palette_entries ? nbr_palette_entries - 1 : idx_pal));
      std::string color = rgb_to_hex(
        rgb_256.at(idx_pal).red,
        rgb_256.at(idx_pal).green,
        rgb_256.at(idx_pal).blue);

      size_t size_geometry = feature.m_geometry.size();
      for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
      {
        geometry_t geometry = feature.m_geometry.at(idx_geo);
        size_t size_pol = geometry.m_polygons.size();

        for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
        {
          polygon_t polygon = geometry.m_polygons[idx_pol];
          size_t size_crd = polygon.m_coord.size();

          if (size_crd == 0)
          {
            continue;
          }

          ///////////////////////////////////////////////////////////////////////////////////////
          //render each polygon as a vector of vertices passed to Polygon
          ///////////////////////////////////////////////////////////////////////////////////////

          std::vector<double> lat;
          std::vector<double> lon;

          for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
          {
            lat.push_back(polygon.m_coord[idx_crd].m_lat);
            lon.push_back(polygon.m_coord[idx_crd].m_lon);
          }

          if (geometry.m_type.compare("Point") == 0)
          {
            leaflet->Circle(lat[0], lon[0], color);
          }
          else if (geometry.m_type.compare("Polygon") == 0 ||
            geometry.m_type.compare("MultiPolygon") == 0)
          {
            leaflet->Polygon(lat, lon, color);
          }

        }  //idx_pol
      } //idx_geo
    } //idx_fet

    root()->addWidget(std::move(leaflet));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_atms
///////////////////////////////////////////////////////////////////////////////////////

class Application_atms : public WApplication
{
public:
  Application_atms(const WEnvironment& env) : WApplication(env)
  {
    star_dataset_t temperature = find_dataset("AntennaTemperature");
    star_dataset_t latitude = find_dataset("latitude");
    star_dataset_t longitude = find_dataset("longitude");
    size_t nbr_rows = temperature.m_shape[0];
    size_t nbr_cols = temperature.m_shape[1];
    size_t nbr_lev = temperature.m_shape[2];
    double temp_min;
    double temp_max;
    double lat_min;
    double lat_max;
    double lon_min;
    double lon_max;
    temperature.do_min_max(temp_min, temp_max);
    latitude.do_min_max(lat_min, lat_max);
    longitude.do_min_max(lon_min, lon_max);
    double lat = (lat_max + lat_min) / 2;
    double lon = (lon_max + lon_min) / 2;

    setTitle("atms");
    std::unique_ptr<WLeaflet> leaflet =
      cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, lat, lon, 5);

    ///////////////////////////////////////////////////////////////////////////////////////
    //render border
    ///////////////////////////////////////////////////////////////////////////////////////

    std::vector<double> blat(4);
    std::vector<double> blon(4);
    blat[0] = lat_min;
    blon[0] = lon_min;
    blat[1] = lat_max;
    blon[1] = lon_min;
    blat[2] = lat_max;
    blon[2] = lon_max;
    blat[3] = lat_min;
    blon[3] = lon_max;
    leaflet->Polygon(blat, blon, "#76d7c4");

    ///////////////////////////////////////////////////////////////////////////////////////
    //render data
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t idx_channel = 0;

    for (size_t idx_row = 0; idx_row < nbr_rows - 1; idx_row++)
    {
      for (size_t idx_col = 0; idx_col < nbr_cols - 1; idx_col++)
      {
        ///////////////////////////////////////////////////////////////////////////
        //polygon coordinates for a granule
        ///////////////////////////////////////////////////////////////////////////

        std::vector<double> vlat(4);
        std::vector<double> vlon(4);
        vlat[0] = latitude.value_at(idx_row, idx_col);
        vlon[0] = longitude.value_at(idx_row, idx_col);
        vlat[1] = latitude.value_at(idx_row, idx_col + 1);
        vlon[1] = longitude.value_at(idx_row, idx_col + 1);
        vlat[2] = latitude.value_at(idx_row + 1, idx_col + 1);
        vlon[2] = longitude.value_at(idx_row + 1, idx_col + 1);
        vlat[3] = latitude.value_at(idx_row + 1, idx_col);
        vlon[3] = longitude.value_at(idx_row + 1, idx_col);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //color
        //The Vizualization Toolkit, pg.156
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        size_t nbr_palette_entries = rgb_256.size();
        double value_range = temp_max - temp_min;
        double value_min = temp_min;
        double value = temperature.value_at(idx_row, idx_col, idx_channel);
        size_t idx_pal = (size_t)(nbr_palette_entries * ((value - value_min) / value_range));
        idx_pal = (idx_pal < 0 ? 0 : (idx_pal >= nbr_palette_entries ? nbr_palette_entries - 1 : idx_pal));
        std::string color = rgb_to_hex(
          rgb_256.at(idx_pal).red,
          rgb_256.at(idx_pal).green,
          rgb_256.at(idx_pal).blue);

        leaflet->Polygon(vlat, vlon, color);
      }
    }

    root()->addWidget(std::move(leaflet));
  }
};



///////////////////////////////////////////////////////////////////////////////////////
//Application_ep
///////////////////////////////////////////////////////////////////////////////////////

class Application_ep : public WApplication
{
public:
  Application_ep(const WEnvironment& env) : WApplication(env)
  {
    setTitle("geojson");
    std::unique_ptr<WLeaflet> leaflet =
      cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 37.0902, -95.7129, 5);

    ///////////////////////////////////////////////////////////////////////////////////////
    //render geojson
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size_features = geojson.m_feature.size();
    for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
    {
      feature_t feature = geojson.m_feature.at(idx_fet);

      //find ep by state name
      size_t ep = find_ep(feature.m_name);
      //find population by state name
      size_t population = find_population(feature.m_name);

      if (ep == 0 || population == 0)
      {
        continue;
      }

      double percentage_state_epilepsy = (double(ep) / double(population)) / 100.0;

      percentage_state_epilepsy *= 100000;
      std::string color;
      if (percentage_state_epilepsy < 10)
      {
        color = rgb_to_hex(0, 255, 0);
      }
      else if (percentage_state_epilepsy < 10.4)
      {
        color = rgb_to_hex(0, 128, 0);
      }
      else if (percentage_state_epilepsy < 10.8)
      {
        color = rgb_to_hex(0, 255, 255);
      }
      else if (percentage_state_epilepsy < 11.2)
      {
        color = rgb_to_hex(255, 0, 0);
      }
      else
      {
        color = rgb_to_hex(128, 0, 0);
      }

      size_t size_geometry = feature.m_geometry.size();
      for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
      {
        geometry_t geometry = feature.m_geometry.at(idx_geo);
        size_t size_pol = geometry.m_polygons.size();

        for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
        {
          polygon_t polygon = geometry.m_polygons[idx_pol];
          size_t size_crd = polygon.m_coord.size();

          if (size_crd == 0)
          {
            continue;
          }

          ///////////////////////////////////////////////////////////////////////////////////////
          //render each polygon as a vector of vertices passed to Polygon
          ///////////////////////////////////////////////////////////////////////////////////////

          std::vector<double> lat;
          std::vector<double> lon;

          for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
          {
            lat.push_back(polygon.m_coord[idx_crd].m_lat);
            lon.push_back(polygon.m_coord[idx_crd].m_lon);
          }

          if (geometry.m_type.compare("Point") == 0)
          {
            leaflet->Circle(lat[0], lon[0], color);
          }
          else if (geometry.m_type.compare("Polygon") == 0 ||
            geometry.m_type.compare("MultiPolygon") == 0)
          {
            leaflet->Polygon(lat, lon, color);
          }

        }  //idx_pol
      } //idx_geo
    } //idx_fet

    root()->addWidget(std::move(leaflet));
  }
};


///////////////////////////////////////////////////////////////////////////////////////
//Application_schools
///////////////////////////////////////////////////////////////////////////////////////

class Application_schools : public WApplication
{
public:
  Application_schools(const WEnvironment& env) : WApplication(env)
  {
    setTitle("geojson");
    std::unique_ptr<WLeaflet> leaflet =
      cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 39.0443047898, -77.1731281364, 11);

    marker_icon_t marker_green(
      "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-green.png",
      "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png",
      icon_size_t(25, 41),
      icon_size_t(12, 41),
      icon_size_t(1, -34),
      icon_size_t(41, 41));

    marker_icon_t marker_red(
      "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-red.png",
      "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png",
      icon_size_t(25, 41),
      icon_size_t(12, 41),
      icon_size_t(1, -34),
      icon_size_t(41, 41));

    ///////////////////////////////////////////////////////////////////////////////////////
    //render boundary
    ///////////////////////////////////////////////////////////////////////////////////////

    assert(lat_montgomery.size() == lon_montgomery.size());
    std::string color = rgb_to_hex(128, 128, 0);
    leaflet->Polygon(lat_montgomery, lon_montgomery, color);

    ///////////////////////////////////////////////////////////////////////////////////////
    //render geojson (ZIP)
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size_features = geojson.m_feature.size();
    for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
    {
      feature_t feature = geojson.m_feature.at(idx_fet);

      std::string color = rgb_to_hex(255, 255, 255);

      if (feature.m_name.compare("20850") == 0)
      {
        color = rgb_to_hex(0, 255, 0);
      }
      else if (feature.m_name.compare("20851") == 0)
      {
        color = rgb_to_hex(255, 255, 0);
      }
      else if (feature.m_name.compare("20852") == 0)
      {
        color = rgb_to_hex(0, 0, 255);
      }
      else if (feature.m_name.compare("20878") == 0)
      {
        color = rgb_to_hex(0, 255, 255);
      }
      else
      {
        continue;
      }

      size_t size_geometry = feature.m_geometry.size();
      for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
      {
        geometry_t geometry = feature.m_geometry.at(idx_geo);
        size_t size_pol = geometry.m_polygons.size();

        for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
        {
          polygon_t polygon = geometry.m_polygons[idx_pol];
          size_t size_crd = polygon.m_coord.size();

          if (size_crd == 0)
          {
            continue;
          }

          ///////////////////////////////////////////////////////////////////////////////////////
          //render each polygon as a vector of vertices passed to Polygon
          ///////////////////////////////////////////////////////////////////////////////////////

          std::vector<double> lat;
          std::vector<double> lon;

          for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
          {
            lat.push_back(polygon.m_coord[idx_crd].m_lat);
            lon.push_back(polygon.m_coord[idx_crd].m_lon);
          }

          leaflet->Polygon(lat, lon, color);
        }  //idx_pol
      } //idx_geo
    } //idx_fet



    ///////////////////////////////////////////////////////////////////////////////////////
    //render WMATA stations
    ///////////////////////////////////////////////////////////////////////////////////////

    std::string  color_red = rgb_to_hex(255, 0, 0);
    std::string  color_green = rgb_to_hex(0, 255, 0);
    std::string  color_blue = rgb_to_hex(0, 0, 255);
    std::string  color_orange = rgb_to_hex(255, 165, 0);
    std::string  color_yellow = rgb_to_hex(255, 255, 0);
    std::string  color_silver = rgb_to_hex(211, 211, 211);

    size_t size_stations = wmata_station.size();
    for (size_t idx = 0; idx < size_stations; idx++)
    {
      leaflet->Circle(wmata_station.at(idx).lat, wmata_station.at(idx).lon, color_red);
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //render schools
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size_schools = schools_list.size();
    for (size_t idx = 0; idx < size_schools; idx++)
    {
      school_t data = schools_list.at(idx);

      std::string text(data.name);
      text += " ";
      text += std::to_string(data.rating);

      if (data.rating <= 4) //3, 4
      {
        leaflet->Marker(data.lat, data.lon, text, marker_red);
      }
      else if (data.rating <= 6) // 5, 6
      {
        leaflet->Marker(data.lat, data.lon, text);
      }
      else if (data.rating <= 10) // 7, 8, 9, 10
      {
        leaflet->Marker(data.lat, data.lon, text, marker_green);
      }
      else
      {
        assert(0);
      }
    }

    root()->addWidget(std::move(leaflet));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_topojson
///////////////////////////////////////////////////////////////////////////////////////

class Application_topojson : public WApplication
{
public:
  Application_topojson(const WEnvironment& env) : WApplication(env)
  {
    setTitle("topojson sample");

    ///////////////////////////////////////////////////////////////////////////////////////
    //render topojson
    ///////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //must make the coordinates for the topology first
    //3 objects: counties (0), states (1), land (2) 
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t topology_index = 0;
    topojson.make_coordinates(topology_index);
    topology_object_t topology = topojson.m_topology.at(topology_index);

    size_t size_geom = topology.m_geom.size();

    std::vector<double> first = topojson.get_first();
    int ifirst[2];
    ifirst[0] = (int)first[0];
    ifirst[1] = (int)first[1];
    std::vector<double> center = topojson.transform_point(ifirst);

    std::unique_ptr<WLeaflet> leaflet =
      cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, center[1], center[0], 6);

    ///////////////////////////////////////////////////////////////////////////////////////
    //render topojson
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t idx_pal = 0;
    for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
    {
      Geometry_t geometry = topology.m_geom.at(idx_geom);
      if (geometry.type.compare("Polygon") == 0 || geometry.type.compare("MultiPolygon") == 0)
      {
        size_t size_pol = geometry.m_polygon.size();
        for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
        {
          Polygon_topojson_t polygon = geometry.m_polygon.at(idx_pol);
          size_t size_arcs = polygon.arcs.size();

          ///////////////////////////////////////////////////////////////////////////////////////
          //render each polygon as a vector of vertices passed to Polygon
          ///////////////////////////////////////////////////////////////////////////////////////

          std::vector<double> lat;
          std::vector<double> lon;

          size_t size_points = geometry.m_polygon.at(idx_pol).m_y.size();
          for (size_t idx_crd = 0; idx_crd < size_points; idx_crd++)
          {
            lat.push_back(geometry.m_polygon.at(idx_pol).m_y.at(idx_crd));
            lon.push_back(geometry.m_polygon.at(idx_pol).m_x.at(idx_crd));
          }

          std::string color = rgb_to_hex(
            rgb_256.at(idx_pal).red,
            rgb_256.at(idx_pal).green,
            rgb_256.at(idx_pal).blue);
          idx_pal++;
          if (idx_pal >= rgb_256.size())
          {
            idx_pal = 0;
          }

          if (geometry.type.compare("Polygon") == 0)
          {
            color = rgb_to_hex(255, 0, 0);
          }
          else if (geometry.type.compare("MultiPolygon") == 0)
          {
            color = rgb_to_hex(0, 255, 0);
          }
          leaflet->Polygon(lat, lon, color);

        }//size_pol
      }//"Polygon"
    }//size_geom

    root()->addWidget(std::move(leaflet));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_celsium_dc311
///////////////////////////////////////////////////////////////////////////////////////

class Application_celsium_dc311 : public WApplication
{
public:
  Application_celsium_dc311(const WEnvironment& env) : WApplication(env)
  {
    setTitle("Celsium DC311 rodent complaints");
    std::string js;

    ///////////////////////////////////////////////////////////////////////////////////////
    //render CSV points from DC_311 database
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size = dc311_data.size();

    //DC coordinates
    js += "var lon = -77.0369;";
    js += "var lat = 38.9072;";
    js += "var center = Cesium.Cartesian3.fromDegrees(lon, lat);";
    js += "viewer.camera.lookAt(center, new Cesium.Cartesian3(0.0, 0.0, 12000.0)); ";

    js += "var scene = viewer.scene;";
    js += "var instances = [];";

    for (size_t idx = 0; idx < size / 100; idx++)
    {
      dc311_data_t dc = dc311_data.at(idx);

      js += "instances.push(new Cesium.GeometryInstance({";
      js += "  geometry : new Cesium.CircleGeometry({";
      js += "    center : Cesium.Cartesian3.fromDegrees(";
      std::string str_ll;
      str_ll = dc.lon;
      str_ll += ",";
      str_ll += dc.lat;
      js += str_ll;
      js += "     ),";
      js += "    radius : 100.0,";
      js += "    vertexFormat: Cesium.PerInstanceColorAppearance.VERTEX_FORMAT";
      js += "   }),"; //geometry
      js += "  attributes : {";
      js += "    color : new Cesium.ColorGeometryInstanceAttribute(1.0, 0.0, 0.0, 0.3)";
      js += "  }";//attributes
      js += "}));";//push

      js += "scene.primitives.add(new Cesium.Primitive({";
      js += "  geometryInstances : instances,";
      js += "  appearance : new Cesium.PerInstanceColorAppearance()";
      js += "}));";//add
    }

    std::unique_ptr<WCesium> celsium = cpp14::make_unique<WCesium>(js);
    root()->addWidget(std::move(celsium));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_celsium_atms
///////////////////////////////////////////////////////////////////////////////////////

std::string str_lon_lat(double lon, double lat)
{
  std::string str;
  str = std::to_string(lon);
  str += ",";
  str += std::to_string(lat);
  return str;
}

class Application_celsium_atms : public WApplication
{
public:
  Application_celsium_atms(const WEnvironment& env) : WApplication(env)
  {
    setTitle("Cesium NASA HDF5");
    std::string js;

    star_dataset_t temperature = find_dataset("AntennaTemperature");
    star_dataset_t latitude = find_dataset("latitude");
    star_dataset_t longitude = find_dataset("longitude");
    size_t nbr_rows = temperature.m_shape[0];
    size_t nbr_cols = temperature.m_shape[1];
    size_t nbr_lev = temperature.m_shape[2];
    double temp_min;
    double temp_max;
    double lat_min;
    double lat_max;
    double lon_min;
    double lon_max;
    temperature.do_min_max(temp_min, temp_max);
    latitude.do_min_max(lat_min, lat_max);
    longitude.do_min_max(lon_min, lon_max);
    double lat = (lat_max + lat_min) / 2;
    double lon = (lon_max + lon_min) / 2;

    js += "var lon = ";
    js += std::to_string(lon);
    js += ";";
    js += "var lat = ";
    js += std::to_string(lat);
    js += ";";
    js += "var center = Cesium.Cartesian3.fromDegrees(lon, lat);";
    js += "viewer.camera.lookAt(center, new Cesium.Cartesian3(0.0, 0.0, 4000000.0)); ";

    js += "var scene = viewer.scene;";
    js += "var instances = [];";

    ///////////////////////////////////////////////////////////////////////////////////////
    //render data
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t idx_channel = 0;

    for (size_t idx_row = 0; idx_row < nbr_rows - 1; idx_row++)
    {
      for (size_t idx_col = 0; idx_col < nbr_cols - 1; idx_col++)
      {
        ///////////////////////////////////////////////////////////////////////////
        //polygon coordinates for a granule
        ///////////////////////////////////////////////////////////////////////////

        std::vector<double> vlat(4);
        std::vector<double> vlon(4);
        vlat[0] = latitude.value_at(idx_row, idx_col);
        vlon[0] = longitude.value_at(idx_row, idx_col);
        vlat[1] = latitude.value_at(idx_row, idx_col + 1);
        vlon[1] = longitude.value_at(idx_row, idx_col + 1);
        vlat[2] = latitude.value_at(idx_row + 1, idx_col + 1);
        vlon[2] = longitude.value_at(idx_row + 1, idx_col + 1);
        vlat[3] = latitude.value_at(idx_row + 1, idx_col);
        vlon[3] = longitude.value_at(idx_row + 1, idx_col);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //color
        //The Vizualization Toolkit, pg.156
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        size_t nbr_palette_entries = rgb_256.size();
        double value_range = temp_max - temp_min;
        double value_min = temp_min;
        double value = temperature.value_at(idx_row, idx_col, idx_channel);
        size_t idx_pal = (size_t)(nbr_palette_entries * ((value - value_min) / value_range));
        idx_pal = (idx_pal < 0 ? 0 : (idx_pal >= nbr_palette_entries ? nbr_palette_entries - 1 : idx_pal));

        double r = rgb_256.at(idx_pal).red / 255.0;
        double g = rgb_256.at(idx_pal).green / 255.0;
        double b = rgb_256.at(idx_pal).blue / 255.0;

        std::string clr = std::to_string(r); clr += ",";
        clr += std::to_string(g); clr += ",";
        clr += std::to_string(b); clr += ",";
        clr += "0.1"; //alpha

        js += "instances.push(new Cesium.GeometryInstance({";
        js += "  geometry : new Cesium.PolygonGeometry({";
        js += "    polygonHierarchy : new Cesium.PolygonHierarchy(";
        js += "     Cesium.Cartesian3.fromDegreesArray([";

        js += str_lon_lat(vlon[0], vlat[0]); js += ",";
        js += str_lon_lat(vlon[1], vlat[1]); js += ",";
        js += str_lon_lat(vlon[2], vlat[2]); js += ",";
        js += str_lon_lat(vlon[3], vlat[3]);

        js += "     ])";//array
        js += "     ),";//PolygonHierarchy
        js += "    vertexFormat: Cesium.PerInstanceColorAppearance.VERTEX_FORMAT";
        js += "   }),"; //PolygonGeometry
        js += "  attributes : {";
        js += "    color : new Cesium.ColorGeometryInstanceAttribute(";
        js += clr;
        js += "    )";
        js += "  }";//attributes
        js += "}));";//push

        js += "scene.primitives.add(new Cesium.Primitive({";
        js += "  geometryInstances : instances,";
        js += "  appearance : new Cesium.PerInstanceColorAppearance()";
        js += "}));";//add
      }
    }

    std::unique_ptr<WCesium> celsium = cpp14::make_unique<WCesium>(js);
    root()->addWidget(std::move(celsium));
  }
};

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

    std::unique_ptr<WPlotly> plotly = cpp14::make_unique<WPlotly>(js);
    root()->addWidget(Wt::cpp14::make_unique<Wt::WText>("Plot"));
    root()->addWidget(std::move(plotly));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//PushWidget
///////////////////////////////////////////////////////////////////////////////////////

class PushWidget : public WContainerWidget
{
public:
  PushWidget() :
    WContainerWidget(),
    m_iter(0)
  {
    m_text = this->addWidget(cpp14::make_unique<WText>(Wt::asString(m_iter)));
    m_timer = cpp14::make_unique<WTimer>();
    m_timer->setInterval(std::chrono::milliseconds{ 1000 });
    m_timer->timeout().connect(this, &PushWidget::tick);
    m_timer->start();
  }

private:
  std::unique_ptr<WTimer> m_timer;
  WText *m_text;
  size_t m_iter;

  void tick()
  {
    m_timer->stop();
    WApplication *app = WApplication::instance();
    app->enableUpdates(true);
    while (true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      WApplication::UpdateLock uiLock(app);
      if (uiLock)
      {
        m_iter++;
        m_text->setText(Wt::asString(m_iter));
        app->triggerUpdate();
        app->processEvents();
      }
      else
      {
        return;
      }
    }
  }
};

//////////////////////////////////////////////////////////////////////////////////////
//Application_update
///////////////////////////////////////////////////////////////////////////////////////

class Application_update : public WApplication
{
public:
  Application_update(const WEnvironment& env) : WApplication(env)
  {
    std::unique_ptr<PushWidget> update = cpp14::make_unique<PushWidget>();
    root()->addWidget(std::move(update));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//create_application
///////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<WApplication> create_application(const WEnvironment& env)
{
  if (test.compare("1") == 0)
  {
    return cpp14::make_unique<Application_test>(env);
  }
  else if (test.compare("2") == 0)
  {
    return cpp14::make_unique<Application_dc311>(env);
  }
  else if (test.compare("3") == 0)
  {
    return cpp14::make_unique<Application_us_states>(env);
  }
  else if (test.compare("4") == 0)
  {
    return cpp14::make_unique<Application_atms>(env);
  }
  else if (test.compare("5") == 0)
  {
    return cpp14::make_unique<Application_ep>(env);
  }
  else if (test.compare("6") == 0)
  {
    return cpp14::make_unique<Application_schools>(env);
  }
  else if (test.compare("7") == 0)
  {
    return cpp14::make_unique<Application_topojson>(env);
  }
  else if (test.compare("8") == 0)
  {
    return cpp14::make_unique<Application_celsium_dc311>(env);
  }
  else if (test.compare("9") == 0)
  {
    return cpp14::make_unique<Application_celsium_atms>(env);
  }
  else if (test.compare("10") == 0)
  {
    return cpp14::make_unique<Application_plotly>(env);
  }
  else if (test.compare("11") == 0)
  {
    return cpp14::make_unique<Application_update>(env);
  }
  assert(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//usage
/////////////////////////////////////////////////////////////////////////////////////////////////////

void usage()
{
  std::cout << "usage: ./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. ";
  std::cout << "-t TEST <-d DATABASE> <-u DATABASE> <-g GEOJSON> <-z GEOJSON> ";
  std::cout << std::endl;
  std::cout << "-t TEST: test number (1 to 6)" << std::endl;
  std::cout << "-d DATABASE: data file" << std::endl;
  std::cout << "-g GEOJSON: geojson file" << std::endl;
  std::cout << "-u DATABASE: data file" << std::endl;
  std::cout << "-z GEOJSON: data file" << std::endl;
  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  std::string data_file;
  std::string data_file_us_states_pop;
  std::string geojson_file;
  std::string zip_geojson_file;

  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 't':
        test = argv[i + 1];
        i++;
        break;
      case 'd':
        data_file = argv[i + 1];
        i++;
        break;
      case 'u':
        data_file_us_states_pop = argv[i + 1];
        i++;
        break;
      case 'g':
        geojson_file = argv[i + 1];
        i++;
        break;
      case 'm':
        file_wmata_stations = argv[i + 1];
        i++;
        break;
      case 'z':
        zip_geojson_file = argv[i + 1];
        i++;
        break;
      }
    }
  }

  if (test.empty())
  {
    usage();
  }

  if (test.compare("1") == 0)
  {

  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //dc_311 test, read CSV data file with -d, geojson with -g 
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("2") == 0)
  {
    if (data_file.empty())
    {
      usage();
    }
    if (read_dc311(data_file) < 0)
    {
      assert(0);
      exit(1);
    }
    if (geojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //geojson test, read geojson file with -f 
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("3") == 0)
  {
    if (geojson_file.empty())
    {
      usage();
      exit(1);
    }
    if (geojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //NOAA ATMS star json file
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("4") == 0)
  {
    if (data_file.empty())
    {
      usage();
      exit(1);
    }
    if (read_datasets(data_file.c_str(), datasets) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //ep
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("5") == 0)
  {
    if (data_file.empty() || data_file_us_states_pop.empty() || geojson_file.empty())
    {
      usage();
      exit(1);
    }

    std::cout << data_file << std::endl;
    std::cout << data_file_us_states_pop << std::endl;
    std::cout << geojson_file << std::endl;

    if (data_file.empty() || geojson_file.empty())
    {
      usage();
      exit(1);
    }
    if (read_ep_pop(data_file, data_file_us_states_pop) < 0)
    {
      assert(0);
    }
    if (geojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////////
  //schools
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("6") == 0)
  {
    std::cout << data_file << std::endl;
    std::cout << geojson_file << std::endl;
    std::cout << zip_geojson_file << std::endl;
    if (read_schools(data_file) < 0)
    {
      assert(0);
      exit(1);
    }
    if (read_json_montgomery_county(geojson_file.c_str(), lat_montgomery, lon_montgomery) < 0)
    {
      assert(0);
      exit(1);
    }
    if (read_json_wmata(file_wmata_stations, lat_wmata, lon_wmata) < 0)
    {
      assert(0);
      exit(1);
    }
    if (geojson.convert(zip_geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //topojson sample
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("7") == 0)
  {
    std::cout << geojson_file << std::endl;
    if (topojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //celsium
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("8") == 0)
  {
    std::cout << data_file << std::endl;
    if (read_dc311(data_file) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  else if (test.compare("9") == 0)
  {
    std::cout << data_file << std::endl;
    if (data_file.empty())
    {
      usage();
      exit(1);
    }
    if (read_datasets(data_file.c_str(), datasets) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  else if (test.compare("10") == 0)
  {
    std::cout << data_file << std::endl;
    file_plotly = data_file;
  }
  else if (test.compare("11") == 0)
  {

  }
  else
  {
    assert(0);
    exit(1);
  }

  for (size_t idx = 0; idx < 3 * 256; idx += 3)
  {
    unsigned char r = pal_rgb[idx];
    unsigned char g = pal_rgb[idx + 1];
    unsigned char b = pal_rgb[idx + 2];
    rgb_256.push_back(rgb_t(r, g, b));
  }

  return WRun(argc, argv, &create_application);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_dc311
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_dc311(const std::string &file_name)
{
  read_csv_t csv;

  if (csv.open(file_name) < 0)
  {
    std::cout << "Cannot open file " << file_name.c_str() << std::endl;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //iterate until an empty row is returned (end of file)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t rows = 0;
  std::vector<std::string> row;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    dc311_data_t data(row.at(0), row.at(1), row.at(2), row.at(3));
    dc311_data.push_back(data);
  }

  std::cout << "Processed " << rows << " rows" << std::endl;
  return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_ep
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_ep_pop(const std::string &file_name_ep, const std::string &file_name_pop)
{
  read_csv_t csv;

  if (csv.open(file_name_ep) < 0)
  {
    std::cout << "Cannot open file " << file_name_ep.c_str() << std::endl;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //iterate until an empty row is returned (end of file)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t rows = 0;
  std::vector<std::string> row;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    std::string eps = row.at(1);
    size_t pos = eps.find("(");
    eps = eps.substr(0, pos);
    pos = eps.find(",");
    std::string eps1 = eps.substr(0, pos);
    std::string eps2 = eps.substr(pos + 1);
    eps = eps1 + eps2;
    ep_data_t data(row.at(0), std::stoul(eps));
    ep_data.push_back(data);
    std::cout << ep_data.back().state << " " << ep_data.back().ep << "\t";
  }

  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";

  csv.m_ifs.close();

  if (csv.open(file_name_pop) < 0)
  {
    std::cout << "Cannot open file " << file_name_pop.c_str() << std::endl;
    return -1;
  }

  rows = 0;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    std::string state = row.at(0);
    std::string pop = row.at(1);
    us_state_pop_t state_pop(state, std::stoul(pop));
    us_state_pop.push_back(state_pop);
    std::cout << us_state_pop.back().state << " " << us_state_pop.back().population << "\t";

  }
  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";

  csv.m_ifs.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_hex
//convert int to hex string, apply zero padding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n)
{
  std::stringstream ss;
  ss << std::hex << n;
  std::string str(ss.str());
  return str.size() == 1 ? "0" + str : str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_to_hex
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string rgb_to_hex(int r, int g, int b)
{
  std::string str("#");
  str += to_hex(r);
  str += to_hex(g);
  str += to_hex(b);
  return str;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_dataset
/////////////////////////////////////////////////////////////////////////////////////////////////////

star_dataset_t find_dataset(std::string name)
{
  for (size_t idx = 0; idx < datasets.size(); idx++)
  {
    if (datasets.at(idx).m_name.compare(name) == 0)
    {
      return datasets.at(idx);
    }
  }
  assert(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_ep
//by state name
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t find_ep(std::string state_name)
{
  for (size_t idx = 0; idx < ep_data.size(); idx++)
  {
    std::string state = ep_data.at(idx).state;
    if (state.find(state_name) == 0)
    {
      return ep_data.at(idx).ep;
    }
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_population
//by state name
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t find_population(std::string state_name)
{
  for (size_t idx = 0; idx < us_state_pop.size(); idx++)
  {
    std::string state = us_state_pop.at(idx).state;
    if (state.find(state_name) == 0)
    {
      return us_state_pop.at(idx).population;
    }
  }
  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_schools
//CATEGORY,SCHOOL NAME,ADDRESS,CITY,ZIP CODE,PHONE,URL,LONGITUDE,LATITUDE,LOCATION
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_schools(const std::string &file_name)
{
  read_csv_t csv;

  if (csv.open(file_name) < 0)
  {
    std::cout << "Cannot open file " << file_name.c_str() << std::endl;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //iterate until an empty row is returned (end of file)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> row;

  //read header
  row = csv.read_row();
  size_t rows = 0;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    std::string name = row.at(1);
    std::string lon = row.at(7);
    std::string lat = row.at(8);
    std::string rating = row.at(10);
    schools_list.push_back(school_t(name, lat, lon, std::stoi(rating)));
  }

  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";
  csv.m_ifs.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_json_montgomery_county
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_json_montgomery_county(const std::string &file_name, std::vector<double> &lat, std::vector<double> &lon)
{
  char *buf = 0;
  size_t length;
  FILE *f;
  std::string polygon_data;

  std::cout << file_name << std::endl;
  f = fopen(file_name.c_str(), "rb");
  if (!f)
  {
    std::cout << "cannot open " << file_name << std::endl;
    assert(0);
    return -1;
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = (char*)malloc(length);
  if (buf)
  {
    if (fread(buf, 1, length, f) < 0)
    {
    }
  }
  fclose(f);

  char *endptr;
  JsonValue value;
  JsonAllocator allocator;
  int rc = jsonParse(buf, &endptr, &value, allocator);
  if (rc != JSON_OK)
  {
    std::cout << "invalid JSON format for " << buf << std::endl;
    return -1;
  }

  size_t arr_size = 0; //size of array
  for (JsonNode *node_root = value.toNode(); node_root != nullptr; node_root = node_root->next)
  {
    std::cout << node_root->key << std::endl;
    if (std::string(node_root->key).compare("data") == 0)
    {
      assert(node_root->value.getTag() == JSON_ARRAY);
      JsonValue value_data = node_root->value;
      for (JsonNode *node_data = value_data.toNode(); node_data != nullptr; node_data = node_data->next)
      {
        JsonValue value_arr = node_data->value;
        for (JsonNode *node_arr = value_arr.toNode(); node_arr != nullptr; node_arr = node_arr->next)
        {
          //"MULTIPOLYGON" string at index 8
          if (arr_size == 8)
          {
            assert(node_arr->value.getTag() == JSON_STRING);
            polygon_data = node_arr->value.toString();
          }
          arr_size++;
        }


      }//node_data
    }//"data"
  }//node_root

  size_t start = polygon_data.find("(((");
  size_t end = polygon_data.find(")))");
  start += 3;
  end += 3;
  size_t len = end - start + 1;
  std::string str = polygon_data.substr(start, len);
  size_t pos_comma = 0;
  while (true)
  {
    start = pos_comma;
    pos_comma = str.find(",", pos_comma);
    size_t len_lat_lon = pos_comma - start; //skip ","
    if (pos_comma == std::string::npos)
    {
      break;
    }
    std::string lat_lon = str.substr(start, len_lat_lon);
    size_t pos_space = lat_lon.find(" ");
    std::string str_lon = lat_lon.substr(0, pos_space);
    std::string str_lat = lat_lon.substr(pos_space + 1);
    double dlat = std::stod(str_lat);
    double dlon = std::stod(str_lon);
    lat.push_back(dlat);
    lon.push_back(dlon);
    pos_comma += 2; //skip ", " comma and space
  }

  free(buf);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_json_wmata
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_json_wmata(const std::string &file_name, std::vector<double> &lat, std::vector<double> &lon)
{
  char *buf = 0;
  size_t length;
  FILE *f;

  std::cout << file_name << std::endl;
  f = fopen(file_name.c_str(), "rb");
  if (!f)
  {
    std::cout << "cannot open " << file_name << std::endl;
    assert(0);
    return -1;
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = (char*)malloc(length);
  if (buf)
  {
    if (fread(buf, 1, length, f) < 0)
    {
    }
  }
  fclose(f);

  char *endptr;
  JsonValue value;
  JsonAllocator allocator;
  int rc = jsonParse(buf, &endptr, &value, allocator);
  if (rc != JSON_OK)
  {
    std::cout << "invalid JSON format for " << buf << std::endl;
    return -1;
  }

  size_t arr_size = 0; //size of array
  for (JsonNode *node_root = value.toNode(); node_root != nullptr; node_root = node_root->next)
  {
    std::cout << node_root->key << std::endl;
    if (std::string(node_root->key).compare("Stations") == 0)
    {
      JsonValue value_stations = node_root->value;
      assert(value_stations.getTag() == JSON_ARRAY);
      for (JsonNode *node_arr = value_stations.toNode(); node_arr != nullptr; node_arr = node_arr->next)
      {
        arr_size++;
        JsonValue value_obj_station = node_arr->value;
        assert(value_obj_station.getTag() == JSON_OBJECT);

        double lat;
        double lon;
        std::string name;
        std::string line_code;

        for (JsonNode *node_obj = value_obj_station.toNode(); node_obj != nullptr; node_obj = node_obj->next)
        {
          if (std::string(node_obj->key).compare("Lat") == 0)
          {
            assert(node_obj->value.getTag() == JSON_NUMBER);
            lat = node_obj->value.toNumber();
          }
          else if (std::string(node_obj->key).compare("Lon") == 0)
          {
            assert(node_obj->value.getTag() == JSON_NUMBER);
            lon = node_obj->value.toNumber();
          }
          else if (std::string(node_obj->key).compare("LineCode1") == 0)
          {
            assert(node_obj->value.getTag() == JSON_STRING);
            line_code = node_obj->value.toString();
          }
          else if (std::string(node_obj->key).compare("Name") == 0)
          {
            assert(node_obj->value.getTag() == JSON_STRING);
            name = node_obj->value.toString();
            std::cout << name << " ";
          }
        } //node_obj

        wmata_station_t station(std::to_string(lat), std::to_string(lon), name, line_code);
        wmata_station.push_back(station);

      } //node_arr
    } //"Stations"
  } //node_root
  std::cout << "\n";

  free(buf);
  return 0;
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
  return 0;
}