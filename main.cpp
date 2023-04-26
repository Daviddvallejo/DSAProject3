#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
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
    double percentChange = ((avgPrice - stockVal) / stockVal) * 100;

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
    /*
     * Read in data values and when current price is reached, store the value that is timeframe away from it in an
     * adjacency list, values that are timeframe away will be grouped into number of percent change ranges of (+/-)0.25%
     * they are away from current stock price ($303 is 2 ranges away from the current price of $300)
     * and weights will be number of times that percent change was reached.
     */

    // Create graph to store data
    unordered_map<int, vector<pair<int, int>>> myGraph;

    // Create queue to store how many days away we are from the next day that is timeframe away from a day that had
    // current stock price
    queue<int> q;

    // Open file
    inFS.open(file);

    day = 0;
    val = 0;
    token = "";
    percentChange = 0;
    int range;

    // Read lines in file, separate lines into usable values, and add to graph
    while (inFS) {
        inFS.clear();

        getline(inFS, token, ',');
        if (token.empty()) {break;}

        day = stoi(token);
        getline(inFS, token);
        val = stoi(token);

        // Add this value to the graph as it is timeframe away from a day we were at the input stock price
        if (!q.empty() && q.front() == 0) {
            q.pop();
            // Calculate percent change and then how many ranges away it is from input stock price
            percentChange = (static_cast<double>(val - stockVal) / stockVal) * 100;
            range = ceil(((percentChange / 0.25) - 1) / 2);
            if (myGraph[stockVal].empty()) {
                myGraph[stockVal].emplace_back(range, 1);
            } else {
                for (auto it = myGraph[stockVal].begin(); it != myGraph[stockVal].end(); it++) {
                    if (range == it->first) {
                        it->second++;
                        break;
                    } else if (range < it->first && (it == myGraph[stockVal].begin() || range > (it-1)->first)) {
                        myGraph[stockVal].emplace(it, range, 1);
                    } else if (it+1 == myGraph[stockVal].end()) {
                        myGraph[stockVal].emplace_back(range, 1);
                    }
                }
            }

        }



    }




    return 0;
}