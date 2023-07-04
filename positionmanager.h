#ifndef POSITIONMANAGER_H
#define POSITIONMANAGER_H

#include <QJsonObject>
#include <QString>
#include <QMap>
#include <Windows.h>
#include <thread>
#include <atomic>

class PositionManager
{
public:
    PositionManager();

    int init();
    HWND getGameWindow();
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
    void updateVelocity(double x, double y, double z);
    void setFlyHack(bool isFlyHack);
    bool getFlyHack();
    float getFps();
    void setFps(float newFps);
    void setJumpZVelocity(bool bigJump);

private:
    HANDLE game_process;
    HWND game_window;
    uintptr_t xCoord;
    uintptr_t yCoord;
    uintptr_t zCoord;
    uintptr_t rotationCoord;
    uintptr_t movementModeCoord;
    uintptr_t maxFlySpeedCoord;
    uintptr_t brakingDecelerationFlyingCoord;
    uintptr_t airControlCoord;
    uintptr_t jumpZVelocityCoord;
    uintptr_t zVelocityCoord;
    uintptr_t yVelocityCoord;
    uintptr_t xVelocityCoord;
    uintptr_t drakeDistSplineCoord;
    uintptr_t drakeMouvementCoord;
    uintptr_t fpsCoord;
    double x;
    double y;
    double z;
    double xV;
    double yV;
    double zV;
    float drakeDistSpline;
    float drakeMouvement;
    float fps;
    QMap<QString, QJsonObject> positions;
    int findGameWindow();
    int initPos();
    int initVelocity();
    int initDrake();
    int initFps();
    uintptr_t base_address;

    bool flyHack;

    uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
    void loadPositionsFromFile(const QString& filename);
    void isDrakeInit();
    bool DrakeInit;
};

#endif // POSITIONMANAGER_H
