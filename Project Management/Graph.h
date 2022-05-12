#pragma once

#include "framework.h"
class cGraph
{
public:
	void init_graph(int n);
	void add_edge();
	void rank();
	int find_id_task(std::wstring u);
	void deletetask(int id_task);
	void calc();
};

extern cGraph* cG;