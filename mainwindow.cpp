#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "positionmanager.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayPositions()
{
    // Efface tous les labels et boutons précédemment affichés
    for (QLabel* label : positionLabels) {
        delete label;
    }
    positionLabels.clear();

    for (QPushButton* button : deleteButtons) {
        delete button;
    }
    deleteButtons.clear();

    // Efface le layout précédent des positions
    QLayoutItem* item;
    while ((item = ui->verticalLayout_positions->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Crée une nouvelle zone de scroll
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    // Crée un nouveau widget pour le contenu de la zone de scroll
    QWidget* scrollContent = new QWidget(scrollArea);
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(0);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    // Parcours les positions enregistrées dans le PositionManager
    for (const QString& positionName : positionManager.getPositions().keys()) {
        // Crée un nouveau widget pour regrouper les éléments de la position
        QWidget* positionWidget = new QWidget(this);

        // Crée un nouveau layout horizontal pour le widget de position
        QHBoxLayout* positionLayout = new QHBoxLayout(positionWidget);
        positionLayout->setSpacing(0);
        positionLayout->setContentsMargins(0, 0, 0, 0);

        // Crée un nouveau bouton de suppression pour la position
        QPushButton* deleteButton = new QPushButton("🗑", positionWidget);
        deleteButton->setFixedSize(20, 20);
        positionLayout->addWidget(deleteButton);
        deleteButtons.append(deleteButton);

        // Crée un nouveau label avec le nom de la position
        QLabel* nameLabel = new QLabel(positionName, positionWidget);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setFont(QFont("Arial", 10));
        positionLayout->addWidget(nameLabel);
        positionLabels.append(nameLabel);

        // Crée un nouveau bouton "Interagir" pour la position
        QPushButton* interactButton = new QPushButton("→", positionWidget);
        interactButton->setFixedSize(20, 20);
        positionLayout->addWidget(interactButton);

        // Connecte le signal clicked() du bouton de suppression à une fonction de suppression
        connect(deleteButton, &QPushButton::clicked, [this, positionName]() {
            positionManager.deletePosition(positionName);
            displayPositions();
        });

        // Connecte le signal clicked() du bouton "Interagir" à une fonction d'interaction
        connect(interactButton, &QPushButton::clicked, [this, positionName]() {
            positionManager.usePosition(positionName);
            display_track();
            ui->lineEdit_pos_name->setText(positionName);
        });

        // Ajoute le widget de position au layout de la zone de scroll
        scrollLayout->addWidget(positionWidget);
    }

    // Définit le widget de contenu dans la zone de scroll
    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);

    // Ajoute la zone de scroll au layout vertical des positions
    ui->verticalLayout_positions->addWidget(scrollArea);
}



void MainWindow::on_pushButton_init_clicked()
{
    positionManager.init();
    positionManager.loadPos();
    displayPositions();
}

void MainWindow::display_track(){
    ui->label_x_value->setText(QString::number(positionManager.getX()));
    ui->label_y_value->setText(QString::number(positionManager.getY()));
    ui->label_z_value->setText(QString::number(positionManager.getZ()));
}


void MainWindow::on_pushButton_track_clicked()
{
    positionManager.track();
    display_track();
    ui->lineEdit_pos_name->setText("");
}


void MainWindow::on_pushButton_teleport_clicked()
{
    positionManager.teleport();
}


void MainWindow::on_pushButton_save_clicked()
{
    positionManager.createPosition(ui->lineEdit_pos_name->displayText());
    positionManager.savePositionsToFile("pos.json");
    positionManager.loadPos();
    displayPositions();
}


void MainWindow::on_label_linkActivated(const QString &link)
{
    ui->lineEdit_pos_name->setText("selected");
}

