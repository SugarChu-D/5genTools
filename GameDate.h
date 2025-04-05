#ifndef GAMEDATE_H
#define GAMEDATE_H

#include <cstdint> // uint8_tを使用するために必要
#include <iostream>
#include <stdexcept>
#include <string>

class GameDate {
private:
    uint8_t year;   // 年（範囲: 0-99）
    uint8_t month;   // 月（範囲: 1-12）
    uint8_t day;     // 日（範囲: 1-31）
    uint8_t hour;    // 時（範囲: 0-23）
    uint8_t minute;  // 分（範囲: 0-59）
    uint8_t second;  // 秒（範囲: 0-59）
    uint8_t wday;    // 曜日（範囲: 0-6, 0=日曜, 6=土曜）

    void calculateWeekday(); // 曜日を計算するプライベートメソッド
    uint8_t getDaysInMonth(uint8_t month, uint8_t year) const; // 月ごとの最大日数を取得
    bool isLeapYear(uint16_t fullYear) const; // 閏年判定

public:
    // コンストラクタ
    GameDate(uint8_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t sec);

    // ゲッター
    uint8_t getYear() const;
    uint8_t getMonth() const;
    uint8_t getDay() const;
    uint8_t getHour() const;
    uint8_t getMinute() const;
    uint8_t getSecond() const;
    uint8_t getWeekday() const;

    void incrementDay(); // 日付を1日進める
    void print() const;  // デバッグ用の出力
};

#endif // GAMEDATE_H