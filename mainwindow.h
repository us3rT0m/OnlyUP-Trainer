#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "positionmanager.h"
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

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
    void on_pushButton_init_clicked();

    void on_pushButton_track_clicked();

    void on_pushButton_teleport_clicked();

    void on_pushButton_save_clicked();

    void on_label_linkActivated(const QString &link);

private:
    Ui::MainWindow *ui;
    PositionManager positionManager; // Déclarer l'objet PositionManager comme membre
    void display_track();
    void displayPositions();
    QScrollArea* scrollArea;
    QList<QLabel*> positionLabels; // Liste pour stocker les labels des positions
    QList<QPushButton*> deleteButtons; // Liste pour stocker les boutons de suppression
};
#endif // MAINWINDOW_H
