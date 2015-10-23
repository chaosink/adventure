#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include "adventure.hpp"
#include "graphics.hpp"

extern GLFWwindow* window;
extern GLint position_x_max;
extern GLint position_y_max;
extern GLint position_z_max;
extern GLint position_x_min;
extern GLint position_y_min;
extern GLint position_z_min;
extern GLint position_x;
extern GLint position_y;
extern GLint position_z;
extern GLint door[6];
extern GLint find_monster;
extern GLint find_princess;
extern GLint success;

Castle::Castle(int k_room, int size) {
	assert(k_room > 0);
	capacity_ = size * size * size; 
	assert(k_room <= capacity_);
	k_room_ = k_room;
	size_.x = size_.y = size_.z = size;
	max_.x = max_.y = max_.z = size / 2;
	min_.x = min_.y = min_.z = size / 2 - size + 1;

	Generate();
}

Castle::Castle(int k_room, int size_x, int size_y, int size_z) {
	assert(k_room > 0);
	capacity_ = size_x * size_y * size_z;
	assert(k_room <= capacity_);
	k_room_ = k_room;
	size_.x = size_x;
	size_.y = size_y;
	size_.z = size_z;
	max_.x = size_x / 2;
	max_.y = size_y / 2;
	max_.z = size_z / 2;
	min_.x = max_.x - size_x + 1;
	min_.y = max_.y - size_y + 1;
	min_.z = max_.z - size_z + 1;

	Generate();
}

Castle::Castle(int k_room, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z) {
	assert(k_room > 0);
	capacity_ = (max_x - min_x + 1) * (max_y - min_y + 1) * (max_z - min_z + 1);
	assert(k_room <= capacity_);
	k_room_ = k_room;
	size_.x = max_x - min_x + 1;
	size_.y = max_y - min_y + 1;
	size_.z = max_z - min_z + 1;
	min_.x = min_x;
	min_.y = min_y;
	min_.z = min_z;
	max_.x = max_x;
	max_.y = max_y;
	max_.z = max_z;

	Generate();
}

void Castle::Generate() {
	cout << "Generating the castle. Please wait..." << endl;
	srand((unsigned)time(NULL));
//	srand((unsigned)clock());
	GenerateLobby();
	if(k_room_ != 1) GenerateRoom(k_room_ - 1, 0, 0, 0);
	printf("The castle generated! Location: x: %d~%d, y: %d~%d, z: %d~%d.\n", min_.x, max_.x, min_.y, max_.y, min_.z, max_.z);
	GenerateCharactor();
	position_x_min = min_.x;
	position_y_min = min_.y;
	position_z_min = min_.z;
	position_x_max = max_.x;
	position_y_max = max_.y;
	position_z_max = max_.z;
}

void Castle::GenerateCharactor() {
	princess_.room(rand() % k_room_ + 1);
	monster_.room(rand() % k_room_ + 1);
}

void Castle::GenerateLobby() {
	Room room(0, 0, 0, (k_room_ > 1 ? rand() % MaxDoor(0, 0, 0) + 1 : 0));
	room.k_door(0);
	room_.push_back(room);
	room_.push_back(room);
	room_count_[1] = 1;
	vector<int> coordinate(3, 0);
	room_index_[coordinate] = 1;
}

void Castle::GenerateRoom(int k_room, int x, int y, int z) {
	if(++ room_count_[room_.size() - 1] > k_room_ * 5) {
		int n = Unblock();
/*		Print();
		if(!n) {
			map<vector<int>, int> temp;
			vector<int> coordinate(3);
			for(vector<Room>::iterator it = room_.begin() + 1; it != room_.end(); it++) {
				coordinate[0] = it->coordinate(0);
				coordinate[1] = it->coordinate(1);
				coordinate[2] = it->coordinate(2);
				temp[coordinate] = 1;
			}
			for(int i = min_.x ; i <= max_.x; i++)
				for(int j = min_.y ; j <= max_.y; j++)
					for(int k = min_.z ; k <= max_.z; k++) {
						coordinate[0] = i;
						coordinate[1] = j;
						coordinate[2] = k;
						if(!temp[coordinate]) cout << i << " " << j << " " << k << " " << room_index_[coordinate] << " " << OneNeighbor(i, j, k) << endl;
					}
		}
		assert(n);*/
		k_room--;
		x = room_[n].coordinate_x();
		y = room_[n].coordinate_y();
		z = room_[n].coordinate_z();
//		Print();
//		Run();
	}
	vector<int> coordinate(3);
	coordinate[0] = x;
	coordinate[1] = y;
	coordinate[2] = z;
//	int m = MaxDoor(x, y, z);
//	if(!m) return;
//	int k = rand() % min(m, k_room) + 1;
	int n = room_index_[coordinate];
	int left = k_room;
	int a[7] = {0, 1, 2, 3, 4, 5};
	int j;
	for(int i = 0; i < 6; i++) {
		j = rand() % 6;
		a[6] = a[i];
		a[i] = a[j];
		a[j] = a[6];
	}
	j = 5;
	while(left) {
		j = (j + 1) % 6;
		coordinate[0] = x;
		coordinate[1] = y;
		coordinate[2] = z;
		switch(a[j]) {
			case 0: coordinate[1] = y + 1; break;
			case 1: coordinate[1] = y - 1; break;
			case 2: coordinate[0] = x - 1; break;
			case 3: coordinate[0] = x + 1; break;
			case 4: coordinate[2] = z + 1; break;
			case 5: coordinate[2] = z - 1; break;
		}
		int l = rand() % (left + 1);
		if(!l) continue;
		int nn = room_index_[coordinate];
		if(nn) {
			if(!room_[n].door(a[j]) && room_[n].k_door_max() > room_[n].k_door() && room_[nn].k_door_max() > room_[nn].k_door() && rand() % 6 == 0) {
				room_[n].door(a[j], nn);
				room_[n].k_door(room_[n].k_door() + 1);
				room_[nn].door(a[j] ^ 1, n);
				room_[nn].k_door(room_[nn].k_door() + 1);
			}
			if(room_[n].door(a[j])) {
//				cout << n << " *> " << nn << " l: " << l << " left: " << left - l << endl;
				GenerateRoom(l, coordinate[0], coordinate[1], coordinate[2]);
				left -= l;
			}
		} else if(Valid(coordinate[0], coordinate[1], coordinate[2]) && room_[n].k_door_max() > room_[n].k_door()) {
			nn = room_.size();
			room_index_[coordinate] = nn;
			room_[n].door(a[j], nn);
			room_[n].k_door(room_[n].k_door() + 1);
			Room room(coordinate[0], coordinate[1], coordinate[2], rand() % MaxDoor(coordinate[0], coordinate[1], coordinate[2]) + 1);
			room.door(a[j] ^ 1, n);
			room_.push_back(room);
//			cout << "room: " << n << " new: " << nn << endl;
//			cout << n << " -> " << nn << " l: " << l - 1 << " left: " << left - l << endl;
			if(l - 1) GenerateRoom(l - 1, coordinate[0], coordinate[1], coordinate[2]);
			left -= l;
		}
	}
}

int Castle::Valid(int x, int y, int z) {
	if(x < min_.x || x > max_.x || y < min_.y || y > max_.y || z < min_.z || z > max_.z)
		return 0;
	return 1;
}

int Castle::MaxDoor(int x, int y, int z) {
	int n = 0;

/*	coordinate[0] = x;
	coordinate[1] = y;
	coordinate[2] = z + 1;
	if(!room_index_[coordinate]) n++;
	coordinate[2] = z - 1;
	if(!room_index_[coordinate]) n++;

	coordinate[0] = x;
	coordinate[2] = z;
	coordinate[1] = y + 1;
	if(!room_index_[coordinate]) n++;
	coordinate[1] = y - 1;
	if(!room_index_[coordinate]) n++;

	coordinate[2] = z;
	coordinate[1] = y;
	coordinate[0] = x + 1;
	if(!room_index_[coordinate]) n++;
	coordinate[0] = x - 1;
	if(!room_index_[coordinate]) n++;*/

	if(Valid(x + 1, y, z)) n++;
	if(Valid(x - 1, y, z)) n++;
	if(Valid(x, y + 1, z)) n++;
	if(Valid(x, y - 1, z)) n++;
	if(Valid(x, y, z + 1)) n++;
	if(Valid(x, y, z - 1)) n++;

	return n;
}

int Castle::Unblock() {
	vector<int> coordinate(3);
	for(int i = min_.x ; i <= max_.x; i++)
		for(int j = min_.y ; j <= max_.y; j++)
			for(int k = min_.z ; k <= max_.z; k++) {
				coordinate[0] = i;
				coordinate[1] = j;
				coordinate[2] = k;
				int jj;
				if(!room_index_[coordinate] && (jj = OneNeighbor(i, j, k)) != -1) {
					int nn = room_.size();
					room_index_[coordinate] = nn;
					switch(jj) {
						case 0: coordinate[1] = j + 1; break;
						case 1: coordinate[1] = j - 1; break;
						case 2: coordinate[0] = i - 1; break;
						case 3: coordinate[0] = i + 1; break;
						case 4: coordinate[2] = k + 1; break;
						case 5: coordinate[2] = k - 1; break;
					}
					int n = room_index_[coordinate];
					room_[n].door(jj ^ 1, nn);
					room_[n].k_door(room_[n].k_door() + 1);
					Room room(i, j, k, rand() % MaxDoor(i, j, k) + 1);
					room.door(jj, n);
					room_.push_back(room);
//					cout << "                        Unblock: " << nn << " " << i << " " << j << " " << k << endl;
					return nn;
				}
			}
	return 0;
}

int Castle::OneNeighbor(int x, int y, int z) {
	vector<int> coordinate(3);
	int a[7] = {0, 1, 2, 3, 4, 5};
	int j;

	for(int i = 0; i < 6; i++) {
		j = rand() % 6;
		a[6] = a[i];
		a[i] = a[j];
		a[j] = a[6];
	}
	for(j = 0; j < 6; j++) {
		coordinate[0] = x;
		coordinate[1] = y;
		coordinate[2] = z;
		switch(a[j]) {
			case 0: coordinate[1] = y + 1; break;
			case 1: coordinate[1] = y - 1; break;
			case 2: coordinate[0] = x - 1; break;
			case 3: coordinate[0] = x + 1; break;
			case 4: coordinate[2] = z + 1; break;
			case 5: coordinate[2] = z - 1; break;
		}
		if(room_index_[coordinate]) return a[j];
	}

	return -1;
}

void Castle::Run() {
	vector<int> coordinate(3);
	string command;
	int com;
	int n;
	cout << "Press ENTER to start...";
	getchar();
//	cout << endl;
	glfwShowWindow(window);
	while(!glfwWindowShouldClose(window)) {
		position_x = coordinate[0] = hero_.coordinate_x();
		position_y = coordinate[1] = hero_.coordinate_y();
		position_z = coordinate[2] = hero_.coordinate_z();
		n = room_index_[coordinate];
		for(int i = 0; i < 6; i++) door[i] = room_[n].door(i);
		if(n == monster_.room()) find_monster = 1;
		if(!hero_.find_princess() && n == princess_.room()) find_princess = 1;
		if(find_princess && n == 1) success = 1;
		Render(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
//		printf("Welcome to Room[%d %d %d].\n", coordinate[0], coordinate[1], coordinate[2]);
		if(n == monster_.room()) {
/*			cout << "The monster found! Game over!" << endl;
			cout << "Press ENTER to try again...";*/
//			getchar();
//			cout << endl;
			hero_.Initialize();
			sleep(1);
			continue;
//			return;
		}
		if(!hero_.find_princess() && n == princess_.room()) {
			hero_.find_princess(1);
/*			cout << "The princess found! Well done!" << endl;
			cout << "Press ENTER to continue...";*/
//			getchar();
		}
		if(hero_.find_princess() && n == 1) {
/*			cout << "You have saved the princess from the castle successfully! Congratulations!" << endl;
			cout << "Press ENTER to quit..." << endl;*/
			sleep(1);
//			getchar();
			return;
		}
/*		printf("There are %d exits as:", room_[n].k_door());
		int j = room_[n].k_door();
		for(int i = 0; i < 6; i++) {
			if(room_[n].door(i)) {
				switch(i) {
					case 0: cout << " north[^](0)(W)"; break;
					case 1: cout << " south[v](1)(S)"; break;
					case 2: cout << " west[<](2)(A)"; break;
					case 3: cout << " east[>](3)(D)"; break;
					case 4: cout << " up[.](4)(E)"; break;
					case 5: cout << " down[x](5)(Q)"; break;
				}
				if(room_[n].door(i) >= 2 && j == 2) cout << " and";
				else cout << (j == 1 ? "." : ",");
				j--;
			}
		}
		cout << endl;
		cout << "Enter your command: ";
		cin >> command;
		getchar();
		com = 6;
		if(command == "w" || command == "W" || command == "0") com = 0;
		if(command == "s" || command == "S" || command == "1") com = 1;
		if(command == "a" || command == "A" || command == "2") com = 2;
		if(command == "d" || command == "D" || command == "3") com = 3;
		if(command == "e" || command == "E" || command == "4") com = 4;
		if(command == "q" || command == "Q" || command == "5") com = 5;
		while(com < 0 || com > 5 || !room_[n].door(com)) {
			cout << "Wrong command!" << endl;
			cout << "Enter your command: ";
			cin >> command;
			com = 6;
			if(command == "w" || command == "W" || command == "0") com = 0;
			if(command == "s" || command == "S" || command == "1") com = 1;
			if(command == "a" || command == "A" || command == "2") com = 2;
			if(command == "d" || command == "D" || command == "3") com = 3;
			if(command == "e" || command == "E" || command == "4") com = 4;
			if(command == "q" || command == "Q" || command == "5") com = 5;
		}
		hero_.Move(com);
		cout << endl;*/
	}
}

void Castle::Print() {
	vector<int> coordinate(3);
/*	cout << room_.size() - 1 << endl;
	for(vector<Room>::iterator it = room_.begin() + 1; it != room_.end(); it++)
		cout << it->k_door_max() - it->k_door() << " " << it->k_door() << " " << it->coordinate_x() << " " << it->coordinate_y() << " " << it->coordinate_z() << endl;
	cout << room_.size() - 1 << endl;*/
	cout << room_[1].k_door() << " " << room_[1].k_door_max() << endl;
/*	for(int i = min_.x; i <= max_.x; i++)
		for(int j = min_.y; j <= max_.y; j++)
			for(int k = min_.z; k <= max_.z; k++) {
				coordinate[0] = i;
				coordinate[1] = j;
				coordinate[2] = k;
				if(!room_index_[coordinate]) cout << i << " " << j << " " << k << endl;
			}*/
}

Room::Room(int x, int y, int z, int k) {
	coordinate_.x = x;
	coordinate_.y = y;
	coordinate_.z = z;
	k_door_ = 1;
	k_door_max_ = k;
	for(int i = 0; i < 6; i++) door_[i] = 0;
}

void Room::k_door_max(int k) {
	k_door_max_ = k;
}

int Room::k_door_max() {
	return k_door_max_;
}

void Room::k_door(int k) {
	k_door_ = k;
}

int Room::k_door() {
	return k_door_;
}

void Room::door(int k, int b) {
	door_[k] = b;
}

int Room::door(int k) {
	return door_[k];
}

int Room::coordinate_x() {
	return coordinate_.x;
}

int Room::coordinate_y() {
	return coordinate_.y;
}

int Room::coordinate_z() {
	return coordinate_.z;
}

void Charactor::name(string name_string) {
	name_ = name_string;	
}

string Charactor::name() {
	return name_;
}

void Charactor::room(int room_index) {
	room_ = room_index;
}

int Charactor::room() {
	return room_;
}

int Charactor::coordinate_x() {
	return coordinate_.x;
}

int Charactor::coordinate_y() {
	return coordinate_.y;
}

int Charactor::coordinate_z() {
	return coordinate_.z;
}

Hero::Hero() {
	Initialize();
}

void Hero::Initialize() {
	coordinate_.x = coordinate_.y = coordinate_.z = 0;
	room_ = 1;
	find_princess_ = 0;
}

void Hero::Behavior() {

}

void Hero::Move(int direction) {
	switch(direction) {
		case 0: coordinate_.y++; break;
		case 1: coordinate_.y--; break;
		case 2: coordinate_.x--; break;
		case 3: coordinate_.x++; break;
		case 4: coordinate_.z++; break;
		case 5: coordinate_.z--; break;
	}
}

void Hero::find_princess(int find) {
	find_princess_ = find;
}

int Hero::find_princess() {
	return find_princess_;
}

void Princess::Behavior() {
	
}

void Monster::Behavior() {
	
}
