#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cstdint>
using namespace std;

// ビット割り当て（左から Y X L R Down Up Left Right Start Select B A）
constexpr int BIT_A = 0;
constexpr int BIT_B = 1;
constexpr int BIT_SELECT = 2;
constexpr int BIT_START = 3;
constexpr int BIT_RIGHT = 4;
constexpr int BIT_LEFT = 5;
constexpr int BIT_UP = 6;
constexpr int BIT_DOWN = 7;
constexpr int BIT_R = 8;
constexpr int BIT_L = 9;
constexpr int BIT_X = 10;
constexpr int BIT_Y = 11;

constexpr uint16_t KEY_NONE = 0x2fff;

// 無効な組み合わせのビットマスク
constexpr uint16_t INVALID_UP_DOWN = (1 << BIT_UP) | (1 << BIT_DOWN);
constexpr uint16_t INVALID_LEFT_RIGHT = (1 << BIT_LEFT) | (1 << BIT_RIGHT);
constexpr uint16_t INVALID_SOFT_RESET = (1 << BIT_SELECT) | (1 << BIT_START) | (1 << BIT_L) | (1 << BIT_R);

// 12ビット分の各ビット割り当て例（仮）
// 0: A, 1: B, 2: Select, 3: Start, 4: Right, 5: Left, 6: Up, 7: Down, 8: R, 9: L, 10: X, 11: Y

// 例: keypressは0x2000～0x2fffの値
bool is_valid(uint16_t keypress) {
    // ビット反転（0:押してない, 1:押してる になるように）
    uint16_t k = ~keypress;

    // 上下同時押し
    if ((k & (1 << 6)) && (k & (1 << 7))) return false;
    // 左右同時押し
    if ((k & (1 << 4)) && (k & (1 << 5))) return false;
    // Start+Select+L+R同時押し
    if ((k & (1 << 3)) && (k & (1 << 2)) && (k & (1 << 9)) && (k & (1 << 8))) return false;

    return true;
}

int main() {
    vector<uint16_t> valid_keypresses;
    // 0x2fff～0x2000を降順で全探索
    for (uint16_t k = 0x2fff; k >= 0x2000; --k) {
        if (is_valid(k)) valid_keypresses.push_back(k);
    }
    cout << "Found " << valid_keypresses.size() << " valid keypresses" << endl;
    ofstream array_file("keypresses_array_generated.cpp");
    array_file << "#include <iostream>\n";
    array_file << "#include <vector>\n";
    array_file << "#include <string>\n";
    array_file << "#include <iomanip>\n";
    array_file << "#include <cstdint>\n";
    array_file << "using namespace std;\n\n";
    array_file << "constexpr const char* KEY_NAMES[] = {\n";
    array_file << "    \"A\", \"B\", \"Select\", \"Start\", \"Right\", \"Left\", \"Up\", \"Down\", \"R\", \"L\", \"X\", \"Y\"\n";
    array_file << "};\n\n";
    array_file << "constexpr uint16_t VALID_KEYPRESSES[] = {\n";
    for (size_t i = 0; i < valid_keypresses.size(); ++i) {
        if (i % 16 == 0) array_file << "    ";
        array_file << "0x" << hex << valid_keypresses[i];
        if (i < valid_keypresses.size() - 1) array_file << ", ";
        if ((i + 1) % 16 == 0 || i == valid_keypresses.size() - 1) array_file << "\n";
    }
    array_file << "};\n\n";
    array_file << "constexpr size_t VALID_KEYPRESS_COUNT = " << valid_keypresses.size() << ";\n\n";
    array_file << "inline vector<string> getKeysFromBits(uint16_t bits) {\n";
    array_file << "    vector<string> keys;\n";
    array_file << "    keys.reserve(12);\n";
    array_file << "    for (int i = 0; i < 12; i++) {\n";
    array_file << "        if (!(bits & (1 << i))) {\n";
    array_file << "            keys.emplace_back(KEY_NAMES[i]);\n";
    array_file << "        }\n";
    array_file << "    }\n";
    array_file << "    return keys;\n";
    array_file << "}\n\n";
    array_file << "void printKeypressStats() {\n";
    array_file << "    cout << \"Array-based Keypress Statistics:\" << endl;\n";
    array_file << "    cout << \"  Total valid keypresses: \" << VALID_KEYPRESS_COUNT << endl;\n";
    array_file << "    cout << \"  Memory usage: \" << (sizeof(VALID_KEYPRESSES) / 1024.0) << \" KB\" << endl;\n";
    array_file << "    cout << \"  Performance: O(1) direct array access\" << endl;\n";
    array_file << "}\n";
    array_file.close();
    cout << "Generated keypresses_array_generated.cpp with " << valid_keypresses.size() << " valid keypresses" << endl;
    cout << "Memory usage: " << (valid_keypresses.size() * sizeof(uint16_t) / 1024.0) << " KB" << endl;
    return 0;
} 