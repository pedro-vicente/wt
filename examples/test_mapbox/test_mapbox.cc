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
#include "extensions/WLeaflet.hh"
#include "extensions/WLeafletMapbox.hh"
using namespace Wt;

//run
//./leaflet_mapbox.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. -c wt_config.xml

///////////////////////////////////////////////////////////////////////////////////////
//MapApplication
//38.9072, -77.0369, 14 DC
//[37.0902, -95.7129], 5 US
///////////////////////////////////////////////////////////////////////////////////////

class MapApplication : public WApplication
{
public:
  MapApplication(const WEnvironment& env) : WApplication(env)
  {
    auto hbox = root()->setLayout(cpp14::make_unique<WVBoxLayout>());
    std::unique_ptr<WText> text = cpp14::make_unique<WText>("item 1");
    hbox->addWidget(std::move(text));
    std::unique_ptr<WMapbox> leaflet = cpp14::make_unique<WMapbox>();
    hbox->addWidget(std::move(leaflet));
  }
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

