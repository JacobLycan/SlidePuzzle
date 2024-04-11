#include <iostream>
#include <cstdint>
#include "Random.h"
#include <cassert>


class Direction {

public:
	enum Type {
		up,
		down,
		left,
		right,
		max_directions
	};

	Direction(Type type)
		:m_type{ type }
	{

	}

	Direction operator-() const
	{
		switch (m_type)
		{
		case up: return Direction{ down };
		case down: return Direction{ up };
		case left: return Direction{ right };
		case right: return Direction{ left };
		default:
			return Direction{ up };
		}


	}
	Type getType()
	{
		return m_type;
	}

	friend std::ostream& operator<<(std::ostream& stream, Direction dir)
	{
		switch (dir.getType())
		{
		case Direction::up:
			stream << "up";
			break;
		case Direction::down:
			stream << "down";
			break;
		case Direction::left:
			stream << "left";
			break;
		case Direction::right:
			stream << "right";
			break;
		default:
			stream << "???";
		}
		return stream;
	}

	static Direction getRandomDirection()
	{
		Type random{ static_cast<Type>(Random::get(0, Type::max_directions - 1)) };
		return Direction{ random };
	}

private:
	Type m_type{};
};

namespace UserInput {

	bool isValidCommand(char ch)
	{
		return ch == 'w'
			|| ch == 'a'
			|| ch == 's'
			|| ch == 'd'
			|| ch == 'q';
	}

	char getCommandFromUser() {
		char ch{};
		while (!isValidCommand(ch)) {

			std::cin >> ch;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}


		return ch;
	}

	Direction charToDirection(char ch)
	{
		switch (ch)
		{
		case 'w': return Direction{ Direction::up };
		case 's': return Direction{ Direction::down };
		case 'a': return Direction{ Direction::left };
		case 'd': return Direction{ Direction::right };
		}

		return Direction{ Direction::up };
	}
};


struct Point {
	int x{};
	int y{};

	bool operator==(Point p) {
		return (x == p.x) && (y == p.y);
	}

	bool operator!=(Point p) {
		return !(*this == p);
	}

	Point getAdjacentPoint(Direction dir) const
	{
		switch (dir.getType())
		{
		case Direction::up:     return Point{ x, y - 1 };
		case Direction::down:   return Point{ x, y + 1 };
		case Direction::left:   return Point{ x - 1, y };
		case Direction::right:  return Point{ x + 1, y };
		}

		return *this;
	}

};

class Tile {
	int m_num;

public:
	Tile() = default;
	explicit Tile(int number)
		:m_num(number)
	{
	}

	friend std::ostream& operator<<(std::ostream& stream, Tile tile) {
		if (tile.m_num > 9) {
			stream << " " << tile.m_num << " ";
		} else if (tile.m_num > 0) {
			stream << "  " << tile.m_num << " ";
		} else if (tile.m_num == 0) {
			stream << "    ";
		}
		return stream;
	}

	int getNum() const {
		return m_num;
	}
	void setNum(int num) {
		m_num = num;
	}
	bool isEmpty() const {
		return m_num == 0;
	}
};

constexpr int g_consoleLines{ 25 };
class Board {
	static constexpr int size{ 4 };
	Tile m_tiles[size][size]{
		Tile{ 1 }, Tile { 2 }, Tile { 3 } , Tile { 4 },
		Tile { 5 } , Tile { 6 }, Tile { 7 }, Tile { 8 },
		Tile { 9 }, Tile { 10 }, Tile { 11 }, Tile { 12 },
		Tile { 13 }, Tile { 14 }, Tile { 15 }, Tile { 0 } };

public:
	Board() = default;

	static void printEmptyLines(int count)
	{
		for (int i = 0; i < count; ++i)
			std::cout << '\n';
	}


	friend std::ostream& operator<<(std::ostream& stream, Board board) {
		printEmptyLines(g_consoleLines);
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				stream << board.m_tiles[y][x];
			}
			stream << "\n";
		}
		return stream;
	}

	friend bool operator==(const Board& b1, const Board& b2)
	{
		for (int y = 0; y < size; ++y)
			for (int x = 0; x < size; ++x)
				if (b1.m_tiles[y][x].getNum() != b2.m_tiles[y][x].getNum())
					return false;

		return true;
	}

	bool isValid(Point p) {
		return (p.x >= 0) && (p.x < size) && (p.y >= 0) && (p.y < size);
	}

	Point emptyTilePos() {
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				if (m_tiles[y][x].getNum() == 0) {
					return Point(x, y);
				}
			}
		}

		assert(0 && "No Empty Tile");
		return { -1,-1 };
	}

	void swapTiles(Point p1, Point p2) {
		std::swap(m_tiles[p1.y][p1.x], m_tiles[p2.y][p2.x]);
	}

	bool moveTile(Direction dir)
	{
		Point emptyTile{ emptyTilePos() };
		Point adj{ emptyTile.getAdjacentPoint(-dir) };

		if (!isValid(adj))
			return false;

		swapTiles(adj, emptyTile);
		return true;
	}

	void randomize()
	{
		for (int i = 0; i < 1000; ++i)
		{
			bool moved = moveTile(Direction::getRandomDirection());
			
			if (!moved)
				--i;
		}
	}
	bool playerWon() const
	{
		static Board solved{};  // generate a solved board
		return  solved == *this; // player wins if current board == solved board
	}

};




int main()
{

	Board board{};
	board.randomize();
	std::cout << board;


	std::cout << "Enter a command: ";
	while (!board.playerWon())
	{
		char ch{ UserInput::getCommandFromUser() };

		if (ch == 'q')
		{
			std::cout << "\n\nBye!\n\n";
			return 0;
		}
		Direction dir{ UserInput::charToDirection(ch) };

		bool userMoved{ board.moveTile(dir) };
		if (userMoved)
			std::cout << board;
	}

	std::cout << "\n\nYou won!\n\n";
	return 0;
}