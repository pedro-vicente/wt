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
#include <thread>
#include <chrono>
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
  MapApplication(const WEnvironment& env) :
    WApplication(env),
    m_iter(300)
  {
    m_hbox = root()->setLayout(cpp14::make_unique<WVBoxLayout>());
    m_text = m_hbox->addWidget(cpp14::make_unique<WText>(Wt::asString(m_iter)));
    m_leaflet = m_hbox->addWidget(cpp14::make_unique<WMapbox>());
    m_leaflet->Circle(38.9072, -77.0369, m_iter, "#ff0000");

    m_timer = cpp14::make_unique<WTimer>();
    m_timer->setInterval(std::chrono::milliseconds{ 6000 });
    m_timer->timeout().connect(this, &MapApplication::tick);
    m_timer->start();
  }

private:
  WVBoxLayout *m_hbox;
  WText *m_text;
  WMapbox *m_leaflet;
  size_t m_iter;
  std::unique_ptr<WTimer> m_timer;

  void tick()
  {
    m_iter += 100;
    m_text->setText(Wt::asString(m_iter));
    m_leaflet->removeFromParent();
    m_leaflet = m_hbox->addWidget(cpp14::make_unique<WMapbox>());
    m_leaflet->Circle(38.9072, -77.0369, m_iter, "#ff0000");
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

