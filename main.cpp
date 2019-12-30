#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2) {
        cout << "error: argument count is invalid" << endl;
        exit(EXIT_FAILURE);
    }
        
    string input_path = argv[1];

    ifstream fin;

    fin.open(input_path);

    if(!fin) {
        cout << "error: unable to open file" << endl;
        exit(EXIT_FAILURE);
    }

    int num_customers;

    fin >> num_customers;

    vector<int> sleep_times(num_customers);
    vector<int> atm_instances(num_customers);
    vector<string> bill_types(num_customers);
    vector<int> amounts(num_customers);
    string line;
    string token;

    for (int i = 0; i < num_customers; i++) {
        fin >> line;
        stringstream ss(line);

        getline(ss, token, ',');
        sleep_times[i] = stoi(token);
        
        getline(ss, token, ',');
        atm_instances[i] = stoi(token);
        
        getline(ss, token, ',');
        bill_types[i] = token;
        
        getline(ss, token, ',');
        amounts[i] = stoi(token);

    }

    for (int i = 0; i < num_customers; i++) {
        cout << sleep_times[i] << endl;
        cout << atm_instances[i] << endl;
        cout << bill_types[i] << endl;
        cout << amounts[i] << endl;
    }
    

    cout << "reached end" << endl;
    return 0;
}