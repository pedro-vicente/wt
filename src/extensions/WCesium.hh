#ifndef WCesium_H_
#define WCesium_H_

#include <Wt/WColor.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WSignal.h>
#include <vector>
#include <string>

namespace Wt
{
  ///////////////////////////////////////////////////////////////////////////////////////
  //WCesium
  ///////////////////////////////////////////////////////////////////////////////////////

  class WT_API WCesium : public WCompositeWidget
  {
  public:
    WCesium(const std::string &js);
  protected:
    virtual void render(WFlags<RenderFlag> flags);
    std::string m_javascrit;
  };

} //namespace Wt

#endif 
