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

    // Open input filestream for map
    inFS.open(file);

    int day;
    double val;
    string token;
    while (inFS) {
        inFS.clear();

        // Read line and separate values into usable variables
        getline(inFS, token, ',');
        if (token.empty()) {break;}
        day = stoi(token);
        getline(inFS, token);
        val = stoi(token);

        // Normalize values into intervals of 0.5%, going out from the input stock value
        double percentChange = ((val - stockVal) / stockVal) * 100;
        int range = ceil(((percentChange / 0.25) - 1) / 2);


        // Insert values into map
        if (range < 0) {
            myMap["Down"].emplace_back(day,range);
        } else if (range > 0) {
            myMap["Up"].emplace_back(day,range);
        } else {
            myMap["At"].emplace_back(day, range);
        }

    }

    // Close input filestream for map
    inFS.close();

    // Loop through days in "At" and find day either in "Down", "At", or "Up" that is exactly timeframe away from day in "At"
    // If/when a match is found, update counter in price map
    // Keep track of num times price went up, down, or stayed the same and also number of times a day timeframe away was looked for.


    unordered_map<int, int> priceMap;
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
                priceMap[myMap["Down"][mid].second]++;
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
                priceMap[myMap["At"][mid].second]++;
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
                priceMap[myMap["Up"][mid].second]++;
                matchedDays++;
                break;
            } else if (myMap["Up"][mid].first < targetDay) {
                left = mid + 1;
            } else if (myMap["Up"][mid].first > targetDay) {
                right = mid - 1;
            }
        }
    }

    // Calculate weighted average for price of stock after timeframe for every instance the stock was at the current price and
    // the percent change
    double avgRange = 0;
    for (auto x : priceMap) {
        avgRange += static_cast<double>(x.first * x.second) / matchedDays;
    }

    double percentChange = avgRange / 2;

    // Formula for percent confidence
    double maxWeight = 0;
    double maxRange = 0;
    int numMaxRange = 0;
    for (auto x : priceMap) {maxWeight = (maxWeight < x.second) ? x.second : maxWeight;}
    for (auto y : priceMap) {
        if (y.second == maxWeight) {
            maxRange += y.first;
            numMaxRange++;
        }
    }
    double avgTempRange = maxRange / numMaxRange;
    double tempRange = avgTempRange / 2;

    double confidence = 1 - ((abs(percentChange) - abs(tempRange)) / abs(tempRange));

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


    // Start timer for graph setup
    startTime = clock();
    // Graph (Adjacency List) setup
    /*
     * Read in data values and when current price is reached, store the value that is timeframe away from it in an
     * adjacency list, values that are timeframe away will be grouped into number of percent change ranges of (+/-)0.25%
     * they are away from current stock price ($303 is 2 ranges away from the current price of $300)
     * and weights will be number of times that percent change was reached.
     */

    // Create graph to store data
    unordered_map<int, vector<pair<int, int>>> myGraph;

    // Create queue to which days are timeframe away from a day that we saw the input stock price
    queue<int> q;

    // Open input filestream for graph
    inFS.open(file);

    day = 0;
    val = 0;
    token = "";
    percentChange = 0;
    int range, sumWeights = 0;
    double avgPercentChange = 0;

    // Read lines in file, separate lines into usable values, and add to graph
    while (inFS) {
        inFS.clear();

        getline(inFS, token, ',');
        if (token.empty()) {break;}

        day = stoi(token);
        getline(inFS, token);
        val = stoi(token);

        // Add this value to the graph as it is timeframe away from a day we were at the input stock price
        if (!q.empty() && q.front() == day) {
            sumWeights++;
            q.pop();
            // Calculate percent change and then how many ranges away it is from input stock price
            percentChange = (static_cast<double>(val - stockVal) / stockVal) * 100;
            range = ceil(((percentChange / 0.25) - 1) / 2);
            // Update adjacency list by either adding 1 to the weight of an existing range or adding a new range
            if (myGraph[stockVal].empty()) {
                myGraph[stockVal].emplace_back(range, 1);
            } else {
                // Find range if it exists and update its weight
                for (int i = 0; i < myGraph[stockVal].size(); i++) {
                    if (myGraph[stockVal][i].first == range) {
                        myGraph[stockVal][i].second++;
                        int j = i;
                        // Sort ranges by weights
                        while (j > 0 && myGraph[stockVal][j].second > myGraph[stockVal][j-1].second) {
                            pair<int, int> temp = myGraph[stockVal][j-1];
                            myGraph[stockVal][j-1] = myGraph[stockVal][j];
                            myGraph[stockVal][j] = temp;
                        }
                        // If first two weights are equal, prioritize ranges closer to zero, leave in place if ranges are equally away
                        if (myGraph[stockVal][0].second == myGraph[stockVal][1].second && abs(myGraph[stockVal][1].first) < abs(myGraph[stockVal][0].first)) {
                            pair<int, int> temp = myGraph[stockVal][0];
                            myGraph[stockVal][0] = myGraph[stockVal][1];
                            myGraph[stockVal][1] = temp;
                        }
                        break;
                        // If range is not found, add at the end
                    } else if (i+1 == myGraph[stockVal].size()) {
                        myGraph[stockVal].emplace_back(range, 0);
                    }
                }
            }
        }

        // If read in value is equal to input value, add projected day (that is timeframe away) to q
        if (val == stockVal) {q.emplace(day + timeFrame);}

    }

    // Close input filestream for graph
    inFS.close();

    // Calculate price movement and confidence
    for(auto& i : myGraph[stockVal]){
        avgPercentChange += i.first * i.second;
    }

    // Display price movement, confidence, and elapsed time for graph setup
    avgPercentChange = avgPercentChange / sumWeights;
    percentChange = avgPercentChange / 2.0;
    confidence = 0;
    changeString = (percentChange >= 0) ? "go up " : "go down ";

    maxWeight = 0;
    maxRange = 0;
    numMaxRange = 0;
    for (auto x : myGraph[stockVal]) {maxWeight = (maxWeight < x.second) ? x.second : maxWeight;}
    for (auto y : myGraph[stockVal]) {
        if (y.second == maxWeight) {
            maxRange += y.first;
            numMaxRange++;
        }
    }
    avgTempRange = maxRange / numMaxRange;
    tempRange = avgTempRange / 2;

    confidence = 1 - ((abs(percentChange) - abs(tempRange)) / abs(tempRange));

    cout << "The stock will " << changeString;
    cout << abs(percentChange) << "% in ";
    cout << timeFrame << " trading days with a ";
    cout << confidence << "% certainty" << endl;


    // End clock and save time elapsed for graph process
    endTime = clock();
    timeElapsed = endTime - startTime;
    auto graphTime = static_cast<float>(timeElapsed)/CLOCKS_PER_SEC;

    cout << "The graph approach finished in " << graphTime << " seconds." << endl;


    // Compare map runtime to graph runtime and display results with respect to the graph time
    float runTimePercentChange = (graphTime - mapTime) / graphTime;

    if (runTimePercentChange < 0.01 && runTimePercentChange > -0.01){

        cout << "Using either a map or graph resulted in approximately the same runtime." << endl;

    } else {
        if (runTimePercentChange >= 0.01) {
            changeString = "faster.";
        } else if (runTimePercentChange <= -0.01) {
            changeString = "slower.";
        }
        cout << "Using a map instead of a graph allowed the program to run " << abs(runTimePercentChange) << "% ";
        cout << changeString << endl;
    }

    return 0;
}