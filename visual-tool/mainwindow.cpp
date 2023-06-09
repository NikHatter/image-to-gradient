#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMimeData>

#include "../image-to-gradient/gradient.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    inputScene = new QGraphicsScene(this);
    inputRoot = new QGraphicsItemGroup();
    inputScene->addItem(inputRoot);
    inputRoot->setPos(0, 0);

    samplerLine = new QGraphicsLineItem(0, 0, 1, 1);
    samplerStart = new QGraphicsEllipseItem(0, 0, 4, 4);
    samplerEnd =  new QGraphicsEllipseItem(0, 0, 4, 4);

    ui->imageView->setScene(inputScene);

    gradientScene = new QGraphicsScene(this);
    ui->gradientView->setScene(gradientScene);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QUrl url{ event->mimeData()->text() };
    setImage(url.toLocalFile());
    event->acceptProposedAction();
}

void MainWindow::on_chooseImage_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose input image"), {}, tr("Image Files (*.png *.jpg *.bmp)"));

    ui->imagePath->setText(fileName);
}

void MainWindow::on_imagePath_textChanged(const QString& text) {
    setImage(text);
}

void MainWindow::on_startX_valueChanged(double d) { updateSamplePoints(); }
void MainWindow::on_startY_valueChanged(double d) { updateSamplePoints(); }
void MainWindow::on_endX_valueChanged(double d) { updateSamplePoints(); }
void MainWindow::on_endY_valueChanged(double d) { updateSamplePoints(); }

void MainWindow::on_deflectionByte_valueChanged(double d) { ui->deflectionFloat->setValue(d / 255.f); };
void MainWindow::on_deflectionFloat_valueChanged(double d) { updateGradient(); }


inline ItG::Color<4> to_color(const QRgb& value) {
    constexpr float scale = 1.f / 255.f;
    return { qRed(value) * scale, qGreen(value) * scale, qBlue(value) * scale, qAlpha(value) * scale };
}


inline QColor getColor(const QImage& image, float x, float y) {
    return image.pixelColor(
        static_cast<int>(x * (image.width() - 1)), static_cast<int>(y * (image.height() - 1))
    );
}

inline ItG::Gradient::Linear<4> get_linear(const QImage& image, float x1, float y1, float x2, float y2) {
    if (image.isNull())
        return {};

    ItG::Gradient::Linear<4> gradient;

    const int width = image.width();
    const int height = image.height();

    int i_x1 = static_cast<int>(x1 * (width - 1));
    int i_x2 = static_cast<int>(x2 * (width - 1));
    int i_y1 = static_cast<int>(y1 * (height - 1));
    int i_y2 = static_cast<int>(y2 * (height - 1));

    int size_x = i_x2 - i_x1;
    int size_y = i_y2 - i_y1;
    gradient.reserve(std::max(abs(size_x), abs(size_y)));

    int sample_x = i_x1;
    int sample_y = i_y1;
    float position = 0.f;

    if (std::max(abs(size_x), abs(size_y)) < 3 ) {
        gradient.emplace_back(to_color(image.pixel(i_x1, i_y1)), 0.f);
        gradient.emplace_back(to_color(image.pixel(i_x2, i_y2)), 1.f);
    } else {
        while (position <= 1.0f) {
            gradient.emplace_back(to_color(image.pixel(sample_x, sample_y)), position);

            if (size_x == 0 && size_y == 0)
                break;

            if (size_x >= size_y) {
                sample_x++;
                position = float(sample_x - i_x1) / size_x;
                sample_y = static_cast<int>( std::lerp(i_y1, i_y2, position) );
            } else {
                sample_y++;
                position = float(sample_y - i_y1) / size_y;
                sample_x = static_cast<int>( std::lerp(i_x1, i_x2, position) );
            }
        }
    }
    return gradient;
}


void MainWindow::setImage(const QString& path) {
    if (path == ui->imagePath->text() || path.isEmpty() || !QFileInfo::exists(path))
        return;

    QPixmap pixmap{ path };
    if (pixmap.isNull())
        return;

    ui->imagePath->setText(path);

    currentImage = pixmap.toImage();

    inputRoot->childItems().clear();
    if (currentPixmap) {
        currentPixmap->setPixmap(pixmap);
        currentPixmap->update();
    } else {
        currentPixmap = new QGraphicsPixmapItem(pixmap);
        inputRoot->addToGroup(currentPixmap);
        inputRoot->addToGroup(samplerLine);
        inputRoot->addToGroup(samplerStart);
        inputRoot->addToGroup(samplerEnd);
    }
    // todo update inputScene size/transform

    updateSamplePoints();
}

void MainWindow::updateSamplePoints() {
    if (currentImage.isNull())
        return;

    float start_x = ui->startX->value();
    float start_y = ui->startY->value();
    float end_x = ui->endX->value();
    float end_y = ui->endY->value();

    qreal x1 = currentPixmap->boundingRect().x() + currentPixmap->boundingRect().width() * start_x;
    qreal x2 = currentPixmap->boundingRect().x() + currentPixmap->boundingRect().width() * end_x;
    qreal y1 = currentPixmap->boundingRect().y() + currentPixmap->boundingRect().height() * start_y;
    qreal y2 = currentPixmap->boundingRect().y() + currentPixmap->boundingRect().height() * end_y;
    qreal dot_r = 8;

    samplerLine->setLine(x1, y1, x2, y2);

    samplerStart->setBrush( getColor(currentImage, start_x, start_y) );
    samplerStart->setRect(x1 - dot_r, y1 - dot_r, 2 * dot_r, 2 * dot_r);

    samplerEnd->setBrush( getColor(currentImage, end_x, end_y) );
    samplerEnd->setRect(x2 - dot_r, y2 - dot_r, 2 * dot_r, 2 * dot_r);

    updateGradient();
}

void MainWindow::updateGradient() {
    if (currentImage.isNull())
        return;

    float start_x = ui->startX->value();
    float start_y = ui->startY->value();
    float end_x = ui->endX->value();
    float end_y = ui->endY->value();

    // TODO store
    auto linear = get_linear(currentImage, start_x, start_y, end_x, end_y);

    ItG::Gradient::LinearBuilder<4> builder;
    builder.tolerance = ui->deflectionFloat->value();
    auto gradient = builder.from_linear(linear);

    gradientScene->clear();
    QLinearGradient qt_gradient(0, 0, ui->gradientView->contentsRect().width(), 0);
    qt_gradient.setSpread(QGradient::PadSpread);
    for (auto& stop : gradient) {
        qt_gradient.setColorAt(stop.position, QColor(stop.color[0] * 255, stop.color[1] * 255, stop.color[2] * 255, stop.color[3] * 255));
    }
    gradientScene->setSceneRect(ui->gradientView->contentsRect());
    gradientScene->addRect(ui->gradientView->contentsRect(), {}, qt_gradient);
}

