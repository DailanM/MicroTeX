#include "config.h"

//#if defined(BUILD_QT) && !defined(MEM_CHECK)

// (I have no idea what this is for.)
#ifndef GRAPHIC_SOKOL_INCLUDED
#define GRAPHIC_SOKOL_INCLUDED

#include <string>
#include "graphic/graphic.h"

// For font selection and font rendering, reference the fonstash-sapp sokol sample.
// We need to implement all the features in graphic.h, notably graphic2D

// TODO: should I be able to grab an initialized sokol context?

// TODO: sokol has a fontstash, but I don't think it has an actual font class. I dont understand how it switches between fonts yet.
//       Do we make a single stash per font?


// Adding all sokol headers here for my reference. (for future me: sokol system headers are on AUR)
// Delete unneeded includes later!

#include <stdio.h>  // needed by fontstash's IO functions even though they are not used
#define FONTSTASH_IMPLEMENTATION
#include "fontstash/fontstash.h"
#include "fontstash/stb_truetype.h"

// Sokol
#define SOKOL_GL_IMPL
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
#define SOKOL_FONTSTASH_IMPL
#include "sokol_fontstash.h" // font rendering!! Renders them to a texture.
//#include "sokol_gfx_imgui.h"
//#include "sokol_debugtext.h"
//#include "sokol_memtrack.h"
//#include "sokol_shape.h"
//#include "sokol_color.h"  // microtex has some color functions, I might need this for that.
//#include "sokol_spine.h"


// garbage from here down.
// it looks like the skia body will be easier to adapt.

namespace tex {

class Font_sokol : public Font {
  // implementing the Font class in graphics.h

private:

  // The skia implementation had a list of typfaces accessed by mapping sokol font info (string name, int style) to a skia typeface.
  // 
  // For the sokol / fonstash implementation, the instantiated font classes will share a fontstash context, which keeps track of the
  // currently loaded fonts and indexes them by an integer. We create a map from the microtex implementation of a font to that index.
  // 
  //
  // The microtex implementation of a font has a fontname, style (normal/bold/italic), and size. The fonstrash implementation has a
  // dpi scale which is determined by the sokol context, but the size of a font is set in the 'state' of the fontstash context, which
  // is independant of the font data. so we need to remember that here.
  //
  // Following fontstash-sapp, when we create a new font, we should check if the font data is already loaded in the atlas, and then
  // if not, submit the fonts to load to via sfetch, using the name/style pair to find the file.
  //
  // Once loaded it'll callback the loaded function, in which we'll be able to pass the data to the fontstash context.
  //
  // TODO: how does sokol-fetch load callbacks work? I'm assuming they should be scoped as a static function in the sokol font class?
  //
  // when we finally draw the font, we'll need to set the fontstash context's state size, (I assume with the graphics2D class), and
  // after that we can then call sokols render text function (presumable in graphics2D too) which renders "byteStrings," arrays of
  // bytes, as characters on screen.

  static FONScontext fons; // The fontstash context

  static std::map<std::pair<std::string, int>, int> _fontlist; // map from microtex font info to font index in stash.

  //static sk_sp<SkTypeface> loadTypefaceFromName(const std::string &family, int style = PLAIN);

  //static sk_sp<SkTypeface> loadTypefaceFromFile(const std::string &file);

  //Font_sokol(sk_sp<SkTypeface> typeface, float size);

public:

  // Set hinting and edging for rendering on LCD screen
  //static SkFont::Edging Edging;
  //static SkFontHinting Hinting;

  //Font_sokol(const std::string &family = "", int style = PLAIN, float size = 1.f);

  //Font_sokol(const std::string &file, float size);

  // Initializer called by microtex
  Font_sokol(const std::string &family, int style, float size);
  Font_sokol(const std::string &file, float size);

  std::string getFamily() const;

  int getStyle() const;

  int getFONScontextIndex() const;

  virtual float getSize() const override;

  virtual sptr<Font> deriveFont(int style) const override;

  virtual bool operator==(const Font &f) const override;

  virtual bool operator!=(const Font &f) const override;

  virtual ~Font_sokol() {};

};


/**************************************************************************************************/

class TextLayout_skia : public TextLayout {
private:
  SkFont _font;
  std::string _text;

public:
  TextLayout_skia(const std::wstring &src, const sptr<Font_skia> &font);

  virtual void getBounds(_out_ Rect &r) override;

  virtual void draw(Graphics2D &g2, float x, float y) override;
};

/**************************************************************************************************/

class Graphics2D_skia : public Graphics2D {
private:
  static Font_skia _default_font;

  SkCanvas *_canvas;
  SkPaint _paint;
  color _color;
  Stroke _stroke;
  const Font_skia *_font;
  float _sx, _sy;

public:
  Graphics2D_skia(SkCanvas *painter);

  SkCanvas *getSkCanvas() const;

  const SkPaint &getSkPaint() const;

  virtual void setColor(color c) override;

  virtual color getColor() const override;

  virtual void setStroke(const Stroke &s) override;

  virtual const Stroke &getStroke() const override;

  virtual void setStrokeWidth(float w) override;

  virtual const Font *getFont() const override;

  virtual void setFont(const Font *font) override;

  virtual void translate(float dx, float dy) override;

  virtual void scale(float sx, float sy) override;

  virtual void rotate(float angle) override;

  virtual void rotate(float angle, float px, float py) override;

  virtual void reset() override;

  virtual float sx() const override;

  virtual float sy() const override;

  virtual void drawChar(wchar_t c, float x, float y) override;

  virtual void drawText(const std::wstring &t, float x, float y) override;

  virtual void drawLine(float x, float y1, float x2, float y2) override;

  virtual void drawRect(float x, float y, float w, float h) override;

  virtual void fillRect(float x, float y, float w, float h) override;

  virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}

#endif GRAPHIC_SOKOL_INCLUDED
//#endif   // defined(BUILD_QT) && !defined(MEM_CHECK)
