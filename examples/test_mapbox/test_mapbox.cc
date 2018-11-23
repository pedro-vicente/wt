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
#include <Wt/WProgressBar.h>
#include <Wt/WTimer.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include "extensions/WLeaflet.hh"
using namespace Wt;

marker_icon_t marker_violet(
  "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-2x-violet.png",
  "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png",
  icon_size_t(25, 41),
  icon_size_t(12, 41),
  icon_size_t(1, -34),
  icon_size_t(41, 41));

///////////////////////////////////////////////////////////////////////////////////////
//MapApplication
//./leaflet_mapbox.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. -c wt_config.xml
//38.9072, -77.0369, 14 DC
//[37.0902, -95.7129], 5 US
///////////////////////////////////////////////////////////////////////////////////////

class MapApplication : public WApplication
{
public:
  MapApplication(const WEnvironment& env) :
    WApplication(env)
  {
    WVBoxLayout *hbox;
    WText *text;
    hbox = root()->setLayout(cpp14::make_unique<WVBoxLayout>());
    text = hbox->addWidget(cpp14::make_unique<WText>(Wt::asString(0)));
    leaflet = hbox->addWidget(cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13, false));
    leaflet->Circle(38.9072, -77.0369, 100, "#ff0000");
    leaflet->Marker(38.9072, -77.0369, "test", marker_violet);
    leaflet->clicked().connect([=](WLeaflet::Coordinate c)
    {
      this->map_click(c);
    });
  }

  void map_click(WLeaflet::Coordinate c)
  {
    std::cerr << "Clicked at coordinate (" << c.latitude() << "," << c.longitude() << ")";
    leaflet->Marker(c.latitude(), c.longitude(), "test", marker_violet);
    leaflet->Circle(c.latitude(), c.longitude(), 100, "#00ff00");
  }
  WLeaflet *leaflet;
};

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<WApplication> create_application(const WEnvironment& env)
{
  return cpp14::make_unique<MapApplication>(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &create_application);
}
