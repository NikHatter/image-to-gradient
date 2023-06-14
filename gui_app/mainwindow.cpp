#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMimeData>

#include "gradient.hpp"
#include "image/qt_image.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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

    setImage(fileName);
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

    samplerStart->setBrush( ItG::Image::get_color(currentImage, start_x, start_y) );
    samplerStart->setRect(x1 - dot_r, y1 - dot_r, 2 * dot_r, 2 * dot_r);

    samplerEnd->setBrush( ItG::Image::get_color(currentImage, end_x, end_y) );
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
    auto linear = ItG::Image::get_linear(currentImage, start_x, start_y, end_x, end_y);

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

