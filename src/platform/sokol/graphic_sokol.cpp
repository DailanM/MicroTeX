#include "graphic_sokol.h"

#include "config.h"

//#if defined(BUILD_SOKOL) && !defined(MEM_CHECK) // this is in my way

#include "platform/sokol/graphic_sokol.h"

// Do I need to include stuff here again?

// garbage from here down.

using namespace tex;
using namespace std;

//std::map<std::pair<std::string, int>, int> Font_skia::_test;
//std::map<std::pair<std::string, int>, sk_sp<SkTypeface>> Font_skia::_named_typefaces;
//std::map<std::string, sk_sp<SkTypeface>> Font_skia::_file_typefaces;

//SkFont::Edging Font_skia::Edging {SkFont::Edging::kAntiAlias};
//SkFontHinting Font_skia::Hinting {SkFontHinting::kNone};

/*Font_skia::Font_skia(sk_sp<SkTypeface> typeface, float size) {
  _font.setTypeface(std::move(typeface));
  _font.setSubpixel(true);
  _font.setHinting(Hinting);
  _font.setEdging(Edging);
  _font.setSize(size);
}*/

/*sk_sp<SkTypeface> Font_skia::loadTypefaceFromName(const string &family, int style) {
  auto key = std::make_pair(family, style);
  if (auto it = _named_typefaces.find(key); it != _named_typefaces.end()) {
    return it->second;
  } else {
    SkFontStyle fontStyle(style & BOLD ? SkFontStyle::kBold_Weight : SkFontStyle::kNormal_Weight,
                          SkFontStyle::kNormal_Width,
                          style & ITALIC ? SkFontStyle::kItalic_Slant : SkFontStyle::kUpright_Slant);
    auto typeface = SkTypeface::MakeFromName(family.c_str(), fontStyle);
    _named_typefaces[key] = typeface;
    return typeface;
  }
}*/

/*sk_sp<SkTypeface> Font_skia::loadTypefaceFromFile(const string &file) {
  if (auto it = _file_typefaces.find(file); it != _file_typefaces.end()) {
#ifdef HAVE_LOG
    __log << file << " already loaded, skip\n";
#endif
    return it->second;
  }

  auto typeface = SkTypeface::MakeFromFile(file.c_str());
  if (!typeface) {
#ifdef HAVE_LOG
    __log << file << " failed to load\n";
#endif
    throw std::runtime_error("Failed to load font file: " + file);
  } else {
    _file_typefaces[file] = typeface;
  }
  return typeface;
}*/

Font_sokol::Font_sokol(const string &family, int style, float size)
    : Font_sokol(loadTypefaceFromName(family, style), size) {}

Font_sokol::Font_sokol(const string &file, float size)
    : Font_sokol(loadTypefaceFromFile(file), size) {}

string Font_sokol::getFamily() const {
  SkString s;
  _font.getTypeface()->getFamilyName(&s);
  return s.c_str();
}

int Font_sokol::getStyle() const {
  int out = PLAIN;
  auto fs = _font.getTypeface()->fontStyle();
  if (fs.weight() == SkFontStyle::kBold_Weight) out |= BOLD;
  if (fs.slant() == SkFontStyle::kItalic_Slant) out |= ITALIC;
  return out;
}

int Font_sokol::getFONScontextIndex() const {
  return _font;
}

float Font_sokol::getSize() const {
  return _font.getSize();
}

sptr<Font> Font_sokol::deriveFont(int style) const {
  return sptr<Font>(new Font_sokol(getFamily(), style, getSize()));
}

bool Font_sokol::operator==(const Font &ft) const {
  const Font_sokol &o = static_cast<const Font_sokol &>(ft);

  return getFamily() == o.getFamily() && getSize() == o.getSize() &&
         getStyle() == o.getStyle();
}

bool Font_sokol::operator!=(const Font &ft) const {
  return !(*this == ft);
}

Font *Font::create(const string &file, float size) {
  return new Font_sokol(file, size);
}

sptr<Font> Font::_create(const string &name, int style, float size) {
  return sptr<Font>(new Font_sokol(name, style, size));
}

/**************************************************************************************************/

TextLayout_skia::TextLayout_skia(const wstring &src, const sptr<Font_skia> &f) :
    _font(f->getSkFont()),
    _text(wide2utf8(src.c_str())) {}

void TextLayout_skia::getBounds(_out_ Rect &r) {
  SkRect rect;
  _font.measureText(_text.c_str(), _text.size(), SkTextEncoding::kUTF8, &rect);
  r.x = rect.left();
  r.y = rect.top();
  r.w = rect.width();
  r.h = rect.height();
}

void TextLayout_skia::draw(Graphics2D &g2, float x, float y) {
  Graphics2D_skia &g = static_cast<Graphics2D_skia &>(g2);
  g.getSkCanvas()->drawString(_text.c_str(), x, y, _font, g.getSkPaint());
}

sptr<TextLayout> TextLayout::create(const wstring &src, const sptr<Font> &font) {
  sptr<Font_skia> f = static_pointer_cast<Font_skia>(font);
  return sptr<TextLayout>(new TextLayout_skia(src, f));
}

/**************************************************************************************************/

Font_skia Graphics2D_skia::_default_font("SansSerif", PLAIN, 20.f);

Graphics2D_skia::Graphics2D_skia(SkCanvas *canvas)
    : _canvas{canvas} {
  _sx = _sy = 1.f;
  _paint.setAntiAlias(true);
  setColor(BLACK);
  setStroke(Stroke());
  setFont(&_default_font);
}

void Graphics2D_skia::setColor(color c) {
  _color = c;
  _paint.setColor(c);
}

color Graphics2D_skia::getColor() const {
  return _color;
}

void Graphics2D_skia::setStroke(const Stroke &s) {
  _stroke = s;
  SkPaint::Cap cap;
  switch (_stroke.cap) {
    case CAP_ROUND:
      cap = SkPaint::Cap::kRound_Cap;
      break;
    case CAP_SQUARE:
      cap = SkPaint::Cap::kSquare_Cap;
      break;
    case CAP_BUTT:
    default:
      cap = SkPaint::Cap::kButt_Cap;
      break;
  }
  _paint.setStrokeCap(cap);

  _paint.setStrokeWidth(s.lineWidth);
  _paint.setStrokeMiter(s.miterLimit);

  SkPaint::Join join;
  switch (_stroke.join) {
    case JOIN_BEVEL:
      join = SkPaint::kBevel_Join;
      break;
    case JOIN_ROUND:
      join = SkPaint::kRound_Join;
      break;
    case JOIN_MITER:
    default:
      join = SkPaint::kMiter_Join;
      break;
  }
  _paint.setStrokeJoin(join);
}

const Stroke &Graphics2D_skia::getStroke() const {
  return _stroke;
}

void Graphics2D_skia::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
  _paint.setStrokeWidth(w);
}

const Font *Graphics2D_skia::getFont() const {
  return _font;
}

void Graphics2D_skia::setFont(const Font *font) {
  _font = static_cast<const Font_skia *>(font);
}

void Graphics2D_skia::translate(float dx, float dy) {
  //qInfo() << "translate" << dx << dy;
  _canvas->translate(dx, dy);
}

void Graphics2D_skia::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _canvas->scale(sx, sy);
}

static inline float radiansToDegrees(float angle) {
  return angle * 180.0f / (float) M_PI;
}

void Graphics2D_skia::rotate(float angle) {
  _canvas->rotate(radiansToDegrees(angle));
}

void Graphics2D_skia::rotate(float angle, float px, float py) {
  _canvas->translate(px, py);
  _canvas->rotate(radiansToDegrees(angle));
  _canvas->translate(-px, -py);
}

void Graphics2D_skia::reset() {
  _canvas->resetMatrix();
  _sx = _sy = 1.f;
}

float Graphics2D_skia::sx() const {
  return _sx;
}

float Graphics2D_skia::sy() const {
  return _sy;
}

void Graphics2D_skia::drawChar(wchar_t c, float x, float y) {
  wstring str = {c};
  drawText(str, x, y);
}

void Graphics2D_skia::drawText(const wstring &t, float x, float y) {
  auto str = wide2utf8(t.c_str());
  _paint.setStyle(SkPaint::kFill_Style);
  _canvas->drawString(str.c_str(), x, y, _font->getSkFont(), _paint);
}

void Graphics2D_skia::drawLine(float x1, float y1, float x2, float y2) {
  _paint.setStyle(SkPaint::kStroke_Style);
  _canvas->drawLine(x1, y1, x2, y2, _paint);
}

void Graphics2D_skia::drawRect(float x, float y, float w, float h) {
  auto rect = SkRect::MakeXYWH(x, y, w, h);
  _paint.setStyle(SkPaint::kStroke_Style);
  _canvas->drawRect(rect, _paint);
}

void Graphics2D_skia::fillRect(float x, float y, float w, float h) {
  auto rect = SkRect::MakeXYWH(x, y, w, h);
  _paint.setStyle(SkPaint::kFill_Style);
  _canvas->drawRect(rect, _paint);
}

void Graphics2D_skia::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _paint.setStyle(SkPaint::kStroke_Style);
  auto rect = SkRect::MakeXYWH(x, y, w, h);
  _canvas->drawRoundRect(rect, rx, ry, _paint);
}

void Graphics2D_skia::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _paint.setStyle(SkPaint::kFill_Style);
  auto rect = SkRect::MakeXYWH(x, y, w, h);
  _canvas->drawRoundRect(rect, rx, ry, _paint);
}

const SkPaint &Graphics2D_skia::getSkPaint() const {
  return _paint;
}

SkCanvas *Graphics2D_skia::getSkCanvas() const {
  return _canvas;
}
//#endif
