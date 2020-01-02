/*
Ramiz Dündar
2016400012
Compiling
Working
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

/*
Necesary global variables. m and s is here for
communicating between customers and atms. t is
for assuring only one atm can write at the same time.
*/

pthread_mutex_t m[11];
pthread_mutex_t s[11];
pthread_mutex_t t = PTHREAD_MUTEX_INITIALIZER;

// Memory for transferring data from customers to atms.
vector<string> shared_mem(11);

// Count variable for ensuring atms quit while when transactions finish.
// AKA number of transactions done.
int cnt = 0;

// Number of customers are global because different threads needs it.
int num_customers;

// Global versions of total amout of payments.
int g_cabl = 0;
int g_elec = 0;
int g_gas = 0;
int g_tele = 0;
int g_wtr = 0;

// Stream for writing to a log file.
ofstream fout;

// Customer function
void *customer(void *param);

// ATM function
void *atm(void *param);


int main(int argc, char *argv[])
{
    if(argc != 2) {
        cout << "error: argument count is invalid" << endl;
        exit(EXIT_FAILURE);
    }
        
    string input_path = argv[1];

    // Input stream for reading from input.
    ifstream fin;

    fin.open(input_path);

    if(!fin) {
        cout << "error: unable to open file" << endl;
        exit(EXIT_FAILURE);
    }

    // Initializing mutexes
    for (int i = 1; i < 11; i++) {
        m[i] = PTHREAD_MUTEX_INITIALIZER;
        s[i] = PTHREAD_MUTEX_INITIALIZER;
        // Making s mutex initally unavailable.
        pthread_mutex_lock(&s[i]);

    }

    fin >> num_customers;

    // Reading input to lines vector.
    vector<string> lines(num_customers);

    for (int i = 0; i < num_customers; i++) {
        fin >> lines[i];
        // Adding which customer to lines because it will be needed later.
        lines[i].append(",Customer");
        lines[i].append(to_string(i+1));
    }

    fin.close();

    // Getting output name.
    int dot_ix = input_path.find(".");
    string path_wo_txt = input_path.substr(0, dot_ix);
    string output_path = path_wo_txt + "_log.txt";

    fout.open(output_path);

    // Creating atm and cutomer threads
    pthread_t atms[11];
    pthread_t customers[num_customers];

    int atm_num[11];

    for (int i = 1; i < 11; i++) {
        atm_num[i] = i;
        pthread_create(&atms[i], 0, atm, &atm_num[i]);
    }

    for (int i = 0; i < num_customers; i++) 
        pthread_create(&customers[i], 0, customer, &lines[i]);
    
    for (int i = 1; i < 11; i++) 
        pthread_join(atms[i], NULL);
    
    // Main waiting for transactions to finish. 
    for (int i = 0; i < num_customers; i++)
        pthread_join(customers[i], NULL);
    
    // Writing totals to log file.
    fout << "All payments are completed." << endl;
    fout << "CableTV: " << g_cabl << "TL" << endl;
    fout << "Electricity: " << g_elec << "TL" << endl;
    fout << "Gas: " << g_gas << "TL" << endl;
    fout << "Telecommunication: " << g_tele << "TL" << endl;
    fout << "Water: " << g_wtr << "TL" << endl;
    
    // Checking for deadlocks.
    cout << "reached end" << endl;
    return 0;
}

void *customer(void *param) {

    // Necessary local vars. Names are self explanatory.
    string token;
    int sleep_time;
    int atm_num;
    string line =  *((string *)param);

    stringstream ss(line);

    getline(ss, token, ',');
    sleep_time = stoi(token);
    
    // Sleeping customer.
    usleep(sleep_time * 1000);

    getline(ss, token, ',');
    atm_num = stoi(token);

    // Sending transaction information to atms.
    pthread_mutex_lock(&m[atm_num]);
    shared_mem[atm_num] = line;
    pthread_mutex_unlock(&s[atm_num]);
}

void *atm(void *param) {

    int atm_num = *((int *)param);

    // Local transaction sums for each atm to be summed into global versions later.
    int cabl = 0;
    int elec = 0;
    int gas = 0;
    int tele = 0;
    int wtr = 0;
    
    while(true) {

        // Get transaction from customer.
        pthread_mutex_lock(&s[atm_num]);

        string line = shared_mem[atm_num];

        pthread_mutex_unlock(&m[atm_num]);

        // Finish when transactions finish.
        if (cnt >= num_customers) {

            pthread_mutex_lock(&t);
            g_cabl += cabl;
            g_elec += elec;
            g_gas += gas;
            g_tele += tele;
            g_wtr += wtr;
            

            pthread_mutex_unlock(&t);
            break;
        }

        // Parse transaction to get desirable format.
        string token;
        
        stringstream ss(line);

        getline(ss, token, ',');
        getline(ss, token, ',');

        getline(ss, token, ',');
        string bill_type = token;
        getline(ss, token, ',');
        string amount = token;
        getline(ss, token, ',');
        string customer = token;

        string log = customer + "," + amount + "," + bill_type;

        // Update local transaction summs.
        if (bill_type[0] == 'c') 
            cabl += stoi(amount);
        
        if (bill_type[0] == 'e') 
            elec += stoi(amount);

        if (bill_type[0] == 'g') 
            gas += stoi(amount);

        if (bill_type[0] == 't') 
            tele += stoi(amount);

        if (bill_type[0] == 'w') 
            wtr += stoi(amount);


        // Write to log file and update transaction done.
        pthread_mutex_lock(&t);

        fout << log << endl;
        cnt++;

        pthread_mutex_unlock(&t);

        
        // If all transactions finished unlock all locks so that every atm can break.
        if (cnt >= num_customers) {
            cout << "number of transactions done: " << cnt << endl;
            for (int i = 1; i < 11; i++) {
                pthread_mutex_unlock(&s[i]);
            }
        }
    }
}