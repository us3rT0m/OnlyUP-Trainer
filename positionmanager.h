#ifndef POSITIONMANAGER_H
#define POSITIONMANAGER_H

#include <QJsonObject>
#include <QString>
#include <QMap>
#include <Windows.h>

class PositionManager
{
public:
    PositionManager();

    void init();
    void createPosition(const QString& name);
    void savePositionsToFile(const QString& filename);
    void track();
    void teleport();
    void loadPos();
    double getX();
    double getY();
    double getZ();
    void deletePosition(const QString& name);
    void usePosition(const QString& name);
    QMap<QString, QJsonObject> getPositions() const;


private:
    HANDLE game_process;
    uintptr_t xCoord;
    uintptr_t yCoord;
    uintptr_t zCoord;
    uintptr_t zVelocity;
    uintptr_t yVelocity;
    uintptr_t xVelocity;
    double x;
    double y;
    double z;
    QMap<QString, QJsonObject> positions;

    uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
    void loadPositionsFromFile(const QString& filename);
};

#endif // POSITIONMANAGER_H
