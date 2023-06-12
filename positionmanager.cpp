#include "positionmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

PositionManager::PositionManager() {
    base_address = 0x000000;
    xCoord = 0x000000;
    yCoord = 0x000000;
    zCoord = 0x000000;
    xVelocityCoord = 0x000000;
    yVelocityCoord = 0x000000;
    zVelocityCoord = 0x000000;
    x = 0.0;
    y = 0.0;
    z = 0.0;
    xV = 150.0;
    yV = 0.0;
    zV = 0.0;
}

void PositionManager::init() {
    // Trouve la fenêtre du jeu avec le nom "OnlyUP  " à l'aide de la fonction FindWindow.
    HWND game_window = FindWindow(NULL, L"OnlyUP  ");
    // Vérifie si la fenêtre du jeu a été trouvée. Si ce n'est pas le cas, affiche un message d'erreur et termine le programme.
    if (!game_window) {
        std::cerr << "Impossible de trouver la fenêtre du jeu : " << GetLastError() << std::endl;
                system("pause");
    }

    // Déclare une variable pour stocker l'ID du processus du jeu.
    DWORD process_id = 0;
    // Obtient l'ID du processus du jeu à partir de la fenêtre du jeu.
    GetWindowThreadProcessId(game_window, &process_id);
    if (!process_id) {
        std::cerr << "Impossible d'obtenir l'ID du processus : " << GetLastError() << std::endl;
        system("pause");
    }

    // Ouvre le processus du jeu avec tous les droits d'accès.
    game_process = OpenProcess(PROCESS_ALL_ACCESS, true, process_id);
    if (!game_process) {
        std::cerr << "Impossible d'ouvrir le processus : " << GetLastError() << std::endl;
        system("pause");
    }

    const wchar_t* modName = L"OnlyUP-Win64-Shipping.exe";
    base_address = GetModuleBaseAddress(process_id, modName);


    initPos();
    initVelocity();
}

void PositionManager::initPos(){
    // Déclare une variable pour stocker l'adresse actuelle + ajoutez l'offset au début de l'adresse.
    uintptr_t current_address = base_address + 0x07356580;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;

    // Ajoute l'offset à l'adresse actuelle.
    current_address += 0x30;

    // Répète ce processus pour chaque offset.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0xA8;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x50;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0xA60;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0xB0;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
        std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x270;

    zCoord = current_address;
    current_address -= 0x8;
    yCoord = current_address;
    current_address -= 0x8;
    xCoord = current_address;
}

void PositionManager::initVelocity(){
    // Déclare une variable pour stocker l'adresse actuelle + ajoutez l'offset au début de l'adresse.
    uintptr_t current_address = base_address + 0x07356580;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;

    // Lit la mémoire du processus du jeu à l'adresse actuelle pour obtenir la prochaine adresse.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;

    // Ajoute l'offset à l'adresse actuelle.
    current_address += 0x30;

    // Répète ce processus pour chaque offset.
    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x150;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x60;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x5F0;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x10;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x8;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0x6B0;

    if (!ReadProcessMemory(game_process, (void*)current_address, &current_address, sizeof(current_address), nullptr)) {
                std::cerr << "Erreur lors de la lecture de la mémoire du processus : " << GetLastError() << std::endl;
                        system("pause");
    }
    //std::cout << "current_address: " << std::hex << current_address << std::endl;
    current_address += 0xC8;


    zVelocityCoord = current_address;
    current_address -= 0x8;
    yVelocityCoord = current_address;
    current_address -= 0x8;
    xVelocityCoord = current_address;
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
        qDebug() << "Erreur lors de l'ouverture du fichier :" << file.errorString();
        return;
    }

    file.write(doc.toJson());
    file.close();
}

void PositionManager::loadPositionsFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Erreur lors de l'ouverture du fichier :" << file.errorString();
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Erreur lors de la lecture du fichier JSON.";
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
        std::cerr << "Erreur lors de la création du snapshot de module : " << GetLastError() << std::endl;
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
