#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QIntValidator>
#include <QMessageBox>

#include <iostream>
#include <vector>

#include "app/settings.h"
#include "bitmapresource.h"

#include "ui_bitmapresource.h"

QString BitmapResource::m_currentFilePath;
QString BitmapResource::m_currentFileFilter;

const char BitmapResource::FILE_SETTINGS_PATH[] = "paths/bitmap";
const char BitmapResource::FILE_FILTERS[] =
    "Image files (*.png *.bmp *.gif *.jpg *.jpeg);;"
    "Portable Network Graphics (*.png);;"
    "Windows Bitmap (*.bmp);;"
    "Joint Photographic Experts Group (*.jpg *.jpeg);;"
    "All files (*)";

BitmapResource::BitmapResource(QString id, QWidget* parent, Qt::WindowFlags flags)
: Resource(id, parent, flags),
  m_ui(new Ui::BitmapResource)
{
  setup();

  m_ui->editWidth->setText("0");
  m_ui->editHeight->setText("0");
  m_ui->editX->setText("0");
  m_ui->editY->setText("0");
  m_ui->editXalt->setText("0");
  m_ui->editYalt->setText("0");

  m_ui->editPlanar0->setText("01");
  m_ui->editPlanar1->setText("02");
  m_ui->editPlanar2->setText("04");
  m_ui->editPlanar3->setText("08");
}

BitmapResource::BitmapResource(const BitmapResource& res)
: Resource(res.id(), dynamic_cast<QWidget*>(res.parent()), res.windowFlags()),
  m_ui(new Ui::BitmapResource)
{
  setup();

  m_ui->editWidth->setText(res.m_ui->editWidth->text());
  m_ui->editHeight->setText(res.m_ui->editHeight->text());

  m_ui->editX->setText(res.m_ui->editX->text());
  m_ui->editY->setText(res.m_ui->editY->text());
  m_ui->editXalt->setText(res.m_ui->editXalt->text());
  m_ui->editYalt->setText(res.m_ui->editYalt->text());

  m_ui->editPlanar0->setText(res.m_ui->editPlanar0->text());
  m_ui->editPlanar1->setText(res.m_ui->editPlanar1->text());
  m_ui->editPlanar2->setText(res.m_ui->editPlanar2->text());
  m_ui->editPlanar3->setText(res.m_ui->editPlanar3->text());

  if (res.m_image) {
    m_image = new QImage(*res.m_image);
  }

  m_ui->radioScale1->setChecked(res.m_ui->radioScale1->isChecked());
  m_ui->radioScale2->setChecked(res.m_ui->radioScale2->isChecked());
  m_ui->radioScale4->setChecked(res.m_ui->radioScale4->isChecked());
  m_ui->checkAlpha->setChecked(res.m_ui->checkAlpha->isChecked());
  toggleAlpha(m_ui->checkAlpha->isChecked());
}

BitmapResource::BitmapResource(
    QString id,
    QDataStream* in,
    quint32 tocSize,
    bool egaMode,
    QWidget* parent,
    Qt::WindowFlags flags)
: Resource(id, parent, flags),
  m_ui(new Ui::BitmapResource),
  m_tocSize(tocSize),
  m_egaMode(egaMode)
{
  setup();

  parse(in);
}

BitmapResource::~BitmapResource()
{
  delete m_image;
  delete m_ui;
}

void BitmapResource::setup()
{
  m_ui->setupUi(this);

  QIntValidator* posValidator = new QIntValidator(INT16_MIN, INT16_MAX, this);
  m_ui->editX->setValidator(posValidator);
  m_ui->editY->setValidator(posValidator);
  m_ui->editXalt->setValidator(posValidator);
  m_ui->editYalt->setValidator(posValidator);

  m_image = 0;
}


void BitmapResource::parse(QDataStream *in) {
  qint16 width, height, x, y, xAlt, yAlt;
  std::array<quint8, 4> planar;

  // Header.
  *in >> width >> height;
  if (m_egaMode) {
    width *= 8;
  }

  *in >> xAlt >> yAlt >> x >> y;

  *in >> planar[0] >> planar[1] >> planar[2] >> planar[3];
  checkError(in, tr("header"));

  m_ui->editWidth->setText(QString::number(width));
  m_ui->editHeight->setText(QString::number(height));

  m_ui->editX->setText(QString::number(x));
  m_ui->editY->setText(QString::number(y));
  m_ui->editXalt->setText(QString::number(xAlt));
  m_ui->editYalt->setText(QString::number(yAlt));

  m_ui->editPlanar0->setText(QString("%1").arg(planar[0], 2, 16, QChar('0')).toUpper());
  m_ui->editPlanar1->setText(QString("%1").arg(planar[1], 2, 16, QChar('0')).toUpper());
  m_ui->editPlanar2->setText(QString("%1").arg(planar[2], 2, 16, QChar('0')).toUpper());
  m_ui->editPlanar3->setText(QString("%1").arg(planar[3], 2, 16, QChar('0')).toUpper());

  if (width == 0 || height == 0) {
    return;
  }

  if (m_egaMode) {
    parseEga(in, width, height, planar);
  }
  else
  {
    parseVga(in, width, height, planar);
  }

  m_ui->buttonExport->setEnabled(true);
  toggleAlpha(m_ui->checkAlpha->isChecked());
}

void BitmapResource::parseVga(
    QDataStream* in,
    quint16 width,
    quint16 height,
    std::array<quint8, 4> const& planar)
{
    int length = width * height;
    unsigned char *data = 0;

    try {
        try {
            data = new unsigned char[length];
        } catch (std::bad_alloc &exc) {
            throw tr("Couldn't allocate memory for image data.");
        }

        if (in->readRawData((char *)data, length) != length) {
            throw tr("Couldn't read image data.");
        }

        // Process data.
        m_image = new QImage(width, height, QImage::Format_Indexed8);
        m_image->setColorTable(Settings::m_vgaPalette);

        int ry = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if ((planar[2] & 0x10) == 0x10) {
                    m_image->setPixel(x, y, data[(x * height) + y]);
                } else if ((planar[2] & 0x20) == 0x20) {
                    if ((y % 2) == 0) {
                        m_image->setPixel(x, y, data[(x * height) + ry]);
                    } else {
                        m_image->setPixel(x, y, data[(height / 2) + (x * height) + ry]);
                    }
                } else {
                    m_image->setPixel(x, y, data[(y * width) + x]);
                }
            }

            if ((y % 2) == 0) {
                ry++;
            }
        }
    } catch (QString msg) {
        delete[] data;
        data = 0;

        delete m_image;
        m_image = 0;

        throw msg;
    }

    delete[] data;
    data = 0;
}

void BitmapResource::parseEga(
    QDataStream* in,
    quint16 width,
    quint16 height,
    std::array<quint8, 4> const& planar)
{
  if (width == 0 || height == 0) {
    return;
  }

  int headerSize = 16;
  int tocSpriteSize = (m_tocSize > (quint32)headerSize) ? (m_tocSize - headerSize) : 0;

  std::vector<unsigned char> spriteData(tocSpriteSize, 0);
  in->readRawData((char*)spriteData.data(), tocSpriteSize);

  std::vector<std::vector<unsigned char>> bitPlanes(4, std::vector<unsigned char>(width * height, 0));

  int transposed_bitmask = planar[2] >> 4 & 0xF;

  for (int plane = 0; plane < 4; plane++) {
    int transposed = (transposed_bitmask >> plane) & 0x1;
    int planeStartBit = plane * width * height;
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int bit_idx;
        if (transposed) {
          bit_idx = planeStartBit + 8 * ((x / 8) * height + y) + (x % 8);
        } else {
          bit_idx = planeStartBit + y * width + x;
        }

        int byte_idx = bit_idx / 8;
        int bit_pos = 7 - (bit_idx % 8);

        unsigned char bit = 0;
        if (byte_idx < tocSpriteSize) {
          bit = (spriteData[byte_idx] >> bit_pos) & 1;
        }

        int idx = y * width + x;
        bitPlanes[plane][idx] = bit;
      }
    }
  }

  m_image = new QImage(width, height, QImage::Format_Indexed8);
  m_image->setColorTable(Settings::m_egaPalette);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = y * width + x;

      int color_index = (bitPlanes[3][idx] * (planar[3] & 0x0F)) +
                        (bitPlanes[2][idx] * (planar[2] & 0x0F)) +
                        (bitPlanes[1][idx] * (planar[1] & 0x0F)) +
                        (bitPlanes[0][idx] * (planar[0] & 0x0F));
      m_image->setPixel(x, y, color_index & 0xFF);
    }
  }

  m_ui->buttonExport->setEnabled(true);
  toggleAlpha(m_ui->checkAlpha->isChecked());
}

void BitmapResource::write(QDataStream* out) const
{
  qint16 xAlt, yAlt, x, y;
  quint8 planar[4];

  quint16 width = m_image ? m_image->width() : 0;
  quint16 height = m_image ? m_image->height() : 0;

  if (m_egaMode) {
      width /= 8; // in octets
  }

  if (m_image) {
    *out << width << height;
  }
  else {
    *out << (quint16)0 << (quint16)0;
  }

  xAlt = m_ui->editXalt->text().toShort();
  yAlt = m_ui->editYalt->text().toShort();
  x = m_ui->editX->text().toShort();
  y = m_ui->editY->text().toShort();

  *out << xAlt << yAlt << x << y;

  // Always save with vanilla planar scheme: non-transposed, non-interlaced,
  // EGA in standard bit order
  planar[0] = 1; // m_ui->editPlanar0->text().toUShort(0, 16);
  planar[1] = 2; // m_ui->editPlanar1->text().toUShort(0, 16);
  planar[2] = 4; // m_ui->editPlanar2->text().toUShort(0, 16) & 0xCF;
  planar[3] = 8; // m_ui->editPlanar3->text().toUShort(0, 16);
  *out << planar[0] << planar[1] << planar[2] << planar[3];

  checkError(out, tr("header"), true);

  if (m_image) {
    int length = width * height;
    if (m_egaMode) {
        for (unsigned plane = 0; plane < 4; ++plane) {
          for (unsigned y = 0; y < height; ++y) {
            for (unsigned lineOctet = 0; lineOctet < width; ++lineOctet) {
                quint8 byte = 0;
                for (unsigned b = 0; b < 8; ++b) {
                    unsigned x = lineOctet * 8 + b;
                    int pixelIndex = m_image->pixelIndex(x, y);

                    bool isPixelChannelUp = ((pixelIndex >> plane) & 1);
                    byte |= (isPixelChannelUp << (7 - b));
                }
                *out << byte;
            }
          }
        }
    }
    else
    {
      if (out->writeRawData((char*)m_image->bits(), length) != length) {
        throw tr("Couldn't write image data.");
      }
    }
  }
}

void BitmapResource::toggleAlpha(bool alpha)
{
  if (!m_image) {
    return;
  }

  QColor color(m_image->color(ALPHA_INDEX));
  color.setAlpha(alpha ? 0 : 255);
  m_image->setColor(ALPHA_INDEX, color.rgba());

  scale(); // Repaint.
}

void BitmapResource::scale()
{
  if (!m_image) {
    return;
  }

  if (m_ui->radioScale1->isChecked()) {
    m_ui->imageLabel->setPixmap(QPixmap::fromImage(*m_image));
  }
  else if (m_ui->radioScale2->isChecked()) {
    m_ui->imageLabel->setPixmap(QPixmap::fromImage(m_image->scaled(m_image->width() * 2, m_image->height() * 2)));
  }
  else if (m_ui->radioScale4->isChecked()) {
    m_ui->imageLabel->setPixmap(QPixmap::fromImage(m_image->scaled(m_image->width() * 4, m_image->height() * 4)));
  }

  m_ui->imageLabel->adjustSize();
}

void BitmapResource::exportFile()
{
  if (m_currentFilePath.isEmpty()) {
    m_currentFilePath = Settings().getFilePath(FILE_SETTINGS_PATH);
  }

  QFileInfo fileInfo(m_currentFilePath);
  fileInfo.setFile(
      fileInfo.absolutePath() +
      QDir::separator() +
      QString("%1-%2").arg(QString(fileName()).replace('.', '_'), id()) +
      ".png");
  m_currentFilePath = fileInfo.absoluteFilePath();

  QString outFileName = QFileDialog::getSaveFileName(
      this,
      tr("Export bitmap"),
      m_currentFilePath,
      FILE_FILTERS,
      &m_currentFileFilter);

  if (!outFileName.isEmpty()) {
    Settings().setFilePath(FILE_SETTINGS_PATH, m_currentFilePath = outFileName);

    QImageWriter writer(m_currentFilePath);
    writer.setText("Comment", QString("Stunts bitmap \"%1\" (%2)").arg(id(), fileName()));

    if (!writer.write(*m_image)) {
      QMessageBox::critical(
          this,
          QCoreApplication::applicationName(),
          tr("Error exporting bitmap resource \"%1\" to image file \"%2\":\n%3").arg(id(), m_currentFilePath, writer.errorString()));
    }
  }
}

void BitmapResource::importFile()
{
  if (m_currentFilePath.isEmpty()) {
    m_currentFilePath = Settings().getFilePath(FILE_SETTINGS_PATH);
  }

  QString inFileName = QFileDialog::getOpenFileName(
      this,
      tr("Import bitmap"),
      m_currentFilePath,
      FILE_FILTERS,
      &m_currentFileFilter);

  if (!inFileName.isEmpty()) {
    Settings().setFilePath(FILE_SETTINGS_PATH, m_currentFilePath = inFileName);

    QImage* newImage = new QImage();
    QImage* oldImage = m_image;

    QImageReader reader(m_currentFilePath);

    try {
      QSize size = reader.size();
      if ((size.width() > MAX_WIDTH) || (size.height() > MAX_HEIGHT)) {
        throw tr("Source file exceeds max dimensions.");
      }

      if (!reader.read(newImage)) {
        throw reader.errorString();
      }

      // Qt will not upsample color space on images with less than 256 colors.
      // We'll have to increase the color depth to 32 bits before downsampling
      // in order to avoid palette corruption.
      if (newImage->colorCount() && newImage->colorCount() < 256) {
        QImage* tmpImage = newImage;
        newImage =  new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
        delete tmpImage;
      }

      delete oldImage;
      oldImage = 0;

      Palette const& palette = m_egaMode ? Settings::m_egaPalette : Settings::m_vgaPalette;
      m_image = new QImage(newImage->convertToFormat(QImage::Format_Indexed8, palette));

      delete newImage;
      newImage = 0;

      // Quantize alpha channel to 1 bit and set affected transparent pixels
      // to palette index 255.
      if (m_image->hasAlphaChannel()) {
        const QImage alphaChannel = m_image->convertToFormat(QImage::Format_Alpha8);

        for (int y = 0; y < m_image->height(); y++) {
          for (int x = 0; x < m_image->width(); x++) {
            if (alphaChannel.pixelIndex(x, y) < 0x80) {
              m_image->setPixel(x, y, ALPHA_INDEX);
            }
          }
        }
      }

      m_ui->editWidth->setText(QString::number(m_image->width()));
      m_ui->editHeight->setText(QString::number(m_image->height()));

      m_ui->editPlanar0->setText(QString("%1").arg(1, 2, 16, QChar('0')));
      m_ui->editPlanar1->setText(QString("%1").arg(2, 2, 16, QChar('0')));
      m_ui->editPlanar2->setText(QString("%1").arg(4, 2, 16, QChar('0')));
      m_ui->editPlanar3->setText(QString("%1").arg(8, 2, 16, QChar('0')));

      m_ui->buttonExport->setEnabled(true);

      toggleAlpha(m_ui->checkAlpha->isChecked()); // Repaint
      isModified();
    }
    catch (QString msg) {
      delete newImage;
      newImage = 0;

      m_image = oldImage;

      QMessageBox::critical(
          this,
          QCoreApplication::applicationName(),
          tr("Error importing bitmap resource \"%1\" from image file \"%2\":\n%3").arg(id(), m_currentFilePath, msg));
    }
  }
}
