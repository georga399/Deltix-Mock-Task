#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <list>

#define HOUR 3600
#define DAY 24*3600
#define MONTH 30*24*3600

typedef long long ll;
typedef long double ld;

class BarsGenerator final
{
public:

    // function to proccess aggregation
    static void run(std::istream& in_user, std::istream& in_market, std::ostream& out, ll p);

private:

    // entry in the user_data.csv
    struct user_entr
    {
        std::string uid;
        ll time;
        ld delta;
        std::string sym;
    };

    // entry in the market_data.csv
    struct market_entr
    {
        std::string sym;
        ll timestamp;
        ld price;
    };    

    // help function to read one user_entry from user_data.csv
    static bool read_user_entr(std::istream &stream, user_entr &uentr);

    // help function to read one market_entry from market_data.csv
    static bool read_market_entr(std::istream &stream, market_entr &entr);

    // help function to divide file by symbols
    static void divide_by_symbols(std::istream &in_market, 
        std::unordered_map<std::string, std::list<market_entr>> &sym_lists, 
        ll &start_time, ll &end_time);

    // help function to convert users deltas to the usd
    static void convert_user_entries_to_usd(std::istream &in_user, 
        std::unordered_map<std::string, std::list<market_entr>> &sym_lists, 
        std::unordered_map<std::string, std::fstream> &users_streams,
        ll &start_time, ll &end_time);

    // help function to calculate and write of one user to the file
    static void create_user_bar(std::ostream &out, ll p, std::istream &in,
        ll &start_time, ll &end_time);

};

