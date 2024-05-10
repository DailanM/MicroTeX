#include "config.h"

//#if defined(BUILD_QT) && !defined(MEM_CHECK)

#ifndef GRAPHIC_QT_H_INCLUDED
#define GRAPHIC_QT_H_INCLUDED

#include <string>
#include "graphic/graphic.h"

// For font selection and font rendering, reference the fonstash-sapp sokol sample.
// We need to implement all the features in graphic.h, notably graphic2D

// TODO: should I be able to grab an initialized sokol context?

// TODO: sokol has a fontstash, but I don't think it has an actual font class. I dont understand how it switches between fonts yet.
//       Do we make a single stash per font?


// Adding all sokol headers here for my reference. (for future me: sokol system headers are on AUR)
// Delete unneeded includes later!

// core
//#include "sokol_app.h"
#include "sokol_gfx.h" // required by fontstash
//#include "sokol_time.h"
//#include "sokol_audio.h"
#include "sokol_fetch.h" // this will be used to read ttf files. 
//#include "sokol_args.h"
//#include "sokol_log.h"

// util
//#include "sokol_imgui.h"
//#include "sokol_nuklear.h"
#include "sokol_gl.h" // required by fontstash
#include "sokol_fontstash.h" // font rendering!! Renders them to a texture.
//#include "sokol_gfx_imgui.h"
//#include "sokol_debugtext.h"
//#include "sokol_memtrack.h"
//#include "sokol_shape.h"
//#include "sokol_color.h"  // microtex has some color functions, I might need this for that.
//#include "sokol_spine.h"

// TODO: I think I need stdio

// garbage from here down.

namespace tex {

// remove a null byte off end of QString
//QString wstring_to_QString(const std::wstring& ws);

class Font_qt : public Font {

private:
  //QFont _font;

  //static QMap<QString, QString> _loaded_families;

public:

  Font_qt(const std::string& family = "", int style = PLAIN, float size = 1.f);

  Font_qt(const std::string& file, float size);

  std::string getFamily() const;

  int getStyle() const;

  QFont getQFont() const;

  virtual float getSize() const override;

  virtual sptr<Font> deriveFont(int style) const override;

  virtual bool operator==(const Font& f) const override;

  virtual bool operator!=(const Font& f) const override;

  virtual ~Font_qt() {};

};


/**************************************************************************************************/

class TextLayout_qt : public TextLayout {
private:
  QFont _font;
  QString _text;

public:
  TextLayout_qt(const std::wstring& src, const sptr<Font_qt>& font);

  virtual void getBounds(Rect& r) override;

  virtual void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class Graphics2D_qt : public Graphics2D {
private:
  /*static*/ Font_qt _default_font = Font_qt("SansSerif", PLAIN, 20.f);

  QPainter* _painter;

  color _color;
  Stroke _stroke;
  const Font_qt* _font;
  float _sx, _sy;

  void setPen();
  QBrush getQBrush() const;

public:
  Graphics2D_qt(QPainter* painter);

  QPainter* getQPainter() const;

  virtual void setColor(color c) override;

  virtual color getColor() const override;

  virtual void setStroke(const Stroke& s) override;

  virtual const Stroke& getStroke() const override;

  virtual void setStrokeWidth(float w) override;

  virtual const Font* getFont() const override;

  virtual void setFont(const Font* font) override;

  virtual void translate(float dx, float dy) override;

  virtual void scale(float sx, float sy) override;

  virtual void rotate(float angle) override;

  virtual void rotate(float angle, float px, float py) override;

  virtual void reset() override;

  virtual float sx() const override;

  virtual float sy() const override;

  virtual void drawChar(wchar_t c, float x, float y) override;

  virtual void drawText(const std::wstring& t, float x, float y) override;

  virtual void drawLine(float x, float y1, float x2, float y2) override;

  virtual void drawRect(float x, float y, float w, float h) override;

  virtual void fillRect(float x, float y, float w, float h) override;

  virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}

#endif   // GRAPHIC_QT_H_INCLUDED
//#endif   // defined(BUILD_QT) && !defined(MEM_CHECK)
