#ifndef WPLOTLY_H_
#define WPLOTLY_H_

#include <Wt/WColor.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WSignal.h>
#include <vector>
#include <string>

namespace Wt
{
  ///////////////////////////////////////////////////////////////////////////////////////
  //WPlotly
  ///////////////////////////////////////////////////////////////////////////////////////

  class WT_API WPlotly : public WCompositeWidget
  {
  public:

    ///////////////////////////////////////////////////////////////////////////////////////
    //Coordinate
    ///////////////////////////////////////////////////////////////////////////////////////

    class WT_API Coordinate
    {
    public:
      Coordinate();
      Coordinate(time_t x, double y);
      explicit Coordinate(const std::pair<time_t, double>& x_y);
      void set_x(time_t latitude);
      void set_y(double longitude);
      time_t x() const { return m_x; }
      double y() const { return m_y; }
      std::pair<time_t, double> operator ()() const;
    private:
      time_t m_x;
      double m_y;
    };

    WPlotly(const std::string &js);
    JSignal<Coordinate>& clicked() { return m_clicked; }

  protected:
    virtual void render(WFlags<RenderFlag> flags);
    std::string m_javascrit;
    JSignal<Coordinate> m_clicked;
  };

  extern WT_API std::istream& operator >> (std::istream& i, WPlotly::Coordinate& coordinate);

} //namespace Wt

#endif // WLEAFLET_H_
