#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdint>
using namespace std;

/*キー入力について
デフォルトは0x2fff　0b1011-1111-1111-1111
ここからボタンが入力されるたびに1桁ずつ1が0になっている．
左から　Y　X　L　R　下　上　左　右　Start Select　B　Aという順番に対応している．
sha-1に入れるときはエンディアン変換されており，2ffd(Bボタン入力)がfd2fになる．
初期seedを計算するならこれを全探索してもらうのが早いだろう．
ただし，真ん中にある0010と，上下左右反対のキーの同時押し・ソフトリセットの禁忌をうまいこと外してあげる必要があるので，その実装がめんどくさい．
何か知らんけどforループで回すと桁の右から順番に計算してくるので，for文を逆に回すことで対策としている．
*/

// 各ビットに対応するキー名
const vector<string> bitToKey = {
    "A", "B", "Select", "Start","Right", "Left", "Up", "Down", "R", "L", "X", "Y"
};

// ビット位置の定数定義
const int BIT_A = 0;
const int BIT_B = 1;
const int BIT_SELECT = 2;
const int BIT_START = 3;
const int BIT_RIGHT = 4;
const int BIT_LEFT = 5;
const int BIT_UP = 6;
const int BIT_DOWN = 7;
const int BIT_R = 8;
const int BIT_L = 9;
const int BIT_X = 10;
const int BIT_Y = 11;

// KeypressManager クラス
class KeypressManager {
private:
    vector<uint16_t> valid_keypresses;
    
    // 無効なキー組み合わせをチェックする関数
    bool isValidKeypress(uint16_t keypress) {
        // 上下同時押しチェック（両方のビットが0の場合は無効）
        bool up_pressed = !(keypress & (1 << BIT_UP));
        bool down_pressed = !(keypress & (1 << BIT_DOWN));
        if (up_pressed && down_pressed) {
            return false;
        }
        
        // 左右同時押しチェック（両方のビットが0の場合は無効）
        bool left_pressed = !(keypress & (1 << BIT_LEFT));
        bool right_pressed = !(keypress & (1 << BIT_RIGHT));
        if (left_pressed && right_pressed) {
            return false;
        }
        
        // 必要に応じて他の無効な組み合わせもここに追加
        // 例：ソフトリセット（Select + Start + L + R）の除外
        bool select_pressed = !(keypress & (1 << BIT_SELECT));
        bool start_pressed = !(keypress & (1 << BIT_START));
        bool l_pressed = !(keypress & (1 << BIT_L));
        bool r_pressed = !(keypress & (1 << BIT_R));
        if (select_pressed && start_pressed && l_pressed && r_pressed) {
            return false;
        }
        
        return true;
    }

public:
    // コンストラクタ：指定範囲の有効なkeypressを生成
    KeypressManager(uint16_t start = 0x2000, uint16_t end = 0x2fff) {
        valid_keypresses.reserve(end - start + 1);
        
        for (uint16_t keypress = end; keypress >= start ; --keypress) {
            if (isValidKeypress(keypress)) {
                valid_keypresses.push_back(keypress);
            }
        }
        
        // デバッグ情報出力
        int total = end - start + 1;
        int valid = valid_keypresses.size();
        int invalid = total - valid;
        
        cout << "KeypressManager initialized:" << endl;
        cout << "  Total keypresses: " << total << endl;
        cout << "  Valid keypresses: " << valid << endl;
        cout << "  Invalid keypresses: " << invalid << " (" 
             << fixed << setprecision(2) << (100.0 * invalid / total) << "%)" << endl;
    }
    
    // 有効なkeypress数を取得
    size_t size() const {
        return valid_keypresses.size();
    }
    
    // インデックスでアクセス
    uint16_t operator[](size_t index) const {
        return valid_keypresses[index];
    }
    
    // イテレータサポート
    vector<uint16_t>::const_iterator begin() const {
        return valid_keypresses.begin();
    }
    
    vector<uint16_t>::const_iterator end() const {
        return valid_keypresses.end();
    }
    
    // 統計情報取得
    void printStats(uint16_t start = 0x2000, uint16_t end = 0x2fff) const {
        int total = end - start + 1;
        int valid = valid_keypresses.size();
        int invalid = total - valid;
        
        cout << "Keypress Statistics:" << endl;
        cout << "  Range: 0x" << hex << start << " - 0x" << end << dec << endl;
        cout << "  Total: " << total << endl;
        cout << "  Valid: " << valid << endl;
        cout << "  Invalid: " << invalid << " (" 
             << fixed << setprecision(2) << (100.0 * invalid / total) << "%)" << endl;
        cout << "  Performance gain: " << fixed << setprecision(2) 
             << (100.0 * invalid / total) << "% reduction in iterations" << endl;
    }
};

// 16ビット数値を解析して対応するキーを取得
vector<string> getKeysFromBits(unsigned short bits) {
    vector<string> keys;
    for (int i = 0; i < 12; i++) {
        if (!(bits & (1 << i))) {
            keys.push_back(bitToKey[i]);
        }
    }
    return keys;
}

// グローバルなKeypressManagerインスタンス
static KeypressManager keypressManager;

// int main() {
//     // テスト用コード
//     keypressManager.printStats();
    
//     cout << "\nFirst 10 valid keypresses:" << endl;
//     for (size_t i = 0; i < min(static_cast<size_t>(10), keypressManager.size()); ++i) {
//         uint16_t keypress = keypressManager[i];
//         vector<string> keys = getKeysFromBits(keypress);
        
//         cout << "0x" << hex << keypress << ": ";
//         for (const string& key : keys) {
//             cout << key << " ";
//         }
//         cout << endl;
//     }
    
//     return 0;
// }