#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QDragEnterEvent>
#include <QDropEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_chooseImage_clicked();
    void on_imagePath_textChanged(const QString &text);
    void on_startX_valueChanged(double d);
    void on_startY_valueChanged(double d);
    void on_endX_valueChanged(double d);
    void on_endY_valueChanged(double d);
    void on_deflectionFloat_valueChanged(double d);
    void on_deflectionByte_valueChanged(double d);

    void setImage(const QString& path);
    void updateSamplePoints();
    void updateGradient();

private:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);


    Ui::MainWindow *ui;

    QImage currentImage;

    QGraphicsScene* inputScene;
    QGraphicsItemGroup* inputRoot = nullptr;
    QGraphicsEllipseItem* samplerStart = nullptr;
    QGraphicsEllipseItem* samplerEnd = nullptr;
    QGraphicsLineItem* samplerLine = nullptr;
    QGraphicsPixmapItem* currentPixmap = nullptr;

    QGraphicsScene* gradientScene;

};
#endif // MAINWINDOW_H
