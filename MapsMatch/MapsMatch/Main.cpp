#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

struct Distance;
double delt = 0.1;
double quality = 0.9;

pair<double, double> inputdata(string line)
{
	char* x;
	char* y;
	char* init = new char[line.length() + 1];
	strcpy(init, line.c_str());
	x = strtok(init, " ");
	y = strtok(NULL, " ");
	return make_pair(atof(x), atof(y));
}

double leng(pair<double, double> p1, pair<double, double> p2)
{
	double length;
	length = pow(p1.first - p2.first, 2.0) +
			 pow(p1.second - p2.second, 2.0);
	return length;
}

void read(string filename, vector<pair<double, double>> *map)
{
	ifstream file(filename);
	int i = 0;
	string line;
	if(file == NULL)
	{
		cout<<"Open file error"<<endl;
		exit(1);
	}
	while(getline(file, line))
	{	
		(*map).push_back(inputdata(line));
	}
}

struct Distance
{
	double value;
	pair<double, double> a;
	pair<double, double> b;
	friend bool operator < (const Distance &x, const Distance &y) 
	{
		return x.value < y.value;
	}
	friend bool operator == (const Distance &x, const Distance &y) 
	{
		return abs(x.value - y.value) < delt;
	}
};

void countdist(vector<pair<double, double>> map, vector<Distance> *list)
{
	int i, j;
	Distance segment;
	for (i = 0; i < (int)map.size(); i++)
	{
		for (j = i+1; j < (int)map.size(); j++)
		{
			segment.value = leng(map[i], map[j]);
			segment.a = map[i];
			segment.b = map[j];
			(*list).push_back(segment);
		}
	}
	sort((*list).begin(), (*list).end());
}

void shiftmap(vector<pair<double, double>> *map, double x, double y)
{
	int i;
	for (i = 0; i < (int)(*map).size(); i++)
	{
		(*map)[i].first -= x;
		(*map)[i].second -= y;
	}
}
void shiftseg(Distance *seg)
{
	(*seg).b.first -= (*seg).a.first;
	(*seg).b.second -= (*seg).a.second;
	(*seg).a.first = 0.0;
	(*seg).a.second = 0.0;
}

void rotatemap(vector<pair<double, double>> *map, double cos, double sin)
{
	int i;
	for (i = 0; i < (int)(*map).size(); i++)
	{
		(*map)[i].first = (*map)[i].first * cos - (*map)[i].second * sin;
		(*map)[i].second = (*map)[i].second * cos + (*map)[i].first * sin;
	}
}

void rotateseg(Distance *seg, double cos, double sin)
{
	(*seg).b.first = (*seg).b.first * cos - (*seg).b.second * sin;
	(*seg).b.second = (*seg).b.second * cos + (*seg).b.first * sin;
}

bool checkmap(vector<pair<double, double>> map1, vector<pair<double, double>> map2)
{
	int i, eq = 0;
	double test;
	int n = min(map1.size(), map2.size());
	sort(map1.begin(), map1.end());
	sort(map2.begin(), map2.end());
	for (i = 0; i < n; i++)
	{
		if (map1[i] == map2[i])
		{
			eq++;
		}
	}
	test = 2.0*eq/(map1.size()+map2.size());
	return test > quality;
}

bool compare(	vector<pair<double, double>> map1, 
				vector<pair<double, double>> map2, 
				Distance seg1, 
				Distance seg2)
{
	double cos, sin;
	shiftmap(&map1, seg1.a.first, seg1.a.second);
	shiftmap(&map2, seg2.a.first, seg2.a.second);
	shiftseg(&seg1);
	shiftseg(&seg2);
	cos =	(seg1.b.first * seg2.b.first + seg1.b.second * seg2.b.second) /
			(sqrt(seg1.value) * sqrt(seg2.value));
	sin = sqrt(1.0 - cos*cos);
	rotateseg(&seg2, cos, sin);
	if (seg1.b == seg2.b)
	{
		rotatemap(&map2, cos, sin);
	}
	else
	{
		rotatemap(&map2, cos, -sin);
	}
	return checkmap(map1, map2);
}

bool comparemaps(	vector<pair<double, double>> map1, 
					vector<pair<double, double>> map2, 
					Distance seg1, 
					Distance seg2)
{
	bool option1 = false, option2 = false;
	pair<double, double> temp;
	option1 = compare(map1, map2, seg1, seg2);
	if (!option1)
	{
		temp.first = seg1.a.first;
		temp.second = seg1.a.second;
		seg1.a.first = seg1.b.first;
		seg1.a.second = seg2.b.second;
		seg1.b.first = temp.first;
		seg2.b.second = temp.second;
		option2 = compare(map1, map2, seg1, seg2);
	}
	return option1 || option2;
}

bool main()
{
	int i, j, k, n, m;
	vector<pair<double, double>> map1;
	vector<pair<double, double>> map2;
	vector<Distance> distlist1;
	vector<Distance> distlist2;
	string fm = "firstmap.txt";
	string sm = "secondmap.txt";
	read(fm, &map1);
	read(sm, &map2);
	countdist(map1, &distlist1);
	countdist(map2, &distlist2);
	n = distlist1.size();
	m = distlist2.size();
	k = 0;
	for(i = 0; i < n; i++)
	{
		j = k;
		while (j < m)
		{
			if (distlist1[i] == distlist2[j])
			{
				if (comparemaps(map1, map2, distlist1[i], distlist2[j]))
				{
					return true;
				}
			}
			else if (distlist1[i] < distlist2[j])
			{
				j = m;
			}
			else
			{
				k = j;
			}
			j++;
		}
	}
	return false;
}