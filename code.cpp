#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <iostream>

using namespace std;

//nazo値 AABBCCDD

const int nazo_Black_JPN = 0x2215f10;
const int nazo_White_JPN = 0x2215f30;

//VCount
const int VCount_B = 0x60;
const int VCount_W = 0x5f;

int main(){
    
    char rom;int nazo;
    cout << "Choose your ROM b/w:"; cin >> rom;
    
    if(rom == 'b') nazo = nazo_Black_JPN;
    else if(rom == 'w') nazo = nazo_White_JPN;
    else {
        cout << "error: please insert 'b' or 'w'" << endl;
        return 0;
    }
    
	cout << hex << nazo << endl;
    cout << nazo + 0xfc << endl;
    cout << nazo + 0xfc + 0x4c << endl;

}