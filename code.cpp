#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include <sstream>
#include "sha-1.cpp"

using namespace std;

//nazo値 AABBCCDD

const int nazo_Black_JPN = 0x2215f10;
const int nazo_White_JPN = 0x2215f30;

//VCount
const int VCount_B = 0x60;
const int VCount_W = 0x5f;

int main(){
	char Data[2048];
	SHA1_DATA SD1;
    
    char rom;int nazo;
    cout << "Choose your ROM b/w:"; cin >> rom;
    
    if(rom == 'b') nazo = nazo_Black_JPN;
    else if(rom == 'w') nazo = nazo_White_JPN;
    else {
        cout << "error: please insert 'b' or 'w'" << endl;
        return 0;
    }

        // nazo を 16進数文字列に変換して Data に格納
        stringstream ss;
        ss << hex << nazo;
        string nazoHex = ss.str();
        strncpy(Data, nazoHex.c_str(), sizeof(Data) - 1);
        Data[sizeof(Data) - 1] = '\0'; // 安全のため終端文字を追加
    

	cout << hex << nazo << endl;
    cout << nazo + 0xfc << endl;
    cout << nazo + 0xfc + 0x4c << endl;

    cout << "Data(under 2048 bite) = " << Data << endl;

    SHA1(&SD1,Data,0);
	cout << "SHA1 = " << SD1.Val_String << endl;
}