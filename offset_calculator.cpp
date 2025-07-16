#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include "sha-1.h" // For InitialSeedParams

// Probability Table from the article
const std::vector<std::vector<int>> ptable = {
    { 50, 100, 100, 100, 100 }, // L1
    { 50,  50, 100, 100, 100 }, // L2
    { 30,  50, 100, 100, 100 }, // L3
    { 25,  30,  50, 100, 100 }, // L4
    { 20,  25,  33,  50, 100 }, // L5
    { 100, 100, 100, 100, 100 }  // L6 (sentinel)
};

// LCG PRNG
uint32_t lcg_rand(uint64_t& seed) {
    seed = seed * 0x5D588B656C078965ULL + 0x269EC3ULL;
    return static_cast<uint32_t>(seed >> 32);
}

// Performs the probability table check
int pt_check(uint64_t& seed) {
    int consumed = 0;
    int level = 0;
    while (level < 5) { // Loop from L1 to L5
        int p_index = 0;
        while (true) {
            consumed++;
            uint32_t r = lcg_rand(seed);
            uint32_t v = static_cast<uint32_t>((static_cast<uint64_t>(r) * 101) >> 32);

            if (v <= ptable[level][p_index]) {
                level++;
                break;
            } else {
                p_index++;
                if (p_index >= ptable[level].size()) { 
                    level++; 
                    break;
                }
            }
        }
    }
    return consumed;
}

// Calculates the offset for 'Continue Game' mode
int calculate_continue_offset(uint64_t initial_seed_s0, const InitialSeedParams& params) {
    std::string game_version_str = params.ver.getLabel();
    auto it = valid_versions.find(game_version_str);
    if (it == valid_versions.end()) {
        throw std::runtime_error("Invalid or unsupported game version in InitialSeedParams.");
    }
    char version_type = it->second;

    uint64_t current_seed = initial_seed_s0;
    lcg_rand(current_seed); // S0 -> S1
    int offset = 1;

    if (version_type == '1') { // BW
        offset += pt_check(current_seed);
    } else if (version_type == '2') { // B2W2
        offset += pt_check(current_seed);
        offset += pt_check(current_seed);
    }

    return offset;
}


int main() {
    try {
        std::string seed_str;
        std::string game_version_str;

        std::cout << "Enter Initial Seed (S0) in hex (e.g., A0B1C2D3E4F50607): ";
        std::cin >> seed_str;

        std::cout << "Enter Game Version (e.g., JPB1, JPW2): ";
        std::cin >> game_version_str;

        uint64_t initial_seed_s0;
        try {
            initial_seed_s0 = std::stoull(seed_str, nullptr, 16);
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid hex string for initial seed.");
        }

        // Create dummy params for testing
        Version ver(game_version_str);
        InitialSeedParams params(ver, 0, false, 0);

        int offset = calculate_continue_offset(initial_seed_s0, params);

        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Calculated Offset (Continue Game): " << offset << std::endl;
        std::cout << "----------------------------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
