#ifndef UDF_H
#define UDF_H

#include <iostream>
using namespace std;

// priority queue
struct priorityQueueNode {
    int data;
    int priority;
    priorityQueueNode* next;
};

// create a priorityQueueNode
priorityQueueNode* newPriorityQueueNode(int d, int p) {
    priorityQueueNode* temp = new priorityQueueNode();
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;
    return temp;
}

// remove the element with the highest priority
void pop(priorityQueueNode** head) {
    if (*head == NULL) return;
    priorityQueueNode* temp = *head;
    *head = (*head)->next;
    delete temp;
}

//push according to priority
void push(priorityQueueNode** head, int d, int p) {
    priorityQueueNode* start = *head;
    priorityQueueNode* temp = newPriorityQueueNode(d, p);

    if (*head == NULL || (*head)->priority > p) {
        temp->next = *head;
        *head = temp;
    } else {
        while (start->next != NULL && start->next->priority <= p) {
            start = start->next;
        }
        temp->next = start->next;
        start->next = temp;
    }
}

// check if the list is empty
bool isEmpty(priorityQueueNode** head) {
    return (*head) == NULL;
}

// gets the element with the highest priority
int peek(priorityQueueNode** head) {
    if (*head == NULL) return -1;
    return (*head)->data;
}



//------------------------------------------------------------------



// queue node
template <typename T>
struct queueNode {
    T data;
    queueNode* next;
};

// enqueue an element to the queue
template <typename T>
void enqueue(queueNode<T>*& front, queueNode<T>*& rear, T value) {
    queueNode<T>* newqueueNode = new queueNode<T>();
    newqueueNode->data = value;
    newqueueNode->next = NULL;

    if (rear == NULL) {
        front = rear = newqueueNode;
    } else {
        rear->next = newqueueNode;
        rear = newqueueNode;
    }
}

//dequeue an element from the queue
template <typename T>
T dequeue(queueNode<T>*& front, queueNode<T>*& rear) {
    if (front == NULL) {
        cout << "Queue underflow" << endl;
        return T();
    }
    queueNode<T>* temp = front;
    T dequeuedValue = temp->data;
    front = front->next;
    if (front == NULL) {
        rear = NULL;
    }
    delete temp;
    return dequeuedValue;
}

// check if queue is empty
template <typename T>
bool isEmpty(queueNode<T>* front) {
    return front == NULL;
}

// size of queue
template <typename T>
int size(queueNode<T>* front) {
    int count = 0;
    queueNode<T>* temp = front;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

//peek at the front element of queue
template <typename T>
T peek(queueNode<T>* front) {
    if (front == NULL) {
        cout << "Queue is empty" << endl;
        return T();
    }
    return front->data;
}

template <typename T>
bool checkIfDuplicates(queueNode<T>* front, int floor) {
    while (front != NULL) {
        if (front->data == floor) {
            return true;
        }
        front = front->next;
    }
    return false; 
}

//------------------------------------------------------------------


// stack node
template <typename T>
struct stackNode {
    T data;
    stackNode* next;
};

// push stack
template <typename T>
void push(stackNode<T>*& top, T value) {
    stackNode<T>* newstackNode = new stackNode<T>();
    newstackNode->data = value;
    newstackNode->next = top;
    top = newstackNode;
}

// pop stack
template <typename T>
T pop(stackNode<T>*& top) {
    if (top == NULL) {
        cout << "Stack underflow" << endl;
        return T(); // Return default value of T
    }
    stackNode<T>* temp = top;
    T poppedValue = temp->data;
    top = top->next;
    delete temp;
    return poppedValue;
}

// check if stack is empty
template <typename T>
bool isEmpty(stackNode<T>* top) {
    return top == NULL;
}

// peek at top of stack
template <typename T>
T peek(stackNode<T>* top) {
    if (top == NULL) {
        cout << "Stack is empty" << endl;
        return T();
    }
    return top->data;
}

// size of stack
template <typename T>
int size(stackNode<T>* top) {
    int count = 0;
    stackNode<T>* temp = top;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

#endif