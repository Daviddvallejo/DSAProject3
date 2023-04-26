#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <ctime>
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


    // Start clock to time map approach
    clock_t startTime = clock();

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
        if (token.empty()) {break;}
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

    double totalSum = 0;
    int matchedDays = 0;
    for (auto i : myMap["At"]) {

        int targetDay = i.first + timeFrame;
        int mid;

        // Look for match in "Down" (Binary search)
        int left = 0;
        int right = myMap["Down"].size() - 1;
        while (left <= right) {
            mid = left + ((right - left) / 2);
            if (myMap["Down"][mid].first == targetDay) {
                totalSum += myMap["Down"][mid].second;
                matchedDays++;
                break;
            } else if (myMap["Down"][mid].first < targetDay) {
                left = mid + 1;
            } else if (myMap["Down"][mid].first > targetDay) {
                right = mid - 1;
            }
        }

        // Look for match in "At" (Binary search)
        left = 0;
        right = myMap["At"].size() - 1;
        while (left <= right) {
            mid = left + ((right - left) / 2);
            if (myMap["At"][mid].first == targetDay) {
                totalSum += myMap["At"][mid].second;
                matchedDays++;
                break;
            } else if (myMap["At"][mid].first < targetDay) {
                left = mid + 1;
            } else if (myMap["At"][mid].first > targetDay) {
                right = mid - 1;
            }
        }

        // Look for match in "Up" (Binary search)
        left = 0;
        right = myMap["Up"].size() - 1;
        while (left <= right) {
            mid = left + ((right - left) / 2);
            if (myMap["Up"][mid].first == targetDay) {
                totalSum += myMap["Up"][mid].second;
                matchedDays++;
                break;
            } else if (myMap["Up"][mid].first < targetDay) {
                left = mid + 1;
            } else if (myMap["Up"][mid].first > targetDay) {
                right = mid - 1;
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
    cout << fixed << showpoint << setprecision(2);
    string changeString = (percentChange >= 0) ? "go up " : "go down ";

    cout << "The stock will " << changeString;
    cout << abs(percentChange) << "% in ";
    cout << timeFrame << " trading days with a ";
    cout << confidence << "% certainty" << endl;

    // End clock and save time elapsed for map process
    clock_t endTime = clock();
    clock_t timeElapsed = endTime - startTime;
    auto mapTime = static_cast<float>(timeElapsed)/CLOCKS_PER_SEC;

    cout << "The map approach finished in " << mapTime << " seconds." << endl;



    // Graph (Adjacency List) setup



    return 0;
}