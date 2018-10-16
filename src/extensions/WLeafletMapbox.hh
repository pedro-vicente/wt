#ifndef WMAPBOX_H_
#define WMAPBOX_H_

#include <Wt/WColor.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WSignal.h>
#include <vector>
#include <string>

namespace Wt
{
  ///////////////////////////////////////////////////////////////////////////////////////
  //WMapBox
  ///////////////////////////////////////////////////////////////////////////////////////

  class WT_API WMapbox : public WCompositeWidget
  {
  public:
    WMapbox();
    void Circle(const std::string &lat, const std::string &lon);

  protected:
    virtual void render(WFlags<RenderFlag> flags);
    std::vector<std::string> m_additions;
  };

} //namespace Wt

#endif // WMAPBOX_H_
