#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
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
     *      1) Days when price was below current price
     *      2) Days when price was at current price (tolerance of 0.5%)
     *      3) Days when price was above current price
     *
     */
    unordered_map<int, vector<int>> myMap;

    // Create input stream object, read in values and add to map accordingly
    ifstream inFS;
    inFS.open(file);

    int day, val;
    string token;
    while (inFS) {
        inFS.clear();

        getline(inFS, token, ',');
        day = stoi(token);
        getline(inFS, token);
        val = stoi(token);
    }








    // Graph (Adjacency List) setup



    return 0;
}