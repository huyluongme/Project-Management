#include "Graph.h"

void cGraph::init_graph(int n) {
	G.n = task.size();
	G.m = 0;
	G.A.resize(n);
	for (int i = 0; i < n; i++) {
		G.A.at(i).resize(n);
		for (int j = 0; j < n; j++) {
			G.A.at(i).at(j) = 0;
		}
	}
}

int cGraph::find_id_task(std::wstring u) {
	for (int i = 0; i < task.size(); i++)
		if (u == task.at(i).task)
			return i;
	return -1;
}

void cGraph::add_edge() {
	for (int i = 0; i < task.size(); i++) {
		std::wstring tmp = task.at(i).pre;
		size_t found = tmp.find(L",");
		while (found != std::wstring::npos) {
			std::wstring v = tmp.substr(0, found);
			int v_v = find_id_task(v);
			if (v_v != -1) {
				G.A.at(i).at(v_v) += 1;
				task.at(i).adj_in.push_back(v);
				task.at(v_v).adj_out.push_back(task.at(i).task);
				task.at(v_v).deg_out++;
			}
			tmp.erase(0, found + 1);
			found = tmp.find(L",");
		}
		if (tmp.size() > 0) {
			int v_v = find_id_task(tmp);
			if (v_v != -1) {
				G.A.at(i).at(v_v) += 1;
				task.at(i).adj_in.push_back(tmp);
				task.at(v_v).adj_out.push_back(task.at(i).task);
				task.at(v_v).deg_out++;
			}
		}
	}
}

void copy_list(std::vector<int>* pS1, std::vector<int>* pS2) {
	pS1->clear();
	for (int i = 0; i < pS2->size(); i++)
		pS1->push_back(pS2->at(i));
}

void cGraph::deletetask(int id_task) {
	for (int i = 0; i < task.at(id_task).adj_in.size(); i++) {
		int v = find_id_task(task.at(id_task).adj_in.at(i));
		if (v != -1) {
			int found= -1;
			for(int j = 0; j < task.at(v).adj_out.size();j++)
				if (task.at(v).adj_out.at(j) == task.at(id_task).task) {
					found = j;
					break;
				}
			task.at(v).adj_out.erase(task.at(v).adj_out.begin() + found, task.at(v).adj_out.begin() + found + 1);
			task.at(v).deg_out--;
		}
	}
	for (int i = 0; i < task.at(id_task).adj_out.size(); i++) {
		int v = find_id_task(task.at(id_task).adj_out.at(i));
		if (v != -1) {
			int found = -1;
			for (int j = 0; j < task.at(v).adj_in.size(); j++)
				if (task.at(v).adj_in.at(j) == task.at(id_task).task) {
					found = j;
					break;
				}
			task.at(v).adj_in.erase(task.at(v).adj_in.begin() + found, task.at(v).adj_in.begin() + found + 1);
			task.at(v).deg_in--;
			std::wstring tmp = L"";
			for (int j = 0; j < task.at(v).adj_in.size(); j++) {
				tmp += task.at(v).adj_in.at(j);
				if (j < task.at(v).adj_in.size() - 1)
					tmp += L",";
			}
			task.at(v).pre.clear();
			task.at(v).pre = tmp;
		}
	}
	task.erase(task.begin() + id_task, task.begin() + id_task + 1);
	cG->calc();
}

bool my_cmp(const Task& a, const Task& b)
{
	return a.rank < b.rank;
}


void cGraph::rank() {
	init_graph(task.size());
	add_edge();
	std::vector<int>d;
	d.resize(G.n);
	r.resize(G.n);
	for (int u = 0; u < G.n; u++) {
		d[u] = task.at(u).deg_in;

	}
	std::vector<int> S1, S2;
	S1.clear();
	for (int u = 0; u < G.n; u++)
		if (d[u] == 0)
			S1.push_back(u);

	int k = 0; 
	while (!S1.empty()) {
		S2.clear();
		for (int i = 0; i < S1.size(); i++) {
			int u = S1.at(i); 
			r[u] = k;
			task.at(u).rank = r[u];
			for (int v = 0; v < G.n; v++)
				if (G.A[v][u] != 0) {
					d[v]--;
					if (d[v] == 0)
						S2.push_back(v);
				}
		}
		copy_list(&S1, &S2); 	
		k++; 				
	}
	std::sort(task.begin(), task.end(), my_cmp);
}

void cGraph::calc() {
	rank();
	int alpha = 0, beta,d_u_a = 0, d_u_b = 0, t_a = 0, T_a= 0, t_b = 0, T_b = 0;
	for (int i = 0; i < task.size(); i++) {
		if (task.at(i).deg_in == 0) {
			task.at(i).t = d_u_a + t_a;
		}
		else {
			std::vector<int> tmp;
			for (int j = 0; j < task.at(i).adj_in.size(); j++) {
				tmp.push_back(task.at( find_id_task( task.at(i).adj_in.at(j) )).dur + task.at(find_id_task(task.at(i).adj_in.at(j))).t);
			}
			task.at(i).t = *std::max_element(tmp.begin(), tmp.end());
		}
	}
	std::vector<int>tmp;
	for (int i = 0; i < task.size(); i++) {
		if (task.at(i).deg_out == 0) {
			tmp.push_back(task.at(i).dur + task.at(i).t);
		}
	}
	t_b = T_b = total_dur= *std::max_element(tmp.begin(), tmp.end());
	for (int i = task.size() - 1; i >= 0; i--) {
		if (task.at(i).deg_out == 0) {
			task.at(i).T = T_b - task.at(i).dur;
		}
		else {
			std::vector<int> tmp;
			for (int j = 0; j < task.at(i).adj_out.size(); j++) {
				tmp.push_back(task.at(find_id_task( task.at(i).adj_out.at(j))).T);
			}
			task.at(i).T = *std::min_element(tmp.begin(), tmp.end()) - task.at(i).dur;
		}
		if (task.at(i).t == task.at(i).T) task.at(i).critical = true;
	}
}