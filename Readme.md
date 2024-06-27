# Deltix Mock Task
Statements are in the pdf file.

To run program in workdir run command:
```md
make
```
The user_data.csv and market_data.csv files should be located in the workdir.

### Algorithm:
- divide file market_data.csv by symbols -- BarsGenerator::divide_by_symbols()
- convert user entries in the user_data.csv for the usd currency -- BarsGenerator::divide_by_symbols()
- aggregate in converted user entities by time blocks (bars) - -BarsGenerator::create_user_bar()

### Asymptotics
**O(M + N + M + N + (MaxTS - MinTS)/S)**, M -- count of lines in the market_data.csv, N -- count of lines in the user_data.csv, MaxTS -- maximum value of timestamp in the all data, MinTS -- minimum timestamp in the all data,  S -- size of bars in seconds.
- BarsGenerator::divide_by_symbols() -- O(M)
- BarsGenerator::divide_by_symbols() -- O(N + M)
- BarsGenerator::create_user_bar() -- O(N + (MaxTS - MinTS)/S)