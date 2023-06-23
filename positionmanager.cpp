#include "positionmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QThread>
#include <QtMath>

PositionManager::PositionManager() {
    base_address = 0x000000;
    xCoord = 0x000000;
    yCoord = 0x000000;
    zCoord = 0x000000;
    xVelocityCoord = 0x000000;
    yVelocityCoord = 0x000000;
    zVelocityCoord = 0x000000;
    drakeDistSplineCoord = 0x000000;
    drakeMouvementCoord = 0x000000;
    x = 0.0;
    y = 0.0;
    z = 0.0;
    xV = 150.0;
    yV = 0.0;
    zV = 0.0;
    drakeDistSpline = 0.0;
    drakeMouvement = 1;
    DrakeInit = false;
    disableFlyMode();
}

int PositionManager::init(HWND gameWindow) {
    game_window = gameWindow;

    // Déclare une variable pour stocker l'ID du processus du jeu.
    DWORD process_id = 0;
    // Obtient l'ID du processus du jeu à partir de la fenêtre du jeu.
    GetWindowThreadProcessId(game_window, &process_id);
    if (!process_id) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Impossible d'obtenir l'ID du processus.");
        return 1;
    }

    // Ouvre le processus du jeu avec tous les droits d'accès.
    game_process = OpenProcess(PROCESS_ALL_ACCESS, true, process_id);
    if (!game_process) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Impossible d'ouvrir le processus.");
        return 1;
    }

    const wchar_t* modName = L"OnlyUP-Win64-Shipping.exe";
    base_address = GetModuleBaseAddress(process_id, modName);
    if (!base_address) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Impossible d'obtenir la base address du module du jeu.");
        return 1;
    }

    QThread *thread = nullptr;
    auto threadTest = std::function<void ()> ([&]() {
        while (true) {
            this->updateFly();

            thread->usleep(5000);
        }
    });

    thread = QThread::create(threadTest);
    thread->start();

    DrakeInit = false;

    // Initialisation des différentes zone mémoire du jeu.
    if(!initPos()){
        if(!initVelocity()){
            return 0;
        }else{
            return 1;
        }
    }else{
        return 1;
    }
}

void PositionManager::updateFly() {
    bool fly = this->flyLeft || this->flyRight || this->flyUp || this->flyDown || this->flyForwards || this->flyBackwards;
    if (fly || this->hovering) {
        double vel = 0;
        WriteProcessMemory(game_process, (void*)xVelocityCoord, &vel, sizeof(vel), nullptr);
        WriteProcessMemory(game_process, (void*)yVelocityCoord, &vel, sizeof(vel), nullptr);
        WriteProcessMemory(game_process, (void*)zVelocityCoord, &vel, sizeof(vel), nullptr);
    }

    if (!fly) return;

    double currentX, currentY, currentZ, rot;

    ReadProcessMemory(game_process, (void*)rotCoord, &rot, sizeof(rot), nullptr);
    ReadProcessMemory(game_process, (void*)xCoord, &currentX, sizeof(currentX), nullptr);
    ReadProcessMemory(game_process, (void*)yCoord, &currentY, sizeof(currentY), nullptr);
    ReadProcessMemory(game_process, (void*)zCoord, &currentZ, sizeof(currentZ), nullptr);

    if (this->flyUp) {
        currentZ += 15.0;
    } else if (this->flyDown) {
        currentZ -= 15.0;
    }

    double forwardAngle = 0;
    if (this->flyLeft) {
        forwardAngle = -45;
    } else if (this->flyRight) {
        forwardAngle = 45;
    }

    if (this->flyForwards) {
        double rotRadians = qDegreesToRadians(rot + forwardAngle);
        currentX += 10.0 * cos(rotRadians);
        currentY += 10.0 * sin(rotRadians);
    } else if (this->flyBackwards) {
        double rotRadians = qDegreesToRadians(rot - forwardAngle);
        currentX -= 10.0 * cos(rotRadians);
        currentY -= 10.0 * sin(rotRadians);
    } else if (this->flyLeft) {
        double rotLeft = qDegreesToRadians(rot - 90);
        currentX += 10.0 * cos(rotLeft);
        currentY += 10.0 * sin(rotLeft);
    } else if (this->flyRight) {
        double rotRight = qDegreesToRadians(rot + 90);
        currentX += 10.0 * cos(rotRight);
        currentY += 10.0 * sin(rotRight);
    }

    WriteProcessMemory(game_process, (void*)xCoord, &currentX, sizeof(currentX), nullptr);
    WriteProcessMemory(game_process, (void*)yCoord, &currentY, sizeof(currentY), nullptr);
    WriteProcessMemory(game_process, (void*)zCoord, &currentZ, sizeof(currentZ), nullptr);
}

void PositionManager::setFlyForwards(bool flyForward) {
    this->flyForwards = flyForward;
}

void PositionManager::setFlyBackwards(bool flyForward) {
    this->flyBackwards = flyForward;
}

void PositionManager::setFlyUp(bool flyUp) {
    this->flyUp = flyUp;
}

void PositionManager::setFlyDown(bool flyDown) {
    this->flyDown = flyDown;
}

void PositionManager::setFlyLeft(bool flyLeft) {
    this->flyLeft = flyLeft;
}

void PositionManager::setFlyRight(bool flyRight) {
    this->flyRight = flyRight;
}

void PositionManager::setHovering(bool hovering) {
    this->hovering = hovering;
}

void PositionManager::disableFlyMode() {
    this->flyForwards = false;
    this->flyBackwards = false;
    this->flyUp = false;
    this->flyDown = false;
    this->flyLeft = false;
    this->flyRight = false;
    this->hovering = false;
}

int PositionManager::initPos(){
    // Déclare une variable pour stocker l'adresse actuelle + ajoutez l'offset au début de l'adresse.
    uintptr_t current_address = base_address + 0x073C5ED8;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #1");
        return 1;
    }

    // Ajoute l'offset à l'adresse actuelle.
    current_address += 0x180;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #2");
        return 1;
    }

    // Ajoute l'offset à l'adresse actuelle.
    current_address += 0xA0;

    // Répète ce processus pour chaque offset.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #3");
        return 1;
    }
    current_address += 0x98;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #4");
        return 1;
    }
    current_address += 0xA8;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #5");
        return 1;
    }
    current_address += 0x60;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #6");
        return 1;
    }
    current_address += 0x328;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #7");
        return 1;
    }
    current_address += 0x270;

    zCoord = current_address;
    current_address -= 0x8;
    yCoord = current_address;
    current_address -= 0x8;
    xCoord = current_address;
    current_address -= 0x78;
    rotCoord = current_address;

    return 0;
}


int PositionManager::initVelocity(){
    // Déclare une variable pour stocker l'adresse actuelle + ajoutez l'offset au début de l'adresse.
    uintptr_t current_address = base_address + 0x07356580;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #8");
        return 1;
    }

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #9");
        return 1;
    }

    // Ajoute l'offset à l'adresse actuelle.
    current_address += 0x30;

    // Répète ce processus pour chaque offset.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #10");
        return 1;
    }
    current_address += 0xA8;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #11");
        return 1;
    }
    current_address += 0x50;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #12");
        return 1;
    }
    current_address += 0xAB8;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #13");
        return 1;
    }
    current_address += 0x20;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #14");
        return 1;
    }
    current_address += 0x320;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Run non trouvé #15");
        return 1;
    }
    current_address += 0xC8;


    zVelocityCoord = current_address;
    current_address -= 0x8;
    yVelocityCoord = current_address;
    current_address -= 0x8;
    xVelocityCoord = current_address;

    return 0;
}

int PositionManager::initDrake(){
    // Déclare une variable pour stocker l'adresse actuelle + ajoutez l'offset au début de l'adresse.
    uintptr_t current_address = base_address + 0x07356580;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #17");
        return 1;
    }

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #18");
        return 1;
    }

    // Ajoute l'offset à l'adresse actuelle.
    current_address += 0x2B0;

    // Répète ce processus pour chaque offset.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #19");
        return 1;
    }
    current_address += 0x170;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #20");
        return 1;
    }
    current_address += 0x50;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #21");
        return 1;
    }
    current_address += 0x28;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #22");
        return 1;
    }
    current_address += 0x8;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #23");
        return 1;
    }
    current_address += 0x70;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'initialisation du dragon #24");
            return 1;
    }
    current_address += 0x7C;

    drakeDistSplineCoord = current_address;
    current_address -= 0x4;
    drakeMouvementCoord = current_address;

    return 0;
}

void PositionManager::createPosition(const QString& name) {
    QJsonObject positionJson;
    positionJson["x"] = x;
    positionJson["y"] = y;
    positionJson["z"] = z;

    positions[name] = positionJson;
}

void PositionManager::track(){
    ReadProcessMemory(game_process, (void*)xCoord, &x, sizeof(x), nullptr);
    ReadProcessMemory(game_process, (void*)yCoord, &y, sizeof(y), nullptr);
    ReadProcessMemory(game_process, (void*)zCoord, &z, sizeof(z), nullptr);
}

void PositionManager::teleport(){
    WriteProcessMemory(game_process, (void*)xCoord, &x, sizeof(x), nullptr);
    WriteProcessMemory(game_process, (void*)yCoord, &y, sizeof(y), nullptr);
    WriteProcessMemory(game_process, (void*)zCoord, &z, sizeof(z), nullptr);

    WriteProcessMemory(game_process, (void*)xVelocityCoord, &xV, sizeof(x), nullptr);
    WriteProcessMemory(game_process, (void*)yVelocityCoord, &yV, sizeof(y), nullptr);
    WriteProcessMemory(game_process, (void*)zVelocityCoord, &zV, sizeof(z), nullptr);
}

void PositionManager::loadPos(){
    loadPositionsFromFile("pos.json");
}

double PositionManager::getX(){
    return x;
}

double PositionManager::getY(){
    return y;
}

double PositionManager::getZ(){
    return z;
}

void PositionManager::savePositionsToFile(const QString& filename) {
    QJsonObject root;
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        const QString& name = it.key();
        const QJsonObject& positionJson = it.value();

        root[name] = positionJson;
    }

    QJsonDocument doc(root);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'ouverture du fichier de checkpoint.");
        return;
    }

    file.write(doc.toJson());
    file.close();
}

void PositionManager::loadPositionsFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'ouverture du fichier de checkpoint.");
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de la lecture du fichier de checkpoint.");
        return;
    }

    QJsonObject root = jsonDoc.object();
    positions.clear(); // Efface les positions actuelles

    for (const QString& key : root.keys()) {
        const QJsonObject& positionJson = root.value(key).toObject();
        positions[key] = positionJson;
    }
}

uintptr_t PositionManager::GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!_wcsicmp(modEntry.szModule, modName)) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    else {
        // Affichage du message d'erreur dans une boîte de dialogue
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de la création du snapshot de module.");
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

void PositionManager::deletePosition(const QString& name)
{
    positions.remove(name);
    savePositionsToFile("pos.json");
}

void PositionManager::usePosition(const QString& positionName){
    QMap<QString, QJsonObject> positions = getPositions();
    QJsonObject position = positions.value(positionName);
    x = position.value("x").toDouble();
    y = position.value("y").toDouble();
    z = position.value("z").toDouble();
}

QMap<QString, QJsonObject> PositionManager::getPositions() const
{
    return positions;
}

void PositionManager::isDrakeInit(){
//    float tempValue = 0.f;
    if (!ReadProcessMemory(game_process, (void*)drakeDistSplineCoord, nullptr, sizeof(nullptr), nullptr)) {
        initDrake();
    }else{
        if(!DrakeInit){
            DrakeInit = true;
        }
    }
}

void PositionManager::resetDrake(){
    if(!DrakeInit){
        isDrakeInit();
    }
    WriteProcessMemory(game_process, (void*)drakeDistSplineCoord, &drakeDistSpline, sizeof(drakeDistSpline), nullptr);
}

void PositionManager::speedUpDrake(){
    if(!DrakeInit){
        isDrakeInit();
    }
    ReadProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);

    drakeMouvement+=1;

    WriteProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);
}

void PositionManager::speedDownDrake(){
    if(!DrakeInit){
        isDrakeInit();
    }
    ReadProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);

    drakeMouvement-=1;

    WriteProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);
}


void PositionManager::pauseDrake(){
    if(!DrakeInit){
        isDrakeInit();
    }
    ReadProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);

    const float pausedDrake = 0.0f;
    const float movingDrake = 1.0f;

    if(drakeMouvement != pausedDrake){
        drakeMouvement = pausedDrake;
    }else{
        drakeMouvement = movingDrake;
    }

    WriteProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);
}

void PositionManager::resetSpeedDrake(){
    if(!DrakeInit){
        isDrakeInit();
    }

    const float pausedDrake = 0.0f;
    const float movingDrake = 1.0f;

    if(drakeMouvement != pausedDrake){
        drakeMouvement = movingDrake;
    }

    WriteProcessMemory(game_process, (void*)drakeMouvementCoord, &drakeMouvement, sizeof(drakeMouvement), nullptr);
}
