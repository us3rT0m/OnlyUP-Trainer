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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow* instance;

    void closeEvent(QCloseEvent *event) override;

    void display_track();

    void setPositionName(const QString &name);

    PositionManager positionManager;

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    int vkCodeTP;
    int vkCodeSAVE;

private slots:
    void on_pushButton_init_clicked();

    void on_pushButton_track_clicked();

    void on_pushButton_teleport_clicked();

    void on_pushButton_save_clicked();

    void on_label_linkActivated(const QString &link);

    void on_pushButton_clicked();

    void on_searchCheckpoint_textChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_btn_reset_drake_clicked();

    void on_btn_pause_drake_clicked();

    void on_btn_speed_drake_up_clicked();

    void on_btn_speed_drake_down_clicked();

    void on_btn_speed_drake_clicked();

private:
    Ui::MainWindow *ui;
    void displayPositions(const QString& searchText);
    QScrollArea* scrollArea;
    QList<QLabel*> positionLabels; // Liste pour stocker les labels des positions
    QList<QPushButton*> deleteButtons; // Liste pour stocker les boutons de suppression
    HHOOK hHook;
};
#endif // MAINWINDOW_H
