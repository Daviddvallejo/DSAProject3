#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <cmath>
using namespace std;

int main(){

    // Take input of stock value (integer) and timeframe in days (integer) separated by a space
    string input;
    cout << "Please input stock value and timeframe (in trading days): ";
    getline(cin, input);
    int stockVal = stoi(input.substr(0, input.find(' ')));
    int timeFrame = stoi(input.substr(input.find(' '), string::npos));

    // File to use
    string file = "100.csv";



    // FIXME: START CLOCK FOR MAP HERE
    // Map setup
    /*
     * Create map and store:
     *
     *      1) Days when price was below current price
     *      2) Days when price was at current price (tolerance of 0.5%)
     *      3) Days when price was above current price
     *
     * Keys:
     *       "Down" when price is below tolerance
     *       "At" when price is within tolerance
     *       "Up" when price is above tolerance
     * Values: A vector of pairs containing the day and the value
     */
    unordered_map<string, vector<pair<int, int>>> myMap;

    // Create input stream object, read in values and add to map accordingly
    ifstream inFS;
    inFS.open(file);

    int upperBound = ceil(stockVal + (stockVal / 200));
    int lowerBound = ceil(stockVal - (stockVal / 200));
    int day, val;
    string token;
    while (inFS) {
        inFS.clear();

        // Read line and separate values into usable variables
        getline(inFS, token, ',');
        day = stoi(token);
        getline(inFS, token);
        val = stoi(token);

        // Insert values into map
        if (val < lowerBound) {
            myMap["Down"].emplace_back(day, val);
        } else if (val > upperBound) {
            myMap["Up"].emplace_back(day, val);
        } else {
            myMap["At"].emplace_back(day, val);
        }

    }

    inFS.close();

    // Loop through days in "At" and find day either in "Down", "At", or "Up" that is exactly timeframe away from day in "At"
    // If/when a match is found, update average price.
    // Keep track of num times price went up, down, or stayed the same and also number of times a day timeframe away was looked for.


    // FIXME: CHANGE INNER FOR-LOOPS TO BINARY SEARCHES FOR DAYS TIMEFRAME AWAY
    double totalSum = 0;
    int matchedDays = 0;
    for (auto i : myMap["At"]) {
        for (auto x : myMap["Down"]) {
            if (x.first - i.first == timeFrame) {
                totalSum += x.second;
                matchedDays++;
                break;
            }
            if (x.first - i.first > timeFrame) {
                break;
            }
        }
        for (auto y : myMap["At"]){
            if (y.first - i.first == timeFrame) {
                totalSum += y.second;
                matchedDays++;
                break;
            }
            if (y.first - i.first > timeFrame) {
                break;
            }
        }
        for (auto z : myMap["Up"]) {
            if (z.first - i.first == timeFrame) {
                totalSum += z.second;
                matchedDays++;
                break;
            }
            if (z.first - i.first > timeFrame) {
                break;
            }
        }
    }

    // Calculate average for price of stock after timeframe for every instance the stock was at the current price and
    // the percent change
    double avgPrice = totalSum / matchedDays;
    double percentChange = (avgPrice - stockVal) / stockVal;

    // FIXME: WRITE FORMULA FOR PERCENT CONFIDENCE
    double confidence = 0;

    // Output results to screen
    cout << "The stock will change by " << percentChange << " in " << timeFrame << " trading days with a ";
    cout << confidence << "% certainty" << endl;

    // FIXME: END CLOCK FOR MAP HERE



    // Graph (Adjacency List) setup



    return 0;
}