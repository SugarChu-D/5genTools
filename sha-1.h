#ifndef SHA1_H
#define SHA1_H

#include "GameDate.h"
#include "const.h"
#include <cstdint>

struct SHA1_DATA {
    uint32_t Value[5];     // 従来のハッシュ値配列
    uint64_t Iseed;        // 上位16バイトを64ビット値として保持
    char Val_String[50];   // 文字列表現
};

// InitialSeedParamsの定義を追加
struct InitialSeedParams {
    const Version& ver;
    uint16_t Timer0;
    bool isDSLite;
    uint64_t MAC;
    
    InitialSeedParams(
        const Version& v, 
        uint16_t t0, 
        bool isLite, 
        uint64_t mac
    ) : ver(v), Timer0(t0), isDSLite(isLite), MAC(mac) {}
};

// SHA1関数の宣言
bool SHA1(SHA1_DATA* sha1d, const char* data, unsigned int size);
uint64_t initialSEED(SHA1_DATA* sha1d, 
                     const InitialSeedParams& params, 
                     const GameDate& gameDate, 
                     uint16_t keypress);

#endif // SHA1_H