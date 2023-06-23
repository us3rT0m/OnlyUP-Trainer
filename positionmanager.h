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

    int init(HWND gameWindow);
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
    void resetDrake();
    void pauseDrake();
    void speedUpDrake();
    void speedDownDrake();
    void resetSpeedDrake();
    void setFlyForwards(bool flyForward);
    void setFlyBackwards(bool flyBackward);
    void setFlyUp(bool flyUp);
    void setFlyDown(bool flyDown);
    void setFlyLeft(bool flyLeft);
    void setFlyRight(bool flyRight);
    void setHovering(bool hovering);
    void disableFlyMode();

private:
    HWND game_window;
    HANDLE game_process;
    uintptr_t xCoord;
    uintptr_t yCoord;
    uintptr_t zCoord;
    uintptr_t rotCoord;
    uintptr_t zVelocityCoord;
    uintptr_t yVelocityCoord;
    uintptr_t xVelocityCoord;
    uintptr_t drakeDistSplineCoord;
    uintptr_t drakeMouvementCoord;
    double x;
    double y;
    double z;
    double xV;
    double yV;
    double zV;
    float drakeDistSpline;
    float drakeMouvement;
    QMap<QString, QJsonObject> positions;
    int initPos();
    int initVelocity();
    int initDrake();
    uintptr_t base_address;
    bool flyForwards;
    bool flyBackwards;
    bool flyUp;
    bool flyDown;
    bool flyLeft;
    bool flyRight;
    bool hovering;

    void updateFly();

    uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
    void loadPositionsFromFile(const QString& filename);
    void isDrakeInit();
    bool DrakeInit;
};

#endif // POSITIONMANAGER_H
