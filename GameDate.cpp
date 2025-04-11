#include "GameDate.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

using namespace std;

// コンストラクタ uint8_tを使用するためcinで入力を受け取るとASCIICodeが入るので注意
GameDate::GameDate(uint8_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t sec)
    : year(y), month(m), day(d), hour(h), minute(min), second(sec) {
    if (month < 1 || month > 12) {
        throw invalid_argument("Invalid month: " + to_string(month));
    }
    if (day < 1 || day > getDaysInMonth(month, year)) {
        throw invalid_argument("Invalid day: " + to_string(day));
    }
    calculateWeekday(); // 曜日を自動計算
}
// 曜日を計算するプライベートメソッド
void GameDate::calculateWeekday() {
    uint8_t y = year,m = month;
    if (m < 3 ) {
        y = (y == 0) ? 99 : y - 1;
        m += 12;
    }

    // ツェラーの公式を使用して曜日を計算
    int16_t h = (day + ((13*m + 8) / 5) + y + (y >> 2) ) % 7;
    wday = h;
}

// 月ごとの最大日数を取得
uint8_t GameDate::getDaysInMonth(uint8_t month, uint8_t year) const {
    static const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t days = daysInMonth[month - 1];
    if (month == 2 && (year % 4 == 0)) { // 閏年の2月は29日
        days = 29;
    }
    return days;
}

// 日付を1日進める
void GameDate::incrementDay() {
    if (day < getDaysInMonth(month, year)) {
        day++;
    } else {
        day = 1;
        if (month < 12) {
            month++;
        } else {
            month = 1;
            year = (year + 1) % 100; // 年を下2桁で管理
        }
    }
    calculateWeekday(); // 曜日を再計算
}

// sha1に渡すためのもの
uint32_t GameDate::getDate8Format() const {
    // 10進数を16進数に変換する処理を直接ビット演算で行う
    uint8_t hexYear = ((year / 10) << 4) | (year % 10);
    uint8_t hexMonth = ((month / 10) << 4) | (month % 10);
    uint8_t hexDay = ((day / 10) << 4) | (day % 10);

    // // デバッグ出力
    // cout << "Decimal year: " << static_cast<int>(year) 
    //      << " -> Hex: 0x" << hex << static_cast<int>(hexYear) << endl;
    // cout << "Decimal month: " << static_cast<int>(month) 
    //      << " -> Hex: 0x" << hex << static_cast<int>(hexMonth) << endl;
    // cout << "Decimal day: " << static_cast<int>(day) 
    //      << " -> Hex: 0x" << hex << static_cast<int>(hexDay) << endl;

    // 1回のビット演算で結合
    return (static_cast<uint32_t>(hexYear) << 24) | 
           (static_cast<uint32_t>(hexMonth) << 16) | 
           (static_cast<uint32_t>(hexDay) << 8) | 
           wday;
}

uint32_t GameDate::getTime9Format() const {
    // 各値を10進数から16進数として解釈
    uint8_t hexHour = decimalToHex(hour + (hour > 11 ? 40 : 0));
    uint8_t hexMinute = decimalToHex(minute);
    uint8_t hexSecond = decimalToHex(second);

    // // デバッグ出力
    // cout << "Decimal hour: " << static_cast<int>(hour) 
    //      << " -> Hex: 0x" << hex << static_cast<int>(hexHour) << endl;
    // cout << "Decimal minute: " << static_cast<int>(minute) 
    //      << " -> Hex: 0x" << hex << static_cast<int>(hexMinute) << endl;
    // cout << "Decimal second: " << static_cast<int>(second) 
    //      << " -> Hex: 0x" << hex << static_cast<int>(hexSecond) << endl;

    // 時、分、秒を結合
    return (static_cast<uint32_t>(hexHour) << 24) | 
           (static_cast<uint32_t>(hexMinute) << 16) | 
           (static_cast<uint32_t>(hexSecond) << 8) | 
           0x00;
}

// ゲッター
uint8_t GameDate::getYear() const { return year; }
uint8_t GameDate::getMonth() const { return month; }
uint8_t GameDate::getDay() const { return day; }
uint8_t GameDate::getHour() const { return hour; }
uint8_t GameDate::getMinute() const { return minute; }
uint8_t GameDate::getSecond() const { return second; }
uint8_t GameDate::getWeekday() const { return wday; }

// デバッグ用の出力
void GameDate::print() const {
    cout << "Date: 20" << dec <<  static_cast<int>(year) << "-"
         << setw(2) << setfill('0') << static_cast<int>(month) << "-"
         << setw(2) << setfill('0') << static_cast<int>(day) << " "
         << setw(2) << setfill('0') << static_cast<int>(hour) << ":"
         << setw(2) << setfill('0') << static_cast<int>(minute) << ":"
         << setw(2) << setfill('0') << static_cast<int>(second)
         << " (Weekday: " << static_cast<int>(wday) << ")" << endl;
}

// int main() {
//     try {
//         // GameDateオブジェクトを作成
//         GameDate gameDate(60, 3, 23, 13, 1, 18);
//         gameDate.print();

//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//         return 1;
//     }

//     return 0;
// }