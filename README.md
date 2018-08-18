# Wt.extensions

[Wt](https://www.webtoolkit.eu/wt) is a C++ library for developing web applications. 

Wt.extensions is a fork (branch extensions) that adds use of several popular Javascript libraries:

[Leaflet](http://leafletjs.com/)

![image](https://user-images.githubusercontent.com/6119070/44011785-916e3b40-9e88-11e8-938d-d835da2690da.png)

[Plotly.js](https://plot.ly/javascript/)

![image](https://user-images.githubusercontent.com/6119070/44011812-c712a588-9e88-11e8-8e67-472a0663225d.png)

[Cesium](https://cesiumjs.org/)

![image](https://user-images.githubusercontent.com/6119070/44011818-d6f8543e-9e88-11e8-9995-6a1ad707e005.png)

# Examples

To run all the examples, this same set of parameters is used, as typical for any Wt application

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.

In addition to these parameters, each example has a set of extra parameters, to load specific data to the example. The parameters for these examples are of the form

-t TEST: test number (1 to 9)

-d DATABASE: data file

-g GEOJSON: geojson file

-u DATABASE: data file

-z GEOJSON: data file

All examples run in a browser in port 8080

http://127.0.0.1:8080/

## Washington DC 311 database (Leaflet)

Washington [DC311](https://311.dc.gov/) code (rodent complaints) occurrences for year 2016. The circle has a radius of 100 meters

### demo

http://www.eden-earth.org:8082/

![image](https://user-images.githubusercontent.com/6119070/43999560-a7e11336-9ddc-11e8-9319-5bc278b19d5b.png)

### run

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.
-t 2 -d ../../../examples/test_extensions/data/dc_311-2016.csv.s0311.csv -g ../../../examples/test_extensions/data/ward-2012.geojson


## Montgomery County schools (Leaflet)

This example shows Montgomery County, MD, schools, with a color rating. The red circles are Metro Read line stations.
The county boundaries is read from .geoJSON files and shown as polygons.

### demo

http://www.eden-earth.org:8086/

![image](https://user-images.githubusercontent.com/6119070/44007562-1102ae0e-9e66-11e8-8b0a-b2da71e7b83a.png)

### run

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.
-t 6 -d ../../../examples/test_extensions/montgomery_county_schools.csv
-g ../../../examples/test_extensions/montgomery_county_boundary.json 
-m ../../../examples/test_extensions/wmata_stations.json
-z ../../../examples/test_extensions/md_maryland_zip_codes_geo.min.json

## US states (Leaflet)

This example shows US states, with random colors. The topograghy is read from .geoJSON files.

### demo

http://www.eden-earth.org:8083/

![image](https://user-images.githubusercontent.com/6119070/44009572-eb420a2e-9e7a-11e8-83f8-9ef85d6bea71.png)

### run

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.
-t 3 -g ../../../examples/test_extensions/data/gz_2010_us_040_00_20m.json

## NASA ATMS HDF5 satellite data (Cesium)

The Advanced Technology Microwave Sounder (ATMS) is a NASA satellite that measures atmospheric temperature and moisture for weather forecasting, 
and its data is saved in HDF5.
https://jointmission.gsfc.nasa.gov/atms.html
HDF5 (Hierarchical Data Format) is a data format funded by NASA and its original use was to store NASA satellite data.
https://www.hdfgroup.org/
This example shows HDF5 ATMS data (converted to JSON)

### demo

http://www.eden-earth.org:8089/

![image](https://user-images.githubusercontent.com/6119070/43999577-fd5093fa-9ddc-11e8-9260-63967958197e.png)

### run

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.
-t 9 -d ../../../examples/test_extensions/data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json 

## Dow Jones chart(Plotly)

This example shows a Plotly.js chart using data from Dow Jones Industrial Average index, read from a CSV file.

### demo

http://www.eden-earth.org:8090/

![image](https://user-images.githubusercontent.com/6119070/44009396-921a8a4e-9e79-11e8-8141-99f855a0f769.png)

### run

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.
-t 10 -d ../../../examples/test_extensions/data/DJI_2018_minor.3600.txt

# Building Wt.extensions

## Install dependencies

[cmake](https://cmake.org/)

[boost](http://www.boost.org/)

### Linux Ubuntu

Install packages with

sudo apt-get install cmake

sudo apt-get install build-essential

sudo apt-get install python-dev

sudo apt-get install libboost-all-dev


### Mac OSX

Install Homebrew

ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

Install packages with

brew install cmake 

brew install boost 

## Clone and switch to branch extensions

git clone https://github.com/pedro-vicente/wt.git

git checkout extensions

cd build

cmake .. 

make

# Developer


### List of added files in /src

extensions/WLeaflet.cc
extensions/WLeaflet.hh
extensions/WPlotly.cc
extensions/WPlotly.hh
extensions/WCelsium.cc
extensions/WCelsium.hh
extensions/geojson.hh
extensions/geojson.cc
extensions/topojson.hh
extensions/topojson.cc
extensions/csv.hh
extensions/csv.cc
extensions/star_json.cc
extensions/star_json.hh
extensions/star_dataset.cc
extensions/star_dataset.hh
3rdparty/gason/src/gason.h
3rdparty/gason/src/gason.cpp

### Extensions example

examples/test_extensions

# APIs

## Usage of the Leaflet Wt class

The API supports 2 map tile providers: CartoDB and RRZE Openstreetmap-Server

[cartoDB](https://carto.com/)

[RRZE Openstreetmap-Server](https://osm.rrze.fau.de/)

```c++
enum class tile_provider_t
{
  CARTODB, RRZE
};
```

Example of a map of Washington DC using CartoDB tiles

```c++
class MapApplication : public WApplication
{
public:
  MapApplication(const WEnvironment& env) : WApplication(env)
  {
    std::unique_ptr<WLeaflet> leaflet = cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 38.9072 -77.0369, 13);
    root()->addWidget(std::move(leaflet));
  }
};
```

## API
```c++
void Circle(const std::string &lat, const std::string &lon);
void Polygon(const std::vector<double> &lat, const std::vector<double> &lon);
```


