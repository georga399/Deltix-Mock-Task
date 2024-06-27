#include <iostream>
#include <fstream>
#include "bars_generator.hpp"

int main()
{
    std::ifstream in_market("./market_data.csv");
    std::ifstream in_user("./user_data.csv");

    std::ofstream out_bar_1h("./bars-1h.csv");
    std::ofstream out_bar_1d("./bars-1d.csv");
    std::ofstream out_bar_30d("./bars-30d.csv");

    std::cout << "_____START PROCESSING_____" << std::endl;
    auto time_before = std::chrono::system_clock::now();
    // Process
    BarsGenerator::run(in_user, in_market, out_bar_1h, MONTH);
    auto time_after = std::chrono::system_clock::now(); 
    std::cout << "Time of proccess to create bars of 1h: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before).count() 
        << " ms" << std::endl;

    in_user.clear();
    in_user.seekg(0, std::ios::beg);
    in_market.clear();
    in_market.seekg(0, std::ios::beg);
    time_before = std::chrono::system_clock::now();
    // Process
    BarsGenerator::run(in_user, in_market, out_bar_1d, MONTH);
    time_after = std::chrono::system_clock::now(); 
    std::cout << "Time of proccess to create bars of 1d: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before).count() 
        << " ms" << std::endl;

    in_user.clear();
    in_user.seekg(0, std::ios::beg);
    in_market.clear();
    in_market.seekg(0, std::ios::beg);
    time_before = std::chrono::system_clock::now();
    // Process
    BarsGenerator::run(in_user, in_market, out_bar_30d, MONTH);
    time_after = std::chrono::system_clock::now(); 
    std::cout << "Time of proccess to create bars of 30d: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before).count() 
        << " ms" << std::endl;
}