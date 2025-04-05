#include "GameDate.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

using namespace std;

// コンストラクタ
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
    uint8_t m = month;
    if (m == 1 || m == 2) {
        if (year == 0) year = 94; // 2000年を扱う 挙動は94年と変わらないはず
        else year--;
        m += 12;
    }

    // ツェラーの公式を使用して曜日を計算
    int16_t h = (day + ((13*m + 8) / 5) + year + (year / 4)  ) % 7;
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
    cout << "Date: " << (2000 + year) << "-"
         << setw(2) << setfill('0') << static_cast<int>(month) << "-"
         << setw(2) << setfill('0') << static_cast<int>(day) << " "
         << setw(2) << setfill('0') << static_cast<int>(hour) << ":"
         << setw(2) << setfill('0') << static_cast<int>(minute) << ":"
         << setw(2) << setfill('0') << static_cast<int>(second)
         << " (Weekday: " << static_cast<int>(wday) << ")" << endl;
}

int main() {
    GameDate gameDate(60, 2, 28, 0, 0, 0); // 2060年2月28日
    for (int i = 0; i < 5; i++) {
        gameDate.print();
        gameDate.incrementDay();
    }
    return 0;
}