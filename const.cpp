#include <iostream>
#include <vector>
using namespace std;

static class Version{
    string Label;
    vector<unsigned int> NazoValues;
    unsigned int VCount;
    vector<unsigned int> DefauleTimer0Range;
public:
    Version(string label, vector<unsigned int> nazo, unsigned int vcount, vector<unsigned int> DTimer0){
        label = Label;
        VCount = vcount;
    };
};
