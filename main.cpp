#include <iostream>
#include "adventure.hpp"
using namespace std;

int main() {
	int k_room, size;
	cout << "Please input the side length of the castle: ";
	cin >> size;
	cout << "Please input the number of the rooms in the castle: ";
	cin >> k_room;
	cout << endl;
	Castle castle(k_room, size);
//	Castle castle(125, 50);
//	Castle castle(64, 4);
//	Castle castle(8, 2);
//	Castle castle(8, 0, 1, 0, 1, 0, 1);
//	castle.Print();
	castle.Run();
	return 0;
}
