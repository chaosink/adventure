#ifndef __ADVENTURE__
#define __ADVENTURE__

#include <vector>
#include <map>
#include <string>
using namespace std;

class Castle;
class Room;
class Charactor;
class Hero;
class Princess;
class Monster;
struct Coordinate;

struct Coordinate {
	int x, y, z;
};

class Charactor {
protected:
	Coordinate coordinate_;
	int room_;
	string name_;
public:
	virtual void Behavior() = 0;
	void name(string name_string);
	string name();
	void room(int room_index);
	int room();
	int coordinate_x();
	int coordinate_y();
	int coordinate_z();
};

class Hero : public Charactor {
	int find_princess_;
public:
	Hero();
	virtual void Behavior();
	void Move(int direction);
	void find_princess(int find);
	int find_princess();
	void Initialize();
};

class Princess : public Charactor {
public:
	virtual void Behavior();
};

class Monster : public Charactor {
public:
	virtual void Behavior();
};

class Room {
	Coordinate coordinate_;
	int k_door_max_;
	int k_door_;
	int door_[6];
public:
	Room(int x, int y, int z, int k);
	void k_door_max(int k);
	int k_door_max();
	void k_door(int k);
	int k_door();
	void door(int k, int b);
	int door(int k);
	int coordinate_x();
	int coordinate_y();
	int coordinate_z();
};

class Castle {
	Coordinate size_;
	Coordinate min_;
	Coordinate max_;
	int k_room_;
	map<int, int> room_count_;
	int capacity_;
	vector<Room> room_;
	map<vector<int>, int> room_index_;
	Hero hero_;
	Princess princess_;
	Monster monster_;
	void Generate();
	void GenerateLobby();
	void GenerateRoom(int k_room, int x, int y, int z);
	void GenerateCharactor();
	int Valid(int x, int y, int z);
	int MaxDoor(int x, int y, int z);
	int Unblock();
	int OneNeighbor(int x, int y, int z);
public:
	Castle(int k_room, int size);
	Castle(int k_room, int size_x, int size_y, int size_z);
	Castle(int k_room, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z);
	void Run();
	void Print();
};

#endif
