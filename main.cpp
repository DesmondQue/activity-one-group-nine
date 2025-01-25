#include <iostream>
#include <fstream>
#include <string>
#include "udf.h"
using namespace std;

void CLS() {
#ifdef _WIN32 // windows
    system("cls");
#else // unix
    (void)system("clear");
#endif
}

struct Request {
    int floor;
};

class Elevator {
private:
    int currentFloor;
    string direction; // "up", "down", or "neutral"
    priorityQueueNode* upRequests; // priority queue for up requests
    priorityQueueNode* downRequests; // priority queue for down requests
    stackNode<int>* recentlyVisited; // stack for visited floors
    queueNode<int>* floorRequestsFront; // queue for floor requests (front)
    queueNode<int>* floorRequestsRear; // queue for floor requests (rear)
    ofstream logFile;
    bool isProcessing; // flag for processing state

public:
    Elevator() : currentFloor(1), direction("neutral"), upRequests(NULL), downRequests(NULL), recentlyVisited(NULL), floorRequestsFront(NULL), floorRequestsRear(NULL), isProcessing(false) {
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

    void acceptRequest(int floor) {
        if (floor < 1 || floor > 9) {
            CLS();
            cout << "invalid floor request: " << floor << endl;
            log("invalid floor request: " + to_string(floor));
            return;
        }

        if (floor == currentFloor) {
            CLS();
            cout << "                elevator is already on floor " << floor << ". request ignored." << endl;
            log("request ignored: elevator already on floor " + to_string(floor));
            return;
        }

        bool duplicate = checkIfDuplicates(floorRequestsFront, floor);
        if (duplicate) {
            CLS();
            cout << "duplicate floor request: " << floor << ". request ignored." << endl;
            log("duplicate floor request: " + to_string(floor));
            return;
        }

        enqueue(floorRequestsFront, floorRequestsRear, floor);
        log("request added for floor: " + to_string(floor));
    }

    void processRequests() {
        isProcessing = true;
        while (!isEmpty(floorRequestsFront) || !isEmpty(&upRequests) || !isEmpty(&downRequests)) {
            // process requests from the queue
            while (!isEmpty(floorRequestsFront)) {
                int nextFloor = dequeue(floorRequestsFront, floorRequestsRear);
                if (nextFloor > currentFloor) {
                    push(&upRequests, nextFloor, nextFloor);
                } else if (nextFloor < currentFloor) {
                    push(&downRequests, nextFloor, -nextFloor);
                }
            }

            // process requests based on direction
            if (direction == "up" && !isEmpty(&upRequests)) {
                processUpRequests();
            } else if (direction == "down" && !isEmpty(&downRequests)) {
                processDownRequests();
            }

            // change direction if needed
            if (direction == "up" && isEmpty(&upRequests) && !isEmpty(&downRequests)) {
                direction = "down";
            } else if (direction == "down" && isEmpty(&downRequests) && !isEmpty(&upRequests)) {
                direction = "up";
            } else if (isEmpty(&upRequests) && isEmpty(&downRequests)) {
                direction = "neutral";
            }
        }
        isProcessing = false;
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
        }

        cout << "stopping at floor: " << currentFloor << endl;
        log("elevator stopped at floor: " + to_string(currentFloor));
        push(recentlyVisited, currentFloor);
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

void elevatorHeader(){
    cout << "=================================================================================\n";
    cout << "                                     |" << elevator.getCurrentFloor() << " " << elevator.getDirection() << "|\n";
    cout << "=================================================================================\n";
}

void elevatorPrompt(){
    cout << "enter floor numbers (1-9) to request, [o] to process requests, or [x] to exit.\n";
    cout << "enter your input: ";
}

int main() {
    char input;
    int floor;

    while (true) {
        CLS();
        elevatorHeader();
        elevatorPrompt();
        cin >> input;

        if (input == 'X' || input == 'x') {
            break;
        } else if (input == 'O' || input == 'o') {
            elevator.processRequests();
        } else if (isdigit(input)) {
            floor = input - '0';
            elevator.acceptRequest(floor);
        } else {
            cout << "invalid input. please try again.\n";
        }
    }
    return 0;
}