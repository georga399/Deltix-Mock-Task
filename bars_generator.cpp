#include "bars_generator.hpp"
#include <sstream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <list>

void BarsGenerator::run(std::istream &in_user, std::istream &in_market, std::ostream &out, ll p)
{
    std::string inputline;
    std::getline(in_user, inputline);
    std::getline(in_market, inputline);

    // start timestamp of transactions
    ll start_time = 1e15;

    // end timestamp of transactions
    ll end_time = 0;

    // divide by symbols by files
    std::unordered_map<std::string, std::list<market_entr>> sym_lists; // SYMBOL - list
    divide_by_symbols(in_market, sym_lists, start_time, end_time);

    // convert user deltas to the USD
    std::unordered_map<std::string, std::fstream> users_streams; // user_id- ostream
    convert_user_entries_to_usd(in_user, sym_lists, users_streams, start_time, end_time);

    // aggregate data by bars
    out << "user_id,minimum_balance,maximum_balance,average_balance,start_timestamp\n";
    for(auto& it: users_streams)
    {
        it.second.seekp(0, std::ios::beg);
        create_user_bar(out, p, it.second, start_time, end_time);
    }
    
}

// help function to read one user_entry from user_data.csv
bool BarsGenerator::read_user_entr(std::istream &stream, user_entr &uentr)
{
    std::string inputline;
    if(stream.eof())
        return false;
    std::getline(stream, inputline);
    if(inputline.empty())
        return false;
    std::stringstream linestream(inputline);
    std::string tbuf;
    std::getline(linestream, tbuf, ',');
    uentr.uid = tbuf;
    std::getline(linestream, tbuf, ',');
    uentr.sym = tbuf;
    std::getline(linestream, tbuf, ',');
    uentr.time = std::stoll(tbuf);
    std::getline(linestream, tbuf, ',');
    uentr.delta = std::stold(tbuf);
    return true;
}

// help function to read one market_entry from market_data.csv
bool BarsGenerator::read_market_entr(std::istream &stream, market_entr &entr)
{
    std::string inputline;
    if(stream.eof())
        return false;
    std::getline(stream, inputline);
    if(inputline.empty())
        return false;
    std::stringstream linestream(inputline);
    std::string tbuf;
    std::getline(linestream, tbuf, ',');
    entr.sym = tbuf.substr(0,3);
    std::getline(linestream, tbuf, ',');
    entr.timestamp = std::stoll(tbuf);
    std::getline(linestream, tbuf, ',');
    entr.price = std::stold(tbuf);
    return true;
}

// help function to divide file by symbols
void BarsGenerator::divide_by_symbols(std::istream &in_market, 
    std::unordered_map<std::string, std::list<market_entr>> &sym_lists,
    ll &start_time, ll &end_time)
{
    market_entr cur_m_entr;
    while(read_market_entr(in_market, cur_m_entr))
    {
        auto it_list = sym_lists.find(cur_m_entr.sym);
        if(it_list == sym_lists.end())
        {
            // sym_streams[cur_m_entr.sym] = std::fstream(std::string("./temp/syms/") + cur_m_entr.sym + ".csv", 
            //     std::ios::in|std::ios::out|std::ios::trunc);
            sym_lists[cur_m_entr.sym] = std::list<market_entr>();
        }
        // sym_streams.at(cur_m_entr.sym) << cur_m_entr.sym << ',' << cur_m_entr.timestamp << ',' << cur_m_entr.price << '\n'; 
        // end_time = std::max(end_time, cur_m_entr.timestamp);
        sym_lists.at(cur_m_entr.sym).push_back(cur_m_entr);
    }
}

// help function to convert users deltas to the usd
void BarsGenerator::convert_user_entries_to_usd(std::istream& in_user,
    std::unordered_map<std::string, std::list<market_entr>> &sym_lists, 
    std::unordered_map<std::string, std::fstream>& users_streams,
    ll &start_time, ll &end_time)
{
    std::unordered_map<std::string, std::list<market_entr>::iterator> sym_list_iters; // SYM - iterators in the according lists of sym_lists
    for(auto& it : sym_lists)
    {
        sym_list_iters[it.first] = it.second.begin();
    }
    // convert to the usd in userfiles
    std::unordered_map<std::string, std::pair<market_entr, market_entr>> instrument_changes; // SYM - <current market_entr, future market_entr>
    user_entr cur_u_entr;
    while(read_user_entr(in_user, cur_u_entr))
    {
        std::string sym = cur_u_entr.sym;
        ll time = cur_u_entr.time;
        std::string uid = cur_u_entr.uid;
        ld delta = cur_u_entr.delta;
        ld delta_in_usd;
        if(sym != "USD")
        {

            // getting current cost of SYMBOL
             while( ! (instrument_changes[sym].first.timestamp <= time 
                && time < instrument_changes[sym].second.timestamp ) )
            {
                // update market_changes
                instrument_changes[sym].first = instrument_changes[sym].second;
                if(sym_list_iters[sym] != sym_lists[sym].end())
                {
                    instrument_changes[sym].second = *(sym_list_iters[sym]);
                    ++sym_list_iters[sym];
                }
                else
                {
                    instrument_changes[sym].second = market_entr{sym, ll(1e15), 0};
                }
            }
            // convertation
            delta_in_usd = instrument_changes[sym].first.price*delta;
        }
        else
        {
            delta_in_usd = delta;
        }
        // saving to the stream
        auto it_stream = users_streams.find(uid);
        if(it_stream == users_streams.end())
        {
            users_streams[uid] = std::fstream(std::string("./temp/users/") + uid + ".csv",
                std::ios::in|std::ios::out|std::ios::trunc);
        }
        users_streams.at(uid) << uid << ',' << "USD" << ',' << time << ',' << delta_in_usd << '\n';  

        // getting start timestamp of transactions
        start_time = std::min(start_time, cur_u_entr.time);
        // getting end timestamp of transactions
        end_time = std::max(end_time, cur_u_entr.time);
    }
}

// help function to aggregate data by bars
void BarsGenerator::create_user_bar(std::ostream &out, ll p, std::istream &in,
    ll &start_time, ll &end_time)
{
    ll cur_start_bar = (start_time/p)*p; // start timestamp of current bar
    ll prev_u_entr_time = cur_start_bar; // previous timestamp of user entry
    ld calc = 0;
    ld min = 0;
    ld max = 0;
    ld avg = 0; // average
    ld int_avg = 0; // integrated average
    user_entr cur_u_entr;
    while(read_user_entr(in, cur_u_entr))
    {
        std::string uid = cur_u_entr.uid;
        ld delta = cur_u_entr.delta;
        ll time = cur_u_entr.time;
        if(time - cur_start_bar >= p)
        {
            // calculate avg before writing
            int_avg += (cur_start_bar + p - prev_u_entr_time)*calc;
            avg = int_avg/p;
            while(time - cur_start_bar >= p)
            {
                out << uid << ',' << min << ',' << max << ',' << avg << ',' << cur_start_bar << '\n';
                cur_start_bar += p;
            }
            min = std::min(calc, calc + delta);
            max = std::max(calc, calc + delta);

            // calc int_avg
            int_avg = 0;
            int_avg += (time-cur_start_bar)*calc;
            calc += delta;
        }
        else
        {
            int_avg += calc*(time - prev_u_entr_time);
            calc += delta;
            min = std::min(min, calc);
            max = std::max(max, calc);
        }
        prev_u_entr_time = time;
    }
    while(end_time - cur_start_bar >= p)
    {
        out << cur_u_entr.uid << ',' << min << ',' << max << ',' << avg << ',' << cur_start_bar << '\n';
        cur_start_bar += p;
    }
}
