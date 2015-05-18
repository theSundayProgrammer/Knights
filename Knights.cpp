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


#include <tchar.h>
#include <vector>
#include <deque>
#include <stack>
#include <algorithm>
#include <iostream>
#include <exception>
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
struct KState
{
	int x;
	int y;
        int Parent; // pointer to the parent state
};

struct KDist
{
	int cx;
	int cy;
};

KState  operator+(KState const& inp, KDist const& d)
{
	KState res = {inp.x + d.cx, inp.y + d.cy};
	return res;
}
bool operator==(KState const& f, KState const& s)
{
	return f.x == s.x && f.y == s.y;
}
bool IsValidState(KState const& p)
{
	return p.x >= 0 && p.x <8 && p.y>=0 && p.y <8;
}
std::vector<KState> ValidMoves(KState const& inp)
{
	const int MAX_POSITIONS = 8; //There are at most 8 positions that knight can move to in one move
	KDist const moves[MAX_POSITIONS] = { { -2, -1 }, { -2, 1 }, { 2, -1 }, { 2, 1 }, { -1, -2 }, { -1, 2 }, { 1, -2 }, { 1, 2 } };
	std::vector<KState> outp;
	for (int i=0; i<MAX_POSITIONS; ++i)
	{
		KState newState= inp + moves[i];
		if (IsValidState(newState))
		{
			outp.push_back(newState);
		}
	}
	return outp;
}

std::ostream& operator<<(std::ostream& o, KState const& pos)
{
	char c= 'A' + pos.x;
	return o << c << pos.y+1;
}
void PrintOut(KState const& endState, KState const* seen, int curMax)
{
	std::stack<int> successMoves;
	int k=curMax;
	while (k!=0)
	{
		successMoves.push(k);
		k=seen[k].Parent;
	}

	while(!successMoves.empty())
	{
		int k = successMoves.top();
		successMoves.pop();
		std::cout << seen[k] << " ";
	}
	std::cout << endState << std::endl;

}


int Compute(KState const& startState, KState const& endState, KState* seen)
{
	int curMax=0;
	if (startState == endState)
		return curMax;

	std::deque<KState> tobeseen;
	tobeseen.push_back(startState);
	while (1)
	{
		KState pos=tobeseen.front();
		tobeseen.pop_front();
		seen[curMax]= pos;
		std::vector<KState> nextpositions = ValidMoves(pos);
		if(nextpositions.end() != 
			std::find(nextpositions.begin(),nextpositions.end(),endState))
		{
			//found
			break;
		}
		for (auto& it : nextpositions)
		{
			if (std::find(seen, seen + curMax, pos) == seen + curMax &&
				std::find(tobeseen.begin(), tobeseen.end(), it) == tobeseen.end())
			{
				it.Parent = curMax;
				tobeseen.push_back(it);
			}
		}
		++curMax;
	}
	return curMax;
}
KState ConvertFromStr(_TCHAR* arg)
{
	std::basic_string<TCHAR> sarg(arg);
	_TCHAR const* err= _T("Invalid Argument ");
	std::basic_string<_TCHAR> msg(err);
	msg += sarg;
	myexception ex(msg);
	if (sarg.length() !=2)
	{
		throw ex;
	}
	KState pos;
	pos.x = toupper(*arg) - 'A';
	pos.y = *++arg - '1';
	if (*++arg != '\0')
		throw ex;
	if (!IsValidState(pos))
	{
		throw ex;
	}
	pos.Parent=0;
	return pos;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc!=3)
	{
		std::cout << "Usage example:" << argv[0] << " D3 D4" << std::endl;
		return 0;
	}
	try 
	{
		const int MAX_BOARD_POSITIONS=64; 
		KState seen[MAX_BOARD_POSITIONS]; 
		//even if the breadth-first search covers ALL the positions on 
		//the board there are at most MAX_BOARD_POSITIONS;
		//hence 'seen' is of sufficient length
		KState startState=ConvertFromStr(argv[1]);
		KState endState=ConvertFromStr(argv[2]);
		int curMax = Compute(startState,endState,seen);
		PrintOut(endState, seen, curMax);
	}
	catch(std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

