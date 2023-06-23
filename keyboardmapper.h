#ifndef KEYBOARDMAPPER_H
#define KEYBOARDMAPPER_H

#include <Windows.h>
#include <string>
#include <map>
#include <vector>

class KeyboardMapper
{
public:
    KeyboardMapper();
    static LRESULT CALLBACK hookFunction(int code, WPARAM wParam, LPARAM lParam);
    int keyNameToVk(const std::string& keyName) const noexcept;


private:
    static HHOOK sHook;
    static std::map<int, int> sKeysMap;
    static std::vector<int> sIgnoreKeys;
};

#endif // KEYBOARDMAPPER_H
