#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>
#include <mutex>
#include <vector>
#include "IntQueueHW6.h"
using namespace std;

// Created a struct in order to store the ID and thread in one each variable at the same time
struct player {
    thread playerThread;
    int ID;
};

mutex myLock;

void capture(IntQueueHW6 & game, const int & playerID, vector<player> & players, struct tm * ptr, vector<int> & remaining) {
    // We make the thread sleep for 2 seconds
    this_thread::sleep_until(chrono::system_clock::from_time_t(mktime(ptr)));

    myLock.lock(); // Locked the mutex in order to avoid synchronization problem
    if (!game.isFull()) { // Meaning that there are still empty chairs to capture
        game.enqueue(playerID);
        cout << "Player " << playerID << " captured a chair at " << put_time(ptr,"%X") << "." << endl;
        remaining.push_back(playerID); // Appending the player ID to the remaining players' list
    }
    else
        cout << "Player " << playerID << " couldn't capture a chair." << endl;
    myLock.unlock(); // Freeing the mutex
}

int main()
{
    int playerNum;

    cout << "Welcome to Musical Chairs game!" << endl;
    cout << "Enter the number of players in the game: " << endl;
    cin >> playerNum;
    cout << "Game Start!" << endl << endl;

    // ------------- This part is taken from the lecture slides -------------
    time_t timeNow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    struct tm *ptr = new struct tm;
    localtime_s(ptr, &timeNow);
    // ----------------------------------------------------------------------

    vector<int> temp; // Created a temporary vector that is used in order to retrieve the remaining players
    int tempNum = playerNum;
    while (playerNum != 1) { // Iterate until there is only 1 player left
        IntQueueHW6 game(playerNum - 1);
        vector<player> players(playerNum);
        vector<int> remaining; // Created a vector consisting of the remaining players' ID
        cout << "Time is now " << put_time(ptr, "%X") << endl;
        ptr->tm_sec += 2; // Incrementing the second by 2

        if (playerNum == tempNum) { // For the first iteration, the players' list will be filled in the order of the player numbers
            for (int i = 0; i < playerNum; i++) {
                players[i].ID = i;
                players[i].playerThread = thread(&capture, ref(game), ref(players[i].ID), ref(players), ptr,
                                                 ref(remaining));
            }
            for (int i = 0; i < playerNum; i++) {
                if (players[i].playerThread.joinable())
                    players[i].playerThread.join();
            }
        }
        else { // For the other iterations, the players' list will be filled with the stored ID's in the temp list
            for (int i = 0; i < playerNum; i++) {
                players[i].ID = temp[i];
                players[i].playerThread = thread(&capture, ref(game), ref(players[i].ID), ref(players), ptr, ref(remaining));
            }
            for (int i = 0; i < playerNum; i++) {
                if (players[i].playerThread.joinable())
                    players[i].playerThread.join(); // Join the threads to the main thread to avoid program crashes
            }
        }
        cout << "Remaining players are as follows: ";
        for (int i = 0; i < remaining.size(); i++) {
            if (i == remaining.size() - 1)
                cout << remaining[i] << endl << endl;
            else
                cout << remaining[i] << " ";
        }


































        temp = remaining; // Since the remaining players' list will get reset in each iteration, the data is stored in the temp list

        playerNum--; // Meaning that one player is eliminated
        if (playerNum == 1) {
            cout << "Game over!" << endl;
            cout << "Winner is Player " << remaining[0] << "!";
        }
    }
    
    return 0;
}