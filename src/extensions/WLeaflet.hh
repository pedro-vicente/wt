#ifndef WLEAFLET_H_
#define WLEAFLET_H_

#include <Wt/WColor.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WSignal.h>
#include <vector>
#include <string>

namespace Wt
{
  enum class tile_provider_t
  {
    CARTODB, RRZE, MAPBOX
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //icon_size_t
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  class WT_API icon_size_t
  {
  public:
    icon_size_t(int w_, int h_) :
      w(w_),
      h(h_)
    {
    }
    int w;
    int h;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //marker_icon_t
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  class WT_API marker_icon_t
  {
  public:
    marker_icon_t(const std::string &icon_url_,
      const std::string &shadow_url_,
      icon_size_t icon_size_,
      icon_size_t icon_anchor_,
      icon_size_t popup_anchor_,
      icon_size_t shadow_size_) :
      icon_url(icon_url_),
      shadow_url(shadow_url_),
      icon_size(icon_size_),
      icon_anchor(icon_anchor_),
      popup_anchor(popup_anchor_),
      shadow_size(shadow_size_)
    {
    }
    std::string icon_url;
    std::string shadow_url;
    icon_size_t icon_size;
    icon_size_t icon_anchor;
    icon_size_t popup_anchor;
    icon_size_t shadow_size;
  };

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet
  ///////////////////////////////////////////////////////////////////////////////////////

  class WT_API WLeaflet : public WCompositeWidget
  {
  public:

    ///////////////////////////////////////////////////////////////////////////////////////
    //Coordinate
    ///////////////////////////////////////////////////////////////////////////////////////

    class WT_API Coordinate
    {
    public:
      Coordinate();
      Coordinate(double latitude, double longitude);
      explicit Coordinate(const std::pair<double, double>& lat_long);
      void setLatitude(double latitude);
      void setLongitude(double longitude);
      double latitude() const { return m_lat; }
      double longitude() const { return m_lon; }
      std::pair<double, double> operator ()() const;
    private:
      double m_lat, m_lon;
    };

    WLeaflet(tile_provider_t tile, double lat, double lon, int zoom, bool verbose = false);
    void MouseDown(const Wt::WMouseEvent &e);
    void Circle(const std::string &lat, const std::string &lon);
    void Circle(const double lat, const double lon, const std::string &color);
    void Circle(const std::string &lat, const std::string &lon, const std::string &color);
    void Circle(const double lat, const double lon, const double radius, const std::string &color);
    void Polygon(const std::vector<double> &lat, const std::vector<double> &lon, const std::string &color);
    void Marker(const std::string &lat, const std::string &lon, const std::string &text);
    void Marker(const std::string &lat, const std::string &lon, const std::string &text, marker_icon_t icon);
    void Marker(const double lat, const double lon, const std::string &text, marker_icon_t icon);
    JSignal<Coordinate>& clicked() { return m_clicked; }
    JSignal<Coordinate>& double_clicked() { return m_double_clicked; }

  protected:
    tile_provider_t m_tile;
    double m_lat;
    double m_lon;
    int m_zoom;
    virtual void render(WFlags<RenderFlag> flags);
    std::vector<std::string> m_additions;
    JSignal<Coordinate> m_clicked;
    JSignal<Coordinate> m_double_clicked;
    virtual void do_javascript(const std::string& jscode);
    void stream_listener(const JSignal<Coordinate> &signal, std::string signalName, WStringStream &strm);
  };

  extern WT_API std::istream& operator >> (std::istream& i, WLeaflet::Coordinate& coordinate);

} //namespace Wt

#endif // WLEAFLET_H_
