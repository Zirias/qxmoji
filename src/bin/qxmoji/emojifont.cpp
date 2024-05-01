#include "emojifont.h"

#include <QFont>
#include <QFontDatabase>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define dbfont(f,s,p) QFontDatabase::font((f),(s),(p))
#define sysfont QFontDatabase::systemFont(QFontDatabase::GeneralFont)
#else
#define dbfont(f,s,p) qfontdatabase.font((f),(s),(p))
#define sysfont qfontdatabase.systemFont(QFontDatabase::GeneralFont)
#endif

#define TranslateScale \
QT_TRANSLATE_NOOP("Scale", "Tiny") \
QT_TRANSLATE_NOOP("Scale", "Small") \
QT_TRANSLATE_NOOP("Scale", "Medium") \
QT_TRANSLATE_NOOP("Scale", "Large") \
QT_TRANSLATE_NOOP("Scale", "Huge")

class EmojiFontPrivate
{
    Q_DISABLE_COPY(EmojiFontPrivate)
    Q_DECLARE_PUBLIC(EmojiFont)
    EmojiFont *const q_ptr;

    QFont font;
    EmojiFont::Scale scale;
    int defpointsz;

    EmojiFontPrivate(EmojiFont *);
    int scaledSize();
};

EmojiFontPrivate::EmojiFontPrivate(EmojiFont *f) :
    q_ptr(f),
    font(),
    scale(EmojiFont::Scale::Small),
    defpointsz(0)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QFontDatabase qfontdatabase;
#endif
    defpointsz = sysfont.pointSize();
    if (defpointsz <= 0) defpointsz = 9;
    font = dbfont("Noto Color Emoji", QString(), scaledSize());
    if (font.family() != "Noto Color Emoji")
    {
	font = dbfont("Noto Emoji", QString(), scaledSize());
    }
}

int EmojiFontPrivate::scaledSize()
{
    switch (scale)
    {
	case EmojiFont::Scale::Tiny: return defpointsz;
	default:
	case EmojiFont::Scale::Small: return defpointsz + defpointsz/2;
	case EmojiFont::Scale::Medium: return 2*defpointsz;
	case EmojiFont::Scale::Large: return 3*defpointsz;
	case EmojiFont::Scale::Huge: return 4*defpointsz;
    }
}

EmojiFont::EmojiFont() :
    QObject(),
    d_ptr(new EmojiFontPrivate(this))
{}

EmojiFont::~EmojiFont() {}

const QFont &EmojiFont::font() const
{
    Q_D(const EmojiFont);
    return d->font;
}

void EmojiFont::setScale(EmojiFont::Scale scale)
{
    Q_D(EmojiFont);
    if (scale == d->scale) return;
    d->scale = scale;
    d->font.setPointSize(d->scaledSize());
    emit fontChanged();
}

#include "moc_emojifont.cpp"
