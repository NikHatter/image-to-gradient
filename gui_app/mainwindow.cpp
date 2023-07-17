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

    stopsRoot = new QGraphicsItemGroup();

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

void MainWindow::on_stopCount_valueChanged(int d) {  updateGradient(); }
void MainWindow::on_stopDistance_valueChanged(double d) { updateGradient(); }


void MainWindow::on_modeApproximate_toggled(bool b) { updateGradient(); };
void MainWindow::on_modeSteps_toggled(bool b) { updateGradient(); };

void MainWindow::setImage(const QString& path) {
    if (path == ui->imagePath->text() || path.isEmpty() || !QFileInfo::exists(path))
        return;

    QPixmap pixmap{ path };
    if (pixmap.isNull())
        return;

    ui->imagePath->setText(path);

    currentImage = pixmap.toImage();

    if (currentPixmap) {
        currentPixmap->setPixmap(pixmap);
        currentPixmap->update();
    } else {
        currentPixmap = new QGraphicsPixmapItem(pixmap);
        inputRoot->addToGroup(currentPixmap);
        inputRoot->addToGroup(samplerLine);
        inputRoot->addToGroup(stopsRoot);
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

    auto stops_rect = currentPixmap->boundingRect();
    qreal stops_x1 = stops_rect.x() + stops_rect.width() * start_x;
    qreal stops_x2 = stops_rect.x() + stops_rect.width() * end_x;
    qreal stops_y1 = stops_rect.y() + stops_rect.height() * start_y;
    qreal stops_y2 = stops_rect.y() + stops_rect.height() * end_y;
    qreal dot_radius = 3;

    // TODO store
    auto linear = ItG::Image::get_linear(currentImage, start_x, start_y, end_x, end_y);

    ItG::Gradient::Linear<4> gradient{};

    if (ui->modeApproximate->isChecked()) {
        ItG::Gradient::Stops::Approximate<4, ItG::Gradient::Operator::MaxDifference<4>> stop_extract;
        stop_extract.tolerance = ui->deflectionFloat->value();

        gradient = ItG::Gradient::from_gradient(linear, stop_extract);
    } else if (ui->modeSteps->isChecked()) {
        auto stopDistance = ui->stopDistance->value();
        if (stopDistance == 0.f) {
            ItG::Gradient::Stops::ColorCount<4, ItG::Gradient::Operator::MaxDifference<4>> stop_extract;
            stop_extract.count = ui->stopCount->value() - 2;

            gradient = ItG::Gradient::from_gradient(linear, stop_extract);
        } else {
            ItG::Gradient::Stops::StepCount<4, ItG::Gradient::Operator::MaxDifference<4>> stop_extract;
            stop_extract.count = ui->stopCount->value() - 2;
            stop_extract.stop_distance = stopDistance;

            gradient = ItG::Gradient::from_gradient(linear, stop_extract);
        }
    }

    auto children = stopsRoot->childItems();
    for (auto child : children) {
        stopsRoot->removeFromGroup(child);
        if (child) {
            delete child;
        }
    }

    gradientScene->clear();
    QLinearGradient qt_gradient(0, 0, ui->gradientView->contentsRect().width(), 0);
    QStringList stops;
    qt_gradient.setSpread(QGradient::PadSpread);
    QRectF total_rect = gradientScene->sceneRect();
    qreal dot_half_size = 3;
    for (auto& stop : gradient) {
        QColor color{ 
            std::clamp(static_cast<int>(stop.color[0] * 255), 0, 255),
            std::clamp(static_cast<int>(stop.color[1] * 255), 0, 255),
            std::clamp(static_cast<int>(stop.color[2] * 255), 0, 255),
            std::clamp(static_cast<int>(stop.color[3] * 255), 0, 255) 
        };
        qt_gradient.setColorAt(stop.position, color);
        stops.push_back(
            QString("rgba(%1, %2, %3, %4) %5%")
            .arg( color.red() ).arg( color.green() ).arg( color.blue() ).arg( color.alpha() )
            .arg(stop.position * 100.f)
        );

        QGraphicsEllipseItem* stopDot = new QGraphicsEllipseItem( 
            std::lerp(stops_x1, stops_x2, stop.position) - dot_radius,
            std::lerp(stops_y1, stops_y2, stop.position) - dot_radius,
            2*dot_radius, 2*dot_radius
        );
        stopsRoot->addToGroup(stopDot);
        stopDot->setBrush(color);
    }
    gradientScene->setSceneRect(ui->gradientView->contentsRect());
    gradientScene->addRect(ui->gradientView->contentsRect(), {}, qt_gradient);

    ui->gradientStops->setPlainText(stops.join(", "));
}

