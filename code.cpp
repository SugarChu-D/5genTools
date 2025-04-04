#include <iostream>
#include <cstdio>
#include <cstring>
#include "sha-1.cpp"
#include "const.cpp"

using namespace std;

int main() {
    char Data[2048] = {0}; // 初期化
    SHA1_DATA SD1;

    string rom;
    cout << "Choose your ROM JPB1/JPW1/JPB2/JPW2: ";
    cin >> rom;

    uint16_t Timer0;
    cout << "Timer0: ";
    cin >> hex >> Timer0;

    bool isDSLite = false;
    cout << "Is this DSLite? (0: No, 1: Yes): ";
    cin >> isDSLite;

    uint64_t GxFrameXorFrame = isDSLite ? 0x6000006 : 0x6000008;

    uint64_t MAC;
    cout << "MAC: ";
    cin >> hex >> MAC;

    uint16_t year, month, day, hour, minute, second;
    cout << "Enter year under 2: ";
    cin >> year;
    cout << "Enter month: ";
    cin >> month;
    cout << "Enter day: ";
    cin >> day;
    cout << "Enter hour: ";
    cin >> hour;
    cout << "Enter minute: ";
    cin >> minute;
    cout << "Enter second: ";
    cin >> second;

    tm time_in = {second, minute, hour, day, month - 1, year + 100};
    mktime(&time_in);

    try {
        Version ver(rom);
        size_t data_len = 0;

        // Data 構築
        data_len += sprintf(Data + data_len, "%s", ver.getNazo().c_str());
        data_len += sprintf(Data + data_len, "%04x", Timer0);
        data_len += sprintf(Data + data_len, "%02x", ver.getVCount());
        data_len += sprintf(Data + data_len, "000000");
        data_len += sprintf(Data + data_len, "%04x", MAC & 0xFFFF);
        data_len += sprintf(Data + data_len, "%08x", (MAC >> 16) ^ GxFrameXorFrame);
        data_len += sprintf(Data + data_len, "%02x%02x%02x%02x", year, month, day, time_in.tm_wday);
        data_len += sprintf(Data + data_len, "%02x%02x%02x", hour + (hour > 11 ? 0x40 : 0), minute, second);
        data_len += sprintf(Data + data_len, "000000000000000000ff2f0000");

        Data[data_len] = '\0';

        cout << "Data = " << Data << endl;

        // SHA1 ハッシュ計算
        SHA1(&SD1, Data, 0);
        cout << "SHA1 = " << SD1.Val_String << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}