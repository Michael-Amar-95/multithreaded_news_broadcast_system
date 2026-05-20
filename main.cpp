//In this code I choose this option as input.
/*
    1
    25
    5

    2
    30
    3

    3
    10
    30

    17
 */

#include <iostream>
#include <semaphore.h>
#include <queue>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;

// struct that represent a string of message
struct message {
    char *types;
    int countType;
    int producerID;
};

// this class represent a Bounded queue
class BoundedQ {
private:
    sem_t full{};
    sem_t empty{};
    pthread_mutex_t m;
    std::queue<message *> buf;

public:

    BoundedQ(int n) { // constructor
        pthread_mutex_init(&m, NULL);
        sem_init(&empty, 0, n);
        sem_init(&full, 0, 0);
    }

    virtual ~BoundedQ() { // destructor
        sem_destroy(&empty);
        sem_destroy(&full);
    }

    void insert(struct message *mes) { // add message* to the queue
        sem_wait(&empty);
        pthread_mutex_lock(&m);
        buf.push(mes);
        pthread_mutex_unlock(&m);
        sem_post(&full);
    }

    message *remove() { // remove message* to the queue
        sem_wait(&full);
        pthread_mutex_lock(&m);
        struct message *x = buf.front();
        buf.pop();
        pthread_mutex_unlock(&m);
        sem_post(&empty);
        return x;
    }
};

// this class represent a ubBounded queue
class UnBoundedQ {
private:
    sem_t full{};
    pthread_mutex_t m;
    std::queue<message *> buf;

public:

    UnBoundedQ() { // constructor
        pthread_mutex_init(&m, NULL);
        sem_init(&full, 0, 0);
    }

    virtual ~UnBoundedQ() { // destructor
        sem_destroy(&full);
    }

    void insert(struct message *mes) { // add message* to the queue
        pthread_mutex_lock(&m);
        buf.push(mes);
        pthread_mutex_unlock(&m);
        sem_post(&full);
    }

    message *remove() { // remove message* to the queue
        sem_wait(&full);
        pthread_mutex_lock(&m);
        struct message *x = buf.front();
        buf.pop();
        pthread_mutex_unlock(&m);
        return x;
    }
};


// Global variables
std::vector<BoundedQ *> b_queue_i; //contain the producer`s queues
UnBoundedQ *u_b_queue_s = new UnBoundedQ(); //sports queue
UnBoundedQ *u_b_queue_n = new UnBoundedQ(); //news queue
UnBoundedQ *u_b_queue_w = new UnBoundedQ(); //weathers queue
BoundedQ *sharedQ = nullptr;
std::vector<std::thread> thr;
std::vector<int> numOfMessage;
mutex printing;

// type of message
char doneMessage[5] = "DONE";
char sports[7] = "SPORTS";
char news[5] = "NEWS";
char weather[8] = "WEATHER";

/**
 * This function is the co editor.
 * It sort the messages by queue.
 * @param identifier - indicate the type of co-editor
 * @return
 */void coEdit(int identifier) {

    struct message *mes = nullptr;

    // identify the co-editor
    if(identifier == 0){ //sport editor
        while (true) {
            mes = u_b_queue_s->remove();
            sharedQ->insert(mes);
            // check if I got DONE message
            if (!strcmp(mes->types, "DONE")) {
                break;
            }
        }
    } else if(identifier == 1){ //news editor
        while (true) {
            mes = u_b_queue_n->remove();
            sharedQ->insert(mes);
            // check if I got DONE message
            if (!strcmp(mes->types, "DONE")) {
                break;
            }
        }
    } else if(identifier == 2){ //weather editor
        while (true) {
            mes = u_b_queue_w->remove();
            sharedQ->insert(mes);
            // check if I got DONE message
            if (!strcmp(mes->types, "DONE")) {
                break;
            }
        }
    }
}

/**
 * This function print to the screen.
 * @return
 */
void screenManager() {
    int countDone = 0;
    struct message *m = nullptr;
    while (countDone < 3) { // iterate the queue
        m = sharedQ->remove();
        if (!strcmp(m->types, "DONE")) { // //we got DONE message
            countDone++;
        } else { // normal message
            // check permission to print to the screen
            printing.lock();
            std::cout << "Producer " << m->producerID << " " << m->types << " " << m->countType << "\n";
            printing.unlock();
        }
    }
    std::cout << "DONE\n";
    delete m; //delete mes;

    // delete the last queue
    delete sharedQ;
}

/**
 * This function is the dispatcher.
 * Sort the message into there queue type.
 */
void dispatcher() {
    while (true) {
        bool flag = true; //flag
        for (auto &b: b_queue_i) { // loop over all the producer
            struct message *mes = b->remove();
            if (!strcmp(mes->types, "DONE")) { // if done enter done
                b->insert(mes);
            } else {
                flag = false; // if I got a normal message done
                // identify the correct type
                if (!strcmp(mes->types, sports)) {
                    // check permission to print to the screen
                    printing.lock();
                    u_b_queue_s->insert(mes);
                    cout << "SPORTS  Inserted to the \"S dispatcher queue\"\n";
                    printing.unlock();
                }
                if (!strcmp(mes->types, news)) {
                    // check permission to print to the screen
                    printing.lock();
                    u_b_queue_n->insert(mes);
                    cout << "NEWS  Inserted to the \"N dispatcher queue\"\n";
                    printing.unlock();
                }
                if (!strcmp(mes->types, weather)) {
                    // check permission to print to the screen
                    printing.lock();
                    u_b_queue_w->insert(mes);
                    cout << "WEATHER  Inserted to the \"W dispatcher queue\"\n";
                    printing.unlock();
                }
            }
        }
        // check if in all queues I got DONE message
        if(flag){
            break;
        }
    }
    // put Done message in the end of the three queue
    struct message* mes1 = new message;
    mes1->types = doneMessage;
    mes1->countType = 0;
    mes1->producerID = 0;
    struct message* mes2 = new message;
    mes2->types = doneMessage;
    mes2->countType = 0;
    mes2->producerID = 0;
    struct message* mes3 = new message;
    mes3->types = doneMessage;
    mes3->countType = 0;
    mes3->producerID = 0;
    // insert done message in all queues
    u_b_queue_s->insert(mes1);
    u_b_queue_n->insert(mes2);
    u_b_queue_w->insert(mes3);

    // delete all the queues
    for (BoundedQ *q: b_queue_i) {
        delete q;
    }
}

/**
 * This function produce the messages
 * @param i the num of the producer
 */
void producer(int numOfQueue, int pro_id) {

    int countSports = 0;
    int countWeather = 0;
    int countNews = 0;

    struct message *mes = nullptr;

    for (int i = 0; i < numOfMessage[numOfQueue]; i++) {

        int randomIndex = rand() % 3; // rand string
        mes = new message;

        if (randomIndex == 0) { // Sport case
            mes->types = sports;
            mes->producerID = pro_id;
            mes->countType = countSports;
            countSports++;
        } else if (randomIndex == 1) { // News case
            mes->types = news;
            mes->producerID = pro_id;
            mes->countType = countNews;
            countNews++;
        } else if (randomIndex == 2) { // Weather case
            mes->types = weather;
            mes->producerID = pro_id;
            mes->countType = countWeather;
            countWeather++;
        }
        b_queue_i[numOfQueue]->insert(mes); // add the new message to the vector
    }

    // creat Done message at the end
    mes = new message;
    mes->types = doneMessage;
    mes->producerID = pro_id;
    mes->countType = -1;
    b_queue_i[numOfQueue]->insert(mes);
}

/**
 * Read the configuration file.
 * @param path the path of the file
 */
void readConf(char *path) {
    ifstream file(path);
    //check if I can read the file
    if (file.is_open()) {
        string s;
        //while there are lines to read
        while (getline(file, s)) {
            //convert the id of the producer\size of last queue to int
            int id = stoi(s);
            string info;
            if (!getline(file, info)) {
                // creat the last queue for the screen manager
                sharedQ = new BoundedQ(id);
                break;
            }

            //save the num of message that each producer creat
            numOfMessage.push_back(stoi(info));

            getline(file, info);

            //creat all the queue
            b_queue_i.push_back(new BoundedQ(stoi(info)));

            getline(file, info); // empty line
        }
    }
    file.close();
}

int main(int argc, char *argv[]) {

    srand(time(nullptr));

    srand(time(nullptr));

    // any run will give me different output
    srand(time(nullptr));

    // read the conf file
    readConf(argv[1]);
    int const numOfProducer = b_queue_i.size(); // num of producer

    //initialize producer threads
    for (int i = 0; i < numOfProducer; ++i) {
        int producerID = i + 1;
        thr.emplace_back(producer, i, producerID);
    }

    //creat dispatcher thread
    thr.emplace_back(dispatcher);

    //creat co-editors threads
    for (int i = 0; i < 3; i++) {
        thr.emplace_back(coEdit, i);
    }

    //creat manager thread
    thr.emplace_back(screenManager);

    //join all the threads
    for (auto &thread: thr) {
        thread.join();
    }
    // delete queues
    delete u_b_queue_s;
    delete u_b_queue_n;
    delete u_b_queue_w;

    return 0;
}
