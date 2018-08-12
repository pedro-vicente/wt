# Wt.extensions

[Wt](https://www.webtoolkit.eu/wt) is a C++ library for developing web applications. 

Wt.extensions is a fork (branch extensions) that adds use of several popular Javascript libraries:

[Leaflet](http://leafletjs.com/)
[Plotly.js](https://plot.ly/javascript/)
[Cesium](https://cesiumjs.org/)

# Live demos

## Running

To run all the examples, this same set of parameters is used, as typical for any Wt application

./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.

In addition to these parameters, each example has a set of extra parameters, to load specific data to the example.
The parameters for these examples are of the form

-t TEST: test number (1 to 6)
-d DATABASE: data file
-g GEOJSON: geojson file
-u DATABASE: data file
-z GEOJSON: data file

All examples run in a browser in port 8080

http://127.0.0.1:8080/

## Washington DC 311 database (Leaflet)

Washington DC S0311 code (rodent complaints) [DC311](https://311.dc.gov/) occurrences for year 2016. The circle has a radius of 100 meters

http://www.eden-earth.org:8082/

![image](https://user-images.githubusercontent.com/6119070/43999560-a7e11336-9ddc-11e8-9319-5bc278b19d5b.png)


## Montgomery County schools (Leaflet)

## NOAA ATMS satellite data (Leaflet, Cesium)

![image](https://user-images.githubusercontent.com/6119070/43999577-fd5093fa-9ddc-11e8-9260-63967958197e.png)


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


