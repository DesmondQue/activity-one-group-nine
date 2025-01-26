#include <iostream>
#include <fstream>
#include <string>
#include "udf.h"
#include <thread>

using namespace std;

void CLS() {
#ifdef _WIN32 // windows
    system("cls");
#else // unix
    (void)system("clear");
#endif
}

void elevatorHeader();
void elevatorPrompt();
void generateRandomRequest(int &seed, int currentFloor);
int pseudoRandom(int seed) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
    }
bool checkForEarthquake(int &seed) {
    seed = pseudoRandom(seed);
    return (seed % 3 == 0); // 33 chance of getting an earthquake event
}


class Elevator {
private:
    int currentFloor;
    string direction; // "up", "down"
    priorityQueueNode* upRequests; // priority queue for up requests
    priorityQueueNode* downRequests; // priority queue for down requests
    stackNode<int>* recentlyVisited; // stack for visited floors
    queueNode<int>* floorRequestsFront; // queue for floor requests (front)
    queueNode<int>* floorRequestsRear; // queue for floor requests (rear)
    ofstream logFile;
    bool isProcessing; // flag for processing state
    int lastRandomRequest; // Track the last random request

public:
    Elevator() : currentFloor(1), direction("up"), upRequests(NULL), downRequests(NULL), recentlyVisited(NULL), floorRequestsFront(NULL), floorRequestsRear(NULL), isProcessing(false),  lastRandomRequest(-1) {
        logFile.open("elevator_log.txt", ios::app);
        if (!logFile.is_open()) {
            cerr << "error opening log file!\n";
        }
    }

    ~Elevator() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    int  getLastRandomRequest() const {
    return lastRandomRequest;
    }

    void setLastRandomRequest(int floor) {
    lastRandomRequest = floor;
    }

    void acceptRequest(int floor) {
        int originatingFloor = currentFloor;
        if (floor < 1 || floor > 9) {
            cout << "invalid floor request: " << floor << endl;
            log("invalid floor request: " + to_string(floor));
            return;
        }

        if (floor == currentFloor) {
            cout << "                elevator is already on floor " << floor << ". request ignored." << endl;
            log("request ignored: elevator already on floor " + to_string(floor));
            return;
        }

        bool duplicate = checkIfDuplicates(floorRequestsFront, floor);
        if (duplicate) {
           
            cout << "duplicate floor request: " << floor << ". request ignored." << endl;
            log("duplicate floor request: " + to_string(floor));
            return;
        }

        if (isEmpty(floorRequestsFront) && isEmpty(&upRequests) && isEmpty(&downRequests)) {
            direction = (floor > currentFloor) ? "up" : "down";
        }

        enqueue(floorRequestsFront, floorRequestsRear, floor);
        log("request added for floor: " + to_string(floor) + " from floor: " + to_string(originatingFloor));
        cout << "Request added for floor: " << floor << " from floor: " << originatingFloor << endl;

        // ewan lagay sa prio queue ata
        if (floor > currentFloor) {
            push(&upRequests, floor, floor);
        } else {
            push(&downRequests, floor, -floor);
        }
    }

    void processRequests() {
        isProcessing = true;
        int seed = time(0);
        while (!isEmpty(floorRequestsFront) || !isEmpty(&upRequests) || !isEmpty(&downRequests)) {
        // Check for earthquake event
        if (checkForEarthquake(seed)) {
            cout << "Earthquake detected! Elevator operation stopped temporarily.\n";
            log("Earthquake detected! Elevator operation stopped temporarily.");
            isProcessing = false;
            goto purgatoryEvent; // Exit the function to stop processing requests
        }
            while (!isEmpty(floorRequestsFront)) {
                int nextFloor = dequeue(floorRequestsFront, floorRequestsRear);
                if (nextFloor > currentFloor) {
                    push(&upRequests, nextFloor, nextFloor);
                } else if (nextFloor < currentFloor) {
                    push(&downRequests, nextFloor, -nextFloor);
                }
            }

            if (direction == "up" && !isEmpty(&upRequests)) {
                processUpRequests();
            } else if (direction == "down" && !isEmpty(&downRequests)) {
                processDownRequests();
            }

            if (direction == "up" && isEmpty(&upRequests) && !isEmpty(&downRequests)) {
                direction = "down";
            } else if (direction == "down" && isEmpty(&downRequests) && !isEmpty(&upRequests)) {
                direction = "up";
            }
        }
        isProcessing = false;
        return;

        purgatoryEvent:
        string userinput;
        cout << "You are in the purgatory event..." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        cout << "Do you wish to wake up? (Y/N)" << endl;
        cin >> userinput;
        if (userinput == "N" || userinput == "n"){
            cout << "It was all a dream..." << endl;
            this_thread::sleep_for(chrono::seconds(3));
            cout << "You have escaped the elevator." << endl;
            this_thread::sleep_for(chrono::seconds(2));
            cout << "Good Ending Achieved :D" << endl;
            cout << "Group 9 || Created by: Que, Desmond & Orbegoso, Aloysious";
            this_thread::sleep_for(chrono::seconds(4));
            exit(1);
        } else {
            cout << "You hear a violent grinding of metal and a sudden thud as you black out." << endl;
            this_thread::sleep_for(chrono::seconds(2));
            cout << "You hear the painful screams of those around you." << endl;
            this_thread::sleep_for(chrono::seconds(2));
            cout << "You see a faint white flash slowly fading..." << endl;
            this_thread::sleep_for(chrono::seconds(2));
            cout << "Moments pass and then suddenly" << endl;
            this_thread::sleep_for(chrono::seconds(2));
            cout << "You are infront";
            this_thread::sleep_for(chrono::seconds(2));
            cout << "... of an elevator." << endl;
            this_thread::sleep_for(chrono::seconds(2));
            return;
        }

        
    }

    void processUpRequests() {
        while (!isEmpty(&upRequests)) {
            int nextFloor = peek(&upRequests);
            pop(&upRequests);
            moveToFloor(nextFloor);
        }
    }

    void processDownRequests() {
        while (!isEmpty(&downRequests)) {
            int nextFloor = peek(&downRequests);
            pop(&downRequests);
            moveToFloor(nextFloor);
        }
    }



    void moveToFloor(int floor) {
        direction = (floor > currentFloor) ? "up" : "down";
        while (currentFloor != floor) {
            currentFloor += (direction == "up") ? 1 : -1;
            displayState();
            this_thread::sleep_for(chrono::seconds(1)); // Sleep for 1 second before moving to the next floor
        }

        cout << "stopping at floor: " << currentFloor << endl;
        log("elevator stopped at floor: " + to_string(currentFloor));
        push(recentlyVisited, currentFloor);
        this_thread::sleep_for(chrono::seconds(3));
        
        //Ask for user input
            string input;
            int newFloor;

            while (true) { 
                elevatorHeader(); 
                elevatorPrompt();
                cin >> input;
                
                if (input == "X" || input == "x") {
                    exit(1);
                } else if (input == "O" || input == "o") {
                    processRequests();
                } else if (isdigit(input[0]) && input.length() == 1) {
                    newFloor = input[0] - '0';
                    if (newFloor >= 1 && newFloor <= 9) {
                        acceptRequest(newFloor);
                    } else {
                        cout << "Invalid floor request. Please enter a number between 1 and 9.\n";
                    }
                } else {
                    cout << "Invalid input. Please try again.\n";
                }
            }

    // Generate a random request after stopping at the floor with 33% probability
        int seed = time(0);
        if (pseudoRandom(seed) % 5 == 0) { // 20% chance
            generateRandomRequest(seed, currentFloor);
    }
    }

    void displayState() {
        cout << "current floor: " << currentFloor << ", direction: " << direction << endl;
        log("state - floor: " + to_string(currentFloor) + ", direction: " + direction);
    }

    void log(const string &message) {
        if (logFile.is_open()) {
            logFile << message << endl;
        }
    }

    int getCurrentFloor() const {
        return currentFloor;
    }

    string getDirection() const {
        return direction;
    }
};



 Elevator elevator;

void generateRandomRequest(int &seed, int currentFloor) {
    int randomFloor, originatingFloor;
    do {
        seed = pseudoRandom(seed);
        randomFloor = (seed % 9) + 1; // Generate a floor number between 1 and 9
    } while (randomFloor == currentFloor || randomFloor == elevator.getLastRandomRequest()); // Ensure it's different from the current floor and from last random request

    do {
        seed = pseudoRandom(seed);
        originatingFloor = (seed % 9) + 1; // Generate an originating floor number between 1 and 9
    } while (originatingFloor == randomFloor); // Ensure it's different from the requested floor

    elevator.acceptRequest(randomFloor);
    cout << "Random floor request generated for floor: " << randomFloor << " from floor: " << originatingFloor << endl;

    elevator.setLastRandomRequest(randomFloor);  // Update the last random request
}
    
 

void elevatorHeader(){
    cout << endl << "                                 |HJT ELEVATOR|" << endl;
    cout << "=================================================================================\n";
    cout << "                                     |" << elevator.getCurrentFloor() << " " << elevator.getDirection() << "|\n";
    cout << "=================================================================================\n";
}

void elevatorPrompt(){
    cout << "enter floor numbers (1-9) to request, [o] to process requests, or [x] to exit.\n";
    cout << "enter your input: ";
}



int main() {

    string input;
    int floor;

    while (true) { 
        elevatorHeader(); 
        elevatorPrompt();
        cin >> input;
        
        if (input == "X" || input == "x") {
            exit(1);
        } else if (input == "O" || input == "o") {
            elevator.processRequests();
        } else if (isdigit(input[0]) && input.length() == 1) {
            floor = input[0] - '0';
            if (floor >= 1 && floor <= 9) {
                elevator.acceptRequest(floor);
            } else {
                cout << "Invalid floor request. Please enter a number between 1 and 9.\n";
            }
        } else {
            cout << "Invalid input. Please try again.\n";
        }
    }

   
    

    return 0;
}