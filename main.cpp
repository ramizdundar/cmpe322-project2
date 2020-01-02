#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

pthread_mutex_t m[11];
pthread_mutex_t s[11];
pthread_mutex_t t = PTHREAD_MUTEX_INITIALIZER;
vector<string> shared_mem(11);
int cnt = 0;
int num_customers;

ofstream fout;


void *customer(void *param);


void *atm(void *param);


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

    for (int i = 1; i < 11; i++) {
        m[i] = PTHREAD_MUTEX_INITIALIZER;
        s[i] = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&s[i]);

    }

    

    fin >> num_customers;

    vector<string> lines(num_customers);

    for (int i = 0; i < num_customers; i++) {
        fin >> lines[i];
        lines[i].append(",Customer");
        lines[i].append(to_string(i+1));
    }

    fin.close();

    int dot_ix = input_path.find(".");
    string path_wo_txt = input_path.substr(0, dot_ix);
    string output_path = path_wo_txt + "_log.txt";

    fout.open(output_path);

    pthread_t atms[11];
    pthread_t customers[num_customers];

    int atm_num[11];

    for (int i = 1; i < 11; i++) {
        atm_num[i] = i;
        pthread_create(&atms[i], 0, atm, &atm_num[i]);
    
    }

    for (int i = 0; i < num_customers; i++) {
        pthread_create(&customers[i], 0, customer, &lines[i]);
    }

    for (int i = 1; i < 11; i++) {
        pthread_join(atms[i], NULL);
    }

    for (int i = 0; i < num_customers; i++) {
        pthread_join(customers[i], NULL);
    }
    

    for (int i = 0; i < num_customers; i++) {
        //cout << lines[i] << endl;
    }
    

    cout << "reached end" << endl;
    return 0;
}

void *customer(void *param) {
    
    string token;
    int sleep_time;
    int atm_num;
    string line =  *((string *)param);

    stringstream ss(line);

    getline(ss, token, ',');
    sleep_time = stoi(token);
    
    usleep(sleep_time * 1000);

    getline(ss, token, ',');
    atm_num = stoi(token);

    pthread_mutex_lock(&m[atm_num]);
    shared_mem[atm_num] = line;
    pthread_mutex_unlock(&s[atm_num]);

    


    
    
    
    

    pthread_mutex_lock(&t);

    cout << "===============" << endl;
    cout << line << endl;
    cout << "===============" << endl;

    pthread_mutex_unlock(&t);
    

}

void *atm(void *param) {

    int atm_num = *((int *)param);
    string str = "atm:::::  " + to_string(atm_num);
    cout << str << endl;
    
    while(true) {
        pthread_mutex_lock(&s[atm_num]);

        string line = shared_mem[atm_num];

        pthread_mutex_unlock(&m[atm_num]);

        if (cnt >= num_customers) {
            cout << "jkbRWUGBABGanglnlgnwg;AG" << endl;
            break;
        }

        string str = "atm  " + to_string(atm_num);
        cout << str << endl;

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


        pthread_mutex_lock(&t);

        fout << log << endl;
        cnt++;
        cout << cnt << endl;

        pthread_mutex_unlock(&t);

        

        if (cnt >= num_customers) {
            cout << cnt << endl;
            for (int i = 1; i < 11; i++) {
                pthread_mutex_unlock(&s[i]);
            }
        }


    }
}