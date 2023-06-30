#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "positionmanager.h"
#include "config.h"
#include <iostream>
#include <QInputDialog>
#include <QSettings>
#include <QTranslator>

MainWindow* MainWindow::instance = nullptr;
extern QTranslator translator;
extern Config config;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIcon windowIcon(":/logo.png"); // Chemin de l'image dans le fichier .qrc
    setWindowIcon(windowIcon);

    qApp->setStyleSheet("QMainWindow { background-color: #323232; }"
                        "QPushButton {"
                        "background-color: #424242; "
                        "border-style: solid; "
                        "border-width: 1px; "
                        "border-radius: 4px; "
                        "border-color: #6a6a6a; "
                        "color: #ffffff; "
                        "padding: 5px;"
                        "}"
                        "QPushButton:hover {"
                        "background-color: #484848;"
                        "}"
                        "QPushButton:pressed {"
                        "background-color: #484848;"
                        "}"
                        "QLabel {"
                        "color: #ffffff;"
                        "}"
                        "QLineEdit {"
                        "background-color: #424242; "
                        "color: #ffffff;"
                        "border-style: solid; "
                        "border-width: 1px; "
                        "border-color: #6a6a6a;"
                        "}"
                        "QWidget {"
                        "background-color: #424242;"
                        "}"
                        "QComboBox {"
                        "color: white;"
                        "}"
                        "QComboBox QAbstractItemView {"
                        "color: white;"
                        "}"
                        "QRadioButton {"
                        "color: white;"
                        "}"
                        );

    ui->languageSelector->blockSignals(true);
    ui->languageSelector->addItem("English", "en");
    ui->languageSelector->addItem("中文", "zh");
    ui->languageSelector->blockSignals(false);

    // Select current language
    QString currentLang = config.get("lang");
//    qDebug() << "Current lang: " << currentLang;
    int index = ui->languageSelector->findData(currentLang);
    if (index != -1) {
        ui->languageSelector->setCurrentIndex(index);
    }

    instance = this;
//    vkCodeTP = 0;
//    vkCodeSAVE = 0;
    QString vkCodeSAVEString = config.get("vkCodeSAVE");
    bool ok;
    vkCodeSAVE = vkCodeSAVEString.toInt(&ok);
    if(!ok){
        vkCodeSAVE = 0;
    }
    QString vkCodeTPString = config.get("vkCodeTP");
    vkCodeTP = vkCodeTPString.toInt(&ok);
    if(!ok){
        vkCodeTP = 0;
    }
    UnhookWindowsHookEx(hHook);
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    UnhookWindowsHookEx(hHook);
    QMainWindow::closeEvent(event); // Appeler la fonction de base pour s'assurer que l'événement de fermeture est correctement traité
}

void MainWindow::displayPositions(const QString& searchText = "")
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
    scrollLayout->setContentsMargins(5, 0, 5, 0);

    // Parcours les positions enregistrées dans le PositionManager
    int i = 0;
    for (const QString& positionName : positionManager.getPositions().keys()) {
        // Si une chaîne de recherche est fournie et que le nom de la position ne la contient pas, passer à la suivante
        if (!searchText.isEmpty() && !positionName.contains(searchText, Qt::CaseInsensitive)) {
            continue;
        }

        // Crée un nouveau widget pour regrouper les éléments de la position
        QWidget* positionWidget = new QWidget(this);
        if (i++ % 2 == 0) {
            QPalette pal = QPalette();
            pal.setColor(QPalette::Window, QColor::fromRgbF(0, 0, 0, 0.1f));
            positionWidget->setStyleSheet("QWidget {"
                                          "background-color: #535353;"
                                          "}"
                                          );
            positionWidget->setPalette(pal);
        }

        // Crée un nouveau layout horizontal pour le widget de position
        QHBoxLayout* positionLayout = new QHBoxLayout(positionWidget);
        positionLayout->setSpacing(0);
        positionLayout->setContentsMargins(0, 0, 0, 0);

        // Crée un nouveau bouton de suppression pour la position
        QPushButton* deleteButton = new QPushButton("🗑", positionWidget);
        deleteButton->setFixedSize(30, 30);
        deleteButton->setCursor(Qt::PointingHandCursor);

        if (i % 2 == 0) {
            QPalette pal = QPalette();
            pal.setColor(QPalette::Window, QColor::fromRgbF(0, 0, 0, 0.1f));
            deleteButton->setStyleSheet("QWidget:hover {"
                                          "background-color: #4A4A4A;"
                                          "}"
                                          );
            deleteButton->setPalette(pal);
        }

        if (i % 2 == 1) {
            QPalette pal = QPalette();
            pal.setColor(QPalette::Window, QColor::fromRgbF(0, 0, 0, 0.1f));
            deleteButton->setStyleSheet("QWidget:hover {"
                                        "background-color: #505050;"
                                        "}"
                                        );
            deleteButton->setPalette(pal);
        }

        positionLayout->addWidget(deleteButton);
        deleteButtons.append(deleteButton);

        // Crée un nouveau label avec le nom de la position
        QLabel* nameLabel = new QLabel(positionName, positionWidget);
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameLabel->setFont(QFont("Arial", 12));
        nameLabel->setContentsMargins(10, 0, 0, 0);
        positionLayout->addWidget(nameLabel);
        positionLabels.append(nameLabel);

        // Crée un nouveau bouton "Interagir" pour la position
        QPushButton* interactButton = new QPushButton("➜", positionWidget);
        interactButton->setFixedSize(30, 30);
        interactButton->setCursor(Qt::PointingHandCursor);

        if (i % 2 == 0) {
            QPalette pal = QPalette();
            pal.setColor(QPalette::Window, QColor::fromRgbF(0, 0, 0, 0.1f));
            interactButton->setStyleSheet("QWidget:hover {"
                                        "background-color: #4A4A4A;"
                                        "}"
                                        );
            interactButton->setPalette(pal);
        }

        if (i % 2 == 1) {
            QPalette pal = QPalette();
            pal.setColor(QPalette::Window, QColor::fromRgbF(0, 0, 0, 0.1f));
            interactButton->setStyleSheet("QWidget:hover {"
                                        "background-color: #505050;"
                                        "}"
                                        );
            interactButton->setPalette(pal);
        }

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

void MainWindow::displayFps(){
    float fps = positionManager.getFps();
    if(fps == 30){
        ui->fps_30->blockSignals(true);
        ui->fps_30->setChecked(true);
        ui->fps_30->blockSignals(false);
    }else if(fps == 60){
        ui->fps_60->blockSignals(true);
        ui->fps_60->setChecked(true);
        ui->fps_60->blockSignals(false);
    }else if(fps == 90){
        ui->fps_90->blockSignals(true);
        ui->fps_90->setChecked(true);
        ui->fps_90->blockSignals(false);
    }else if(fps == 120){
        ui->fps_120->blockSignals(true);
        ui->fps_120->setChecked(true);
        ui->fps_120->blockSignals(false);
    }
}

void MainWindow::on_pushButton_init_clicked()
{
    if(!positionManager.init()){
        this->game_window = positionManager.getGameWindow();
        positionManager.loadPos();
        displayPositions();
        displayFps();
    }
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



void MainWindow::setPositionName(const QString &name){
    ui->lineEdit_pos_name->setText(name);
}

LRESULT CALLBACK MainWindow::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (GetForegroundWindow() != MainWindow::instance->game_window) {
            return 0;
        }

        KBDLLHOOKSTRUCT *pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
        float speed = 1000.0;
        switch (wParam) {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN: {
                if (pKeyStruct->vkCode == MainWindow::instance->vkCodeTP) {
                    MainWindow::instance->positionManager.teleport();
                }else if(pKeyStruct->vkCode == MainWindow::instance->vkCodeSAVE){
                    MainWindow::instance->positionManager.track();
                    MainWindow::instance->display_track();
                } else if (MainWindow::instance->positionManager.getFlyHack()) {
                    qDebug() << pKeyStruct->vkCode;
                    if (pKeyStruct->vkCode == 81) {
                        MainWindow::instance->positionManager.updateVelocity(0, 0, -speed);
                    } else if (pKeyStruct->vkCode == 69) {
                        MainWindow::instance->positionManager.updateVelocity(0, 0, speed);
                    }
                }
                break;
            }
        }
    }
    // Appeler le prochain Hook dans la chaîne
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void saveKey(int key)
{
    QSettings settings("MonApplication", "MonProgramme");
    settings.setValue("userKey", key);
}

int loadKey()
{
    QSettings settings("MonApplication", "MonProgramme");
    return settings.value("userKey", -1).toInt(); // retourne -1 si la clé n'existe pas
}

void MainWindow::on_pushButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Choisir une touche"),
                                         tr("Touche :"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        // Convertir le texte entré en une touche virtuelle
        vkCodeTP = VkKeyScanEx(text[0].toLatin1(), GetKeyboardLayout(0)) & 0xFF;
        config.set("vkCodeTP", QString::number(vkCodeTP));
        config.save("config.ini");
    }
}


void MainWindow::on_searchCheckpoint_textChanged(const QString& searchText)
{
    // Mettre à jour la liste des positions affichées
    displayPositions(searchText);
}


void MainWindow::on_pushButton_2_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Choisir une touche"),
                                         tr("Touche :"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        // Convertir le texte entré en une touche virtuelle
        vkCodeSAVE = VkKeyScanEx(text[0].toLatin1(), GetKeyboardLayout(0)) & 0xFF;
        config.set("vkCodeSAVE", QString::number(vkCodeSAVE));
        config.save("config.ini");
    }
}

void MainWindow::on_btn_reset_drake_clicked()
{
    positionManager.resetDrake();
}


void MainWindow::on_btn_pause_drake_clicked()
{
    positionManager.pauseDrake();
}

void MainWindow::on_btn_speed_drake_up_clicked()
{
    positionManager.speedUpDrake();
}

void MainWindow::on_btn_speed_drake_down_clicked()
{
    positionManager.speedDownDrake();
}


void MainWindow::on_btn_speed_drake_clicked()
{
    positionManager.resetSpeedDrake();
}


void MainWindow::on_languageSelector_currentIndexChanged(int index)
{
    QString newLang = ui->languageSelector->itemData(index).toString();

    // Change la langue dans le fichier de config
    config.set("lang", newLang);
    config.save("config.ini");

    // Recharge la traduction
    QLocale locale;
    if (newLang == "en") {
        locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
    } else if (newLang == "fr") {
        locale = QLocale(QLocale::French, QLocale::France);
    } else if (newLang == "zh") {
        locale = QLocale(QLocale::Chinese, QLocale::China);
    }

    const QString baseName = "OnlyUP_Trainer_" + locale.name();
    if (translator.load(":/i18n/" + baseName)) {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);  // retranslate the user interface
    } else {
        qDebug() << "Failed to load translation:" << baseName;
    }
}

void MainWindow::on_flyRadioButton_toggled(bool checked)
{
    positionManager.setFlyHack(checked);
}

void MainWindow::on_fps_30_toggled(bool checked)
{
    positionManager.setFps(30.f);
}


void MainWindow::on_fps_60_toggled(bool checked)
{
    positionManager.setFps(60.f);
}


void MainWindow::on_fps_90_toggled(bool checked)
{
    positionManager.setFps(90.f);
}


void MainWindow::on_fps_120_toggled(bool checked)
{
    positionManager.setFps(120.f);
}
