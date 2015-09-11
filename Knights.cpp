// Knaves.cpp : Defines the entry point for the console application.
//
// This file can be built using MinGW  (gcc 4.6.2) and VS2010 console
// For MinGW the console commands to build are:
//    gcc -c -std=c++0x knaves.cpp
//    g++ -o knaves.exe knaves.o
//
//  For VS2010 the console commands to build are;
//  CL /EHsc knaves.cpp
// 
// Usage:
//   Knaves d4 B5
//
//
//
//The Compute function is responsible for the algorithm
//



#include <vector>
#include <deque>
#include <stack>
#include <algorithm>
#include <iostream>
#include <exception>
#include <functional>
const int MAX_NODE_COUNT = 64;


class myexception: public std::exception
{
public:
	myexception(std::string const& str):str_(str){}
	virtual const char* what() const throw()
	{
		return str_.c_str();
	}
	~myexception() throw(){}
private:
	std::string str_;
};

//State is the chess board
template<class T>
struct Tracker
{
	T pos;
	int Parent; // pointer to the parent state
};



template<class T>
void PrintOut(Tracker<T> const& endState, Tracker<T> const* seen, int curMax)
{
	std::stack<int> successMoves;
	int k = curMax;
	while (k != 0)
	{
		successMoves.push(k);
		k = seen[k].Parent;
	}

	while (!successMoves.empty())
	{
		int k = successMoves.top();
		successMoves.pop();
		std::cout << seen[k].pos << " ";
	}
	std::cout << endState.pos << std::endl;

}


template<class T, class Container >
int Compute(Tracker<T> const& startState, Tracker<T> const& endState, Tracker<T>* seen, std::function< Container(T const&)> Neighbours)
{
	int curMax = 0;
	if (startState.pos == endState.pos)
		return curMax;

	std::deque<Tracker<T>> tobeseen;
	tobeseen.push_back(startState);
	while (!tobeseen.empty())
	{
		Tracker<T> pos = tobeseen.front();
		tobeseen.pop_front();
		seen[curMax] = pos;
		Container neighbours = Neighbours(pos.pos);
		if (neighbours.end() !=
			std::find(neighbours.begin(), neighbours.end(), endState.pos))
		{
			//found
			return curMax;
		}
		for (auto& it : neighbours)
		{
			if ((std::find(seen, seen + curMax, pos) == seen + curMax) &&
				(std::find_if(tobeseen.begin(), tobeseen.end(), [=](Tracker<T> const & p) {return p.pos == it; }) == tobeseen.end()))
			{
				Tracker<T> val{ it, curMax };
				tobeseen.emplace_back(val);
			}
		}
		++curMax;
	}
	return 0;
}

template<class T>
bool operator==(Tracker<T> const& f, Tracker<T> const& s)
{
	return f.pos == s.pos;
}

struct node
{
	int x;
	int y;
    int index () {return x*8 + y;}
};

struct KDist
{
	int cx;
	int cy;
};



node  operator+(node const& inp, KDist const& d)
{
	node res = {inp.x + d.cx, inp.y + d.cy};
	return res;
}
bool operator==(node const& f, node const& s)
{
	return f.x == s.x && f.y == s.y;
}

bool IsValidState(node const& p)
{
	return p.x >= 0 && p.x <8 && p.y>=0 && p.y <8;
}
std::vector<node> Neighbours(node const& inp)
{
	const int MAX_POSITIONS = 8; //There are at most 8 positions that knight can move to in one move
	KDist const moves[MAX_POSITIONS] = { { -2, -1 }, { -2, 1 }, { 2, -1 }, { 2, 1 }, { -1, -2 }, { -1, 2 }, { 1, -2 }, { 1, 2 } };
	std::vector<node> outp;
	for (int i=0; i<MAX_POSITIONS; ++i)
	{
		node newState= inp + moves[i];
		if (IsValidState(newState))
		{
			outp.push_back(newState);
		}
	}
	return outp;
}

std::ostream& operator<<(std::ostream& o, node const& pos)
{
	char c= 'A' + pos.x;
	return o << c << pos.y+1;
}

typedef char _TCHAR;
#define _T(x) x
node ConvertFromStr(_TCHAR* arg)
{
	std::basic_string<_TCHAR> sarg(arg);
	_TCHAR const* err= _T("Invalid Argument ");
	std::basic_string<_TCHAR> msg(err);
	msg += sarg;
	myexception ex(msg);
	if (sarg.length() !=2)
	{
		throw ex;
	}
	node pos;
	pos.x = toupper(*arg) - 'A';
	pos.y = *++arg - '1';
	if (*++arg != '\0')
		throw ex;
	if (!IsValidState(pos))
	{
		throw ex;
	}
	return pos;
}

int main(int argc, _TCHAR* argv[])
{
	if (argc!=3)
	{
		std::cout << "Usage example:" << argv[0] << " D3 D4" << std::endl;
		return 0;
	}
	try
	{
		typedef Tracker<node> KState;
		KState seen[MAX_NODE_COUNT];
		//even if the breadth-first search covers ALL the positions on 
		//the board there are at most MAX_NODE_COUNT nodes
		//hence 'seen' is of sufficient length
		KState startState{ ConvertFromStr(argv[1]),0 };
		KState endState {ConvertFromStr(argv[2]), 0};
		int curMax = Compute<node, std::vector<node>>(startState,endState,seen,Neighbours);
		PrintOut(endState, seen, curMax);
	}
	catch(std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

