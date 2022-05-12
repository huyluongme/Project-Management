#pragma once

typedef struct {
	std::wstring task;
	std::wstring descr;
	int dur, deg_in, rank,t,T,deg_out;
	bool critical;
	std::wstring pre;
	std::vector<std::wstring> adj_in, adj_out;
}Task;

typedef struct {
	int n, m;
	std::vector<std::vector<int>> A;
}Graph;

typedef std::vector<Task> T;
extern T task;
extern Graph G;
extern std::vector<int> r;
extern int total_dur;