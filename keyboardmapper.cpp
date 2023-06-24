#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <Psapi.h>

#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "keyboardmapper.h"

HHOOK KeyboardMapper::sHook = nullptr;
std::map<int, int> KeyboardMapper::sKeysMap;
std::vector<int> KeyboardMapper::sIgnoreKeys;

KeyboardMapper::KeyboardMapper()
{
    QFile file("keys.json");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de l'ouverture du fichier \"keys.json\".");
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de la lecture du fichier \"keys.json\".");
        return;
    }

    QJsonObject root = jsonDoc.object();

    for (const QString& oldKey : root.keys()) {
        auto newKey = root.value(oldKey).toString();
        sKeysMap.insert({
            keyNameToVk(newKey.toStdString()),
            keyNameToVk(oldKey.toStdString())
        });
        sIgnoreKeys.push_back(keyNameToVk(oldKey.toStdString()));
    }


    KeyboardMapper::sHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardMapper::hookFunction, nullptr, 0);
}

int KeyboardMapper::keyNameToVk(const std::string& keyName) const noexcept
{
    std::map<std::string, int> keysDict({
        {"LeftButton", 1},
        {"RightButton" , 2},
        {"Cancel" , 3},
        {"MiddleButton" , 4},
        {"ExtraButton1" , 5},
        {"ExtraButton2" , 6},
        {"Back" , 8},
        {"Tab" , 9},
        {"Clear" , 12},
        {"Return" , 13},
        {"Shift" , 16},
        {"Control" , 17},
        {"Menu" , 18},
        {"Pause" , 19},
        {"CapsLock" , 20},
        {"Escape" , 0x1B},
        {"Convert" , 0x1C},
        {"NonConvert" , 0x1D},
        {"Accept" , 0x1E},
        {"ModeChange" , 0x1F},
        {"Space" , 0x20},
        {"Prior" , 0x21},
        {"Next" , 0x22},
        {"End" , 0x23},
        {"Home" , 0x24},
        {"Left" , 0x25},
        {"Up" , 0x26},
        {"Right" , 0x27},
        {"Down" , 0x28},
        {"Select" , 0x29},
        {"Print" , 0x2A},
        {"Execute" , 0x2B},
        {"Snapshot" , 0x2C},
        {"Insert" , 0x2D},
        {"Delete" , 0x2E},
        {"Help" , 0x2F},
        {"N0" , 0x30},
        {"N1" , 0x31},
        {"N2" , 0x32},
        {"N3" , 0x33},
        {"N4" , 0x34},
        {"N5" , 0x35},
        {"N6" , 0x36},
        {"N7" , 0x37},
        {"N8" , 0x38},
        {"N9" , 0x39},
        {"A" , 0x41},
        {"B" , 0x42},
        {"C" , 0x43},
        {"D" , 0x44},
        {"E" , 0x45},
        {"F" , 0x46},
        {"G" , 0x47},
        {"H" , 0x48},
        {"I" , 0x49},
        {"J" , 0x4A},
        {"K" , 0x4B},
        {"L" , 0x4C},
        {"M" , 0x4D},
        {"N" , 0x4E},
        {"O" , 0x4F},
        {"P" , 0x50},
        {"Q" , 0x51},
        {"R" , 0x52},
        {"S" , 0x53},
        {"T" , 0x54},
        {"U" , 0x55},
        {"V" , 0x56},
        {"W" , 0x57},
        {"X" , 0x58},
        {"Y" , 0x59},
        {"Z" , 0x5A},
        {"LeftWindows" , 0x5B},
        {"RightWindows" , 0x5C},
        {"Application" , 0x5D},
        {"Numpad1" , 0x61},
        {"Numpad2" , 0x62},
        {"Numpad3" , 0x63},
        {"Numpad4" , 0x64},
        {"Numpad5" , 0x65},
        {"Numpad0" , 0x60},
        {"Numpad6" , 0x66},
        {"Numpad7" , 0x67},
        {"Numpad8" , 0x68},
        {"Numpad9" , 0x69},
        {"Multiply" , 0x6A},
        {"Add" , 0x6B},
        {"Separator" , 0x6C},
        {"Subtract" , 0x6D},
        {"Decimal" , 0x6E},
        {"Divide" , 0x6F},
        {"F1" , 0x70},
        {"F2" , 0x71},
        {"F3" , 0x72},
        {"F4" , 0x73},
        {"F5" , 0x74},
        {"F6" , 0x75},
        {"F7" , 0x76},
        {"F8" , 0x77},
        {"F9" , 0x78},
        {"F10" , 0x79},
        {"F11" , 0x7A},
        {"F12" , 0x7B},
        {"F13" , 0x7C},
        {"F14" , 0x7D},
        {"F15" , 0x7E},
        {"F16" , 0x7F},
        {"F17" , 0x80},
        {"F18" , 0x81},
        {"F19" , 0x82},
        {"F20" , 0x83},
        {"F21" , 0x84},
        {"F22" , 0x85},
        {"F23" , 0x86},
        {"F24" , 0x87},
        {"LeftShift" , 0xA0},
        {"RightShift" , 0xA1},
        {"LeftControl" , 0xA2},
        {"RightControl" , 0xA3},
        {"LeftMenu" , 0xA4},
        {"RightMenu", 0xA5}
    });

    return keysDict[keyName];
}

LRESULT CALLBACK KeyboardMapper::hookFunction(int code, WPARAM wParam, LPARAM lParam )
{
    HWND windowHandle = GetForegroundWindow();
    DWORD pid;
    GetWindowThreadProcessId(windowHandle, &pid);

    TCHAR name[256];
    HANDLE procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (procHandle == INVALID_HANDLE_VALUE)
        return 0;

    GetModuleBaseNameA(procHandle, nullptr, name, 256);

    if (std::string(name) != "OnlyUP-Win64-Shipping.exe") {
        return 0;
    }


    KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
    INPUT inputs[1];

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wScan = 0;
    inputs[0].ki.dwFlags = 0;
    inputs[0].ki.time = 0;
    inputs[0].ki.dwExtraInfo = 0;

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

    if (sKeysMap.count(kbd->vkCode) != 0 && (kbd->flags & LLKHF_INJECTED) == 0) {
        int key = sKeysMap[kbd->vkCode];
        inputs[0].ki.wVk = key;
        SendInput(1, inputs, sizeof (INPUT));
        return 1;
    } else if(std::find(KeyboardMapper::sIgnoreKeys.begin(), KeyboardMapper::sIgnoreKeys.end(), kbd->vkCode) != KeyboardMapper::sIgnoreKeys.end() && (kbd->flags & LLKHF_INJECTED) == 0) {
        return 1; // ignore this key
    }

    return CallNextHookEx(sHook, code, wParam, lParam);
}

