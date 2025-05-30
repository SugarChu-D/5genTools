#ifndef CONST_H
#define CONST_H

#include <string>
#include <array>
#include <map>
#include <cstdint>

class Version {
private:
    std::string Label;
    std::array<uint32_t, 5> nazoArray;
    uint32_t VCount;
    void initializeValues();

public:
    Version(std::string label);
    std::string getLabel() const;
    std::array<uint32_t, 5> getNazoArray() const;
    uint32_t getVCount() const;
    void print() const;
};

#endif