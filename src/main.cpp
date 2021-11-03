#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <deque>
#include <list>
#include <time.h>
using namespace std;

struct Cell
{
    string name = "";
    int size = 0;
    int gain = 0;
    char set = 'A';
    bool lock = 0;
    vector<string> nets;
    bool operator == (const string &cell_name) const
    {
        return this->name == cell_name;
    }
    bool operator < (const Cell &cell) const
    {
        return this->size < cell.size;
    }
    friend  ostream  &operator<<(ostream &os, const Cell &cell){  //声明为友元，重载输出运算符
        os << cell.name << ":" << cell.size << " nets: ";
        for(auto iter=cell.nets.begin(); iter!=cell.nets.end(); iter++)
        {
            os << (*iter) <<", ";
        }
        os <<"gain: "<<cell.gain<<" set: " <<cell.set<<" lock: "<<cell.lock<<endl;
        return os;
    }
};

struct Net
{
    string name = "";
    list<Cell*> cells;
    int A_n = 0;
    int B_n = 0;
    friend  ostream  &operator<<(ostream &os, const Net &net){  //声明为友元，重载输出运算符
        os << net.name << ":" << " cells: ";
        for(auto iter=net.cells.begin(); iter!=net.cells.end(); iter++)
        {
            os << *(*iter) <<", ";
        }
        os <<"A:B = "<< net.A_n <<":"<<net.B_n<<endl;
        return os;
    }
};

struct Solution
{
    Cell* p_cell;
    int n_cut;
    friend  ostream  &operator<<(ostream &os, const Solution &solution){  //声明为友元，重载输出运算符
        os << "cell:" << *(solution.p_cell) << "cut: " << solution.n_cut<<endl;
        return os;
    }
};

void cell_parser(char *filename, unordered_map<string,Cell> &cells, vector<Cell> &cells_vec)
{
    ifstream cells_file;
    cells_file.open(filename, ios::in);
    if (!cells_file.is_open())
	{
		cout << "read .cells file failed" << endl;
	}
    string line;
    string buf;
    string name;
    Cell cell;
    vector<string> temp_buf;
    while (getline(cells_file,line))
    {
        line += '\n';
        for(auto iter=line.begin();iter!=line.end();iter++)
        {
            if((*iter) == ' ' || (*iter) == '\n')
            {
                if(buf == "")
                {
                    continue;
                }
                temp_buf.push_back(buf);
                buf = "";
            }
            else
            {
                buf += (*iter);
            }
        }
        cell.name = temp_buf[0];
		cell.size = stoi(temp_buf[1]);
        cells.insert(make_pair(cell.name,cell));
        //cells_key.push_back(cell.name);
        cells_vec.push_back(cell);
        temp_buf.clear();
    }
    //return cells;
}

vector<Net> net_parser(char *filename, unordered_map<string,Cell> &cells)
{
    ifstream nets_file;
    nets_file.open(filename, ios::in);
    if (!nets_file.is_open())
	{
		cout << "read .nets file failed" << endl;
	}
    string line;
    string temp_line;
    string buf;
    deque<string> temp_s;
    vector<Net> nets;
    Net net;
    while (getline(nets_file,temp_line))
    {
        //if(temp_line.at(temp_line.length()-1)!='}')
        if(temp_line.find('}') == string::npos)
        {
            line += temp_line;
            line += '\n';
            continue;
        }
        else
        {
            line += temp_line;
            for(auto iter=line.begin();iter!=line.end();iter++)
            {
                if ((*iter) == '}')
                {
                    line = "";
                    break;
                }
                else if((*iter) != ' ' && (*iter) !='{' && (*iter) !='\n')
                {
                    buf += (*iter);
                }
                else if((*iter) != ' ' && (*iter) == '{')
                {
                    iter++;
                }
                else if((*iter) != ' ' && (*iter) == '\n')
                {
                    iter++;
                }
                else if((*iter) == ' ')
                {
                    if(buf == "")
                    {
                        continue;
                    }
                    temp_s.push_back(buf);
                    buf = "";
                }
            }
            temp_s.pop_front();
            net.name = temp_s[0];
            temp_s.pop_front();
            while(temp_s.empty() != 1)
            {
                //auto it = find(cells.begin(), cells.end(),temp_s[0]);
                //Cell *p_cell = &(*it);
                Cell *p_cell = &(cells[temp_s[0]]);
                net.cells.push_back(p_cell);
                temp_s.pop_front();
            }
            //sort(net.cells.begin(),net.cells.end());
            nets.push_back(net);
            net.cells.clear();
        }
    }
    return nets;
}

void create_net_array(vector<Net> &nets, unordered_map<string,Net*> &nets_hash)
{
    for(auto &i : nets){
        for(auto &j : i.cells)
        {
            j->nets.push_back(i.name);
        }
        nets_hash.insert(make_pair(i.name,(&i)));
    }
}

vector<string> get_cell_hash_key(vector<Cell> &cells_vec)
{
    vector<string> cells_key;
    for(auto &i : cells_vec){
        string cell_key = i.name;
        cells_key.push_back(cell_key);
    }
    return cells_key;
}

inline bool constraint_check(int area_A, int area_B, int area_n) { return abs(area_A - area_B) < (area_n/10); }

void init_solution(vector<string> cells_key, unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, int &n, int &area_A, int &area_B)
{
    for(auto iter=cells_key.begin(); iter!=cells_key.end();iter++)
    {
        n += set_A[*iter]->size;
    }
    area_A = n;
    int i=0;
    string cell_key;
    while(constraint_check(area_A,area_B,n) != true)
    {
        cell_key = cells_key[i];
        set_A[cell_key]->set = 'B';
        set_B.insert(make_pair(cell_key,set_A[cell_key]));
        area_B += set_A[cell_key]->size;
        area_A -= set_A[cell_key]->size;
        set_A.erase(cell_key);
        i++;
    }
}

void init_gain(unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, vector<Net> &nets, int &init_cut)
{
    list<Cell*> curr_net_cell;
    int A_n = 0; //当前net，setA
    int B_n = 0;
    vector<string> temp_A_key;
    vector<string> temp_B_key;
    for(auto &i : nets)
    {
        curr_net_cell = i.cells;
        for(auto &j : curr_net_cell)
        {
            if (set_A.find(j->name) != set_A.end())
            {
                A_n += 1;
                temp_A_key.push_back(j->name);
            }
            else
            {
                B_n += 1;
                temp_B_key.push_back(j->name);
            }
        }
        if(A_n == 1)
        {
            for(auto &k : temp_A_key)
            {
                set_A[k]->gain += 1;
            }
        }
        if(B_n == 1)
        {
            for(auto &k : temp_B_key)
            {
                set_B[k]->gain += 1;
            }
        }
        if(B_n == 0)
        {
            for(auto &k : temp_A_key)
            {
                set_A[k]->gain -= 1;
            }
        }
        if(A_n == 0)
        {
            for(auto &k : temp_B_key)
            {
                set_B[k]->gain -= 1;
            }
        }
        //cout <<i.name<<": "<< A_n <<":" << B_n<<endl;
        // cout << "setA:"<<endl;
        // print_d<vector<string>>(temp_A_key);
        // cout << "setB:"<<endl;
        // print_d<vector<string>>(temp_B_key);
        if(A_n != 0 && B_n != 0)
        {
            init_cut += 1;
        }
        i.A_n = A_n;
        i.B_n = B_n;
        temp_A_key.clear();
        temp_B_key.clear();
        A_n = 0;
        B_n = 0;
    }
}
void create_bucket(unordered_map<string,Cell> &cells_hash, map<int,unordered_map<string,Cell*>,greater<int>> &bucket)
{
    int p_max = 0;
    //find max_pin of set A
    for (auto& i : cells_hash)
    {
        //cout <<i.first<<":"<<i.second.nets.size()<<":"<<i.second.gain<<endl;
        //cout<<i.second.nets.size()<<endl;
        if(i.second.nets.size() > p_max)
        {
            p_max = i.second.nets.size();
        }
    }
    //bucket init
    for (int i= -p_max; i<=p_max; i++)
    {
        unordered_map<string,Cell*> gain;
        bucket.insert(make_pair(i,gain));
    }
    //create bucket
    for (auto& i : cells_hash)
    {
        bucket[i.second.gain].insert(make_pair(i.first,&(i.second)));
    }
}


// bool check_and_swap(Cell* cell, int &area_A, int &area_B, int &area_n)
// {
//     if (cell->set == 'A')
//     {
//         if (constraint_check(area_A-(cell->size), area_B+(cell->size), area_n) == 1)
//         {
//             area_A = area_A-(cell->size);
//             area_B = area_B+(cell->size);
//             cell->lock = 1;
//             cell->set = 'B';
//             return 1;
//         }
//         else
//         {
//             return 0;
//         }
//     }
//     else if (cell->set == 'B')
//     {
//         if (constraint_check(area_A+(cell->size), area_B-(cell->size), area_n) == 1)
//         {
//             area_A = area_A+(cell->size);
//             area_B = area_B-(cell->size);
//             cell->lock = 1;
//             cell->set = 'A';
//             return 1;
//         }
//         else
//         {
//             return 0;
//         }
//     }
    
// }

// Cell* find_cell(map<int,unordered_map<string, Cell*>,greater<int>> &bucket, int &area_A, int &area_B, int &area_n)
// {
//     unordered_map<string, Cell*> *p_max_cells;
//     Cell *p_max_cell;
//     bool swap;
//     for(auto &i : bucket)
//     {
//         p_max_cells = &(i.second);
//         for(auto &j : *p_max_cells)
//         {
//             p_max_cell = j.second;
//             if(p_max_cell->lock == 1)
//             {
//                 continue;
//             }
//             else
//             {
//                 swap = check_and_swap(p_max_cell, area_A, area_B, area_n);
//                 if (swap == 1)
//                 {
//                     p_max_cells->erase(p_max_cell->name);
//                     return p_max_cell;
//                 }
//             }
//         }
//     }
// }

int get_max(map<int,unordered_map<string, Cell*>,greater<int>> &bucket)
{
    int max_gain = 0;
    for (auto &i : bucket)
    {
        if(i.second.empty() != 1)
        {
            max_gain = i.first;
            return max_gain;
        }
    }
}

Cell* find_cell(map<int,unordered_map<string, Cell*>,greater<int>> &bucket, int &area_A, int &area_B, int &area_n)
{
    int max_gain = get_max(bucket);
    unordered_map<string, Cell*> *p_candidate;
    Cell *max_cell = NULL;
    while(max_cell == NULL)
    {
        p_candidate = &(bucket[max_gain]);
        for (auto &i : *p_candidate)
        {
            if (i.second->set == 'A')
            {
                if (constraint_check(area_A - (i.second->size), area_B + (i.second->size), area_n))
                {
                    area_A = area_A - (i.second->size);
                    area_B = area_B + (i.second->size);
                    max_cell = i.second;
                    max_cell->lock = 1;
                    max_cell->set = 'B';
                    p_candidate->erase(max_cell->name);
                    return max_cell;
                }
            }
            else if (i.second->set == 'B')
            {
                if (constraint_check(area_A + (i.second->size), area_B - (i.second->size), area_n))
                {
                    area_A = area_A + (i.second->size);
                    area_B = area_B - (i.second->size);
                    max_cell = i.second;
                    max_cell->lock = 1;
                    max_cell->set = 'A';
                    p_candidate->erase(max_cell->name);
                    return max_cell;
                }
            }  
        }
        max_gain -= 1;
        while (bucket[max_gain].empty() == 1) //如果当前max gain已无元素，则更新max_gain
        {
            max_gain -= 1;
        }
    }
}

void update_gain(map<int,unordered_map<string,Cell*>,greater<int>> &bucket, unordered_map<string,Net*> &nets_hash, Cell* &max_cell)
{   
    if(max_cell->set == 'B')
    {
        for (auto &i : max_cell->nets) //当前拿走的cell的所有net
        {
            if(nets_hash[i]->B_n == 0)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        bucket[j->gain].erase(j->name);
                        j->gain += 1;
                        bucket[j->gain].insert(make_pair(j->name,j));
                    }
                }
            }
            else if(nets_hash[i]->B_n == 1)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        if(j->set == 'B')
                        {
                            bucket[j->gain].erase(j->name);
                            j->gain -= 1;
                            bucket[j->gain].insert(make_pair(j->name,j));
                        }
                    }
                }
            }
            nets_hash[i]->B_n += 1;
            nets_hash[i]->A_n -= 1;
            if(nets_hash[i]->A_n == 0)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        bucket[j->gain].erase(j->name);
                        j->gain -= 1;
                        bucket[j->gain].insert(make_pair(j->name,j));
                    }
                }   
            }
            else if(nets_hash[i]->A_n == 1)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        if(j->set == 'A')
                        {
                            bucket[j->gain].erase(j->name);
                            j->gain += 1;
                            bucket[j->gain].insert(make_pair(j->name,j));
                        }
                    }
                } 
            }
        }
    }
    else if(max_cell->set == 'A')
    {
        for (auto &i : max_cell->nets) //当前拿走的cell的所有net
        {
            if(nets_hash[i]->A_n == 0)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        bucket[j->gain].erase(j->name);
                        j->gain += 1;
                        bucket[j->gain].insert(make_pair(j->name,j));
                    }
                }
            }
            else if(nets_hash[i]->A_n == 1)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        if(j->set == 'A')
                        {
                            bucket[j->gain].erase(j->name);
                            j->gain -= 1;
                            bucket[j->gain].insert(make_pair(j->name,j));
                        }
                    }
                }
            }
            nets_hash[i]->B_n -= 1;
            nets_hash[i]->A_n += 1;
            if(nets_hash[i]->B_n == 0)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        bucket[j->gain].erase(j->name);
                        j->gain -= 1;
                        bucket[j->gain].insert(make_pair(j->name,j));
                    }
                }   
            }
            else if(nets_hash[i]->B_n == 1)
            {
                for(auto &j : nets_hash[i]->cells)
                {
                    if(j->lock == 1)
                    {
                        continue;
                    }
                    else
                    {
                        if(j->set == 'B')
                        {
                            bucket[j->gain].erase(j->name);
                            j->gain += 1;
                            bucket[j->gain].insert(make_pair(j->name,j));
                        }
                    }
                } 
            }
        }
    }
}

void best_result(unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, vector<Solution> &solution, int init_cut, int best_step)
{
    int n_cut = init_cut;
    for(auto iter = solution.begin(); iter != solution.begin()+best_step+1; iter++)
    {
        if(iter->p_cell->set == 'B')
        {
            set_A.erase(iter->p_cell->name);
            set_B.insert(make_pair(iter->p_cell->name, iter->p_cell));
        }
        else if(iter->p_cell->set == 'A')
        {
            set_B.erase(iter->p_cell->name);
            set_A.insert(make_pair(iter->p_cell->name, iter->p_cell));
        }
    }
}

int FM(map<int,unordered_map<string,Cell*>,greater<int>> &bucket, unordered_map<string,Net*> &nets_hash, int &area_A, int &area_B, int &area_n, int init_cut, vector<Solution> &solution, int iter, unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B)
{
    int curr_cut = init_cut;
    Solution curr_solution;
    Cell *max_cell;
    int best_cut = init_cut;
    int best_step = 0;
    for(int i=0; i<iter; i++)
    {
        max_cell = find_cell(bucket, area_A, area_B, area_n);
        update_gain(bucket, nets_hash, max_cell);
        curr_cut -= max_cell->gain;
        curr_solution.p_cell = max_cell;
        curr_solution.n_cut = curr_cut;
        solution.push_back(curr_solution);
        if (curr_cut < best_cut)
        {
            best_cut = curr_cut;
            best_step = i;
        }
    }
    best_result(set_A, set_B, solution, init_cut, best_step);
    return best_cut;
}

void result_output(char *filename, int best_cut, unordered_map<string,Cell*> set_A, unordered_map<string,Cell*> set_B)
{
    ofstream out_file;
    out_file.open(filename, ios::out);
    out_file << "cut_size " << best_cut << endl;
    out_file << "A " << set_A.size() << endl;
    for (auto &i : set_A)
    {
        out_file << i.first <<endl;
    }
    out_file << "B " << set_B.size() << endl;
    for (auto &i : set_B)
    {
        out_file << i.first <<endl;
    }
    out_file.close();
}

int main(int argc, char *argv[])
{
    clock_t start,end;
    start =clock();
    char *cell_filename = argv[2];
    unordered_map<string,Cell> cells_hash;
    vector<Cell> cells_vec;
    vector<string> cells_key;
    cell_parser(cell_filename,cells_hash,cells_vec);
    sort(cells_vec.begin(),cells_vec.end());
    cells_key = get_cell_hash_key(cells_vec);
    cells_vec.clear();
    char *net_filename = argv[1];
    vector<Net> nets;
    nets=net_parser(net_filename,cells_hash);
    unordered_map<string,Net*> nets_hash;
    create_net_array(nets, nets_hash);
    unordered_map<string,Cell*> set_A;
    for (auto &i:cells_hash)
    {
        set_A.insert(make_pair(i.first,&(i.second)));
    }
    unordered_map<string,Cell*> set_B;
    int area_n = 0;
    int area_A = 0;
    int area_B = 0;
    init_solution(cells_key, set_A, set_B, area_n, area_A, area_B);
    int init_cut = 0;
    init_gain(set_A, set_B, nets, init_cut);
    map<int,unordered_map<string,Cell*>, greater<int>> bucket;
    int max_gain_A = 0;
    int max_gain_B = 0;
    create_bucket(cells_hash, bucket);

    vector<Solution> solution;
    int best_cut = init_cut;
    best_cut = FM(bucket, nets_hash, area_A, area_B, area_n,init_cut, solution, cells_hash.size(), set_A, set_B);

    char *out_filename = argv[3];
    result_output(out_filename, best_cut, set_A, set_B);
    end =clock();
    cout <<"best cut: "<< best_cut << " time: "<< (double) (end-start)/CLOCKS_PER_SEC<<endl;
}