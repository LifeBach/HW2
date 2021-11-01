#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <deque>
#include <list>
#include <set>
using namespace std;

struct Cell
{
    string name = "";
    int size = 0;
    int gain = 0;
    char set = 'A'; //test
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
        os <<"gain: "<<cell.gain<<" set: " <<cell.set<<endl;
        return os;
    }
};

struct Net
{
    string name = "";
    vector<Cell*> cells;
    int A_n = 0;
    int B_n = 0;
    friend  ostream  &operator<<(ostream &os, const Net &net){  //声明为友元，重载输出运算符
        os << net.name << ":" << " cells: ";
        for(auto iter=net.cells.begin(); iter!=net.cells.end(); iter++)
        {
            os << (*iter) <<", ";
        }
        os <<"A:B = "<< net.A_n <<":"<<net.B_n<<endl;
        return os;
    }
};

template<typename T>
void print_d(T& d)
{
    int counter = 0;
    for (auto &i : d) {
        cout <<"data["<<counter<<"]"<< i <<endl;
        counter++;
    }
}

void cell_parser(char *filename, unordered_map<string,Cell> &cells, vector<Cell> &cells_vec)
{
    ifstream cells_file;
    cells_file.open(filename, ios::in);
    if (!cells_file.is_open())
	{
		cout << "read .cells file failed" << endl;
	}
    string buf;
    string name;
    Cell cell;
    while (getline(cells_file,buf))
    {
        //cout<<buf;
        int index1 = buf.find_first_of(' '); 
		cell.name = buf.substr(0,index1); 
		cell.size = stoi(buf.substr(index1 + 1,sizeof(buf)));
        cells.insert(make_pair(cell.name,cell));
        //cells_key.push_back(cell.name);
        cells_vec.push_back(cell);
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
        if(temp_line.at(temp_line.length()-1)!='}')
        {
            line += temp_line;
            line += '\n';
            continue;
        }
        else
        {
            line += temp_line;
            for(auto iter=line.begin()+4;iter!=line.end();iter++)
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
    vector<Cell*> curr_net_cell;
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
void create_bucket(unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, map<int,unordered_map<string,Cell*>> &bucket_A, map<int,unordered_map<string,Cell*>> &bucket_B,int &max_gain_A, int &max_gain_B)
{
    int p_max_A = 0;
    //find max_pin of set A
    for (auto& i : set_A)
    {
        //cout <<i.first<<":"<<i.second.nets.size()<<":"<<i.second.gain<<endl;
        //cout<<i.second.nets.size()<<endl;
        if(i.second->nets.size() > p_max_A)
        {
            p_max_A = i.second->nets.size();
        }
    }
    //bucket A init
    for (int i= -p_max_A; i<=p_max_A; i++)
    {
        unordered_map<string,Cell*> gain_A;
        bucket_A.insert(make_pair(i,gain_A));
    }
    //create bucket_A
    for (auto& i : set_A)
    {
        bucket_A[i.second->gain].insert(make_pair(i.first,i.second));
        //bucket_A[i.second->gain].push_back(i.second);
        if(i.second->gain > max_gain_A)
        {
            max_gain_A = i.second->gain;
        }
    }
    
    int p_max_B=0;
    //find max_pin of set B
    for (auto& i : set_B)
    {
        //cout <<i.first<<":"<<i.second.nets.size()<<":"<<i.second.gain<<endl;
        //cout<<i.second.nets.size()<<endl;
        if(i.second->nets.size() > p_max_B)
        {
            p_max_B= i.second->nets.size();
        }
    }
    //bucket B init
    for (int i= -p_max_B; i<=p_max_B; i++)
    {
        unordered_map<string,Cell*> gain_B;
        bucket_B.insert(make_pair(i,gain_B));
    }
    for (auto& i : set_B)
    {
        bucket_B[i.second->gain].insert(make_pair(i.first,i.second));
        //bucket_B[i.second->gain].push_back(i.second);
        if(i.second->gain > max_gain_B)
        {
            max_gain_B = i.second->gain;
        }
    }
}

// int find_cell(map<int,list<Cell*>> bucket_A, map<int,list<Cell*>> bucket_B, int &area_A, int &area_B, int &area_n, int &max_gain_A, int &max_gain_B, Cell* &max_cell)
// {
//     int temp_i = 0;
//     int index = 0;
//     list<Cell*> candidate;
//     if(max_gain_A >= max_gain_B)
//     {
//         candidate = bucket_A[max_gain_A];
//         candidate.splice(candidate.end(),bucket_B[max_gain_B]);
//     }
//     else
//     {
//         candidate = bucket_B[max_gain_B];
//         candidate.splice(candidate.end(),bucket_A[max_gain_A]);
//     }
    
//     for (auto &i : candidate)
//     {
//         if (i->set == 'A')
//         {
//             if (constraint_check(area_A - (i->size), area_B + (i->size), area_n))
//             {
//                 index = temp_i;
//                 max_cell = i;
//                 return index;
//             }
//         }
//         else if (i->set == 'B')
//         {
//             if (constraint_check(area_A + (i->size), area_B - (i->size), area_n))
//             {
//                 index = temp_i;
//                 max_cell = i;
//                 return index;
//             }
//         }  
//     }
// }

// void update_Gain(map<int,list<Cell*>> &bucket_A, map<int,list<Cell*>> &bucket_B, unordered_map<string,Net*> &nets_hash, int &area_A, int &area_B, int &area_n, int &max_gain_A, int &max_gain_B, int &init_cut)
// {
//     int index = -1;
//     Cell* max_cell;
//     index = find_cell(bucket_A, bucket_B, area_A, area_B, area_n, max_gain_A, max_gain_B, max_cell);
//     // cout << index<<endl;
// }
// void update_Gain(map<int,list<Cell*>> &bucket_A, map<int,list<Cell*>> &bucket_B, unordered_map<string,Net*> &nets_hash, int &area_A, int &area_B, int &area_n, int &max_gain_A, int &max_gain_B, int &init_cut)
// {
//     int index_A = 0;
//     while(bucket_A[max_gain_A].empty() != 1 || bucket_B[max_gain_B].empty() != 1)
//     {
//         if (max_gain_A >= max_gain_B)
//         {
//             for (auto &i : bucket_A[max_gain_A])
//             {
//                 bool constraint = constraint_check(area_A - (i->size),area_B + (i->size), area_n);
//                 if (constraint != 1 && index_A != bucket_A[max_gain_A].size())
//                 {
//                     index_A += 1;
//                 }
//                 else if (constraint != 1 && index_A == bucket_A[max_gain_A].size())
//                 {
//                     while (bucket_A[max_gain_A].empty() == 1) //如果当前max gain已无元素，则更新max_gain
//                     {
//                         max_gain_A -= 1;
//                     }
//                     continue;
//                 }
//             }
//             bucket_A[max_gain_A][0]->set = 'B'; //将当前cell从a换到b
//             init_cut -= bucket_A[max_gain_A][0]->gain; //更新cut
//             for (auto &i : bucket_A[max_gain_A][0]->nets) //当前拿走的cell的所有net
//             {
//                 if(nets_hash[i]->B_n == 0)
//                 {
//                     for(auto &j : nets_hash[i]->cells)
//                     {
//                         j->gain += 1;
//                     }
//                 }
//                 else if(nets_hash[i]->B_n == 1)
//                 {
//                     for(auto &j : nets_hash[i]->cells)
//                     {
//                         if(j->set == 'B')
//                         {
//                             j->gain -= 1;
//                         }
//                     }
//                 }
//                 nets_hash[i]->B_n += 1;
//                 nets_hash[i]->A_n -= 1;
//                 if(nets_hash[i]->A_n == 0)
//                 {
//                     for(auto &j : nets_hash[i]->cells)
//                     {
//                         j->gain -= 1;
//                     }   
//                 }
//                 else if(nets_hash[i]->A_n == 1)
//                 {
//                     for(auto &j : nets_hash[i]->cells)
//                     {
//                         if(j->set == 'A')
//                         {
//                             j->gain += 1;
//                         }
//                     } 
//                 }
//             }
//             bucket_A[max_gain_A].pop_front(); //将当前cell拿出bucket
//             while (bucket_A[max_gain_A].empty() == 1) //如果当前max gain已无元素，则更新max_gain
//             {
//                 max_gain_A -= 1;
//             }
//         }
//         else 
//         {
            
//         }
//     }

// }

int main(int argc, char *argv[])
{
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
    map<int,unordered_map<string,Cell*>> bucket_A;
    map<int,unordered_map<string,Cell*>> bucket_B;
    int max_gain_A = 0;
    int max_gain_B = 0;
    create_bucket(set_A, set_B, bucket_A, bucket_B, max_gain_A, max_gain_B);
    //update_Gain(bucket_A, bucket_B, nets_hash, area_A, area_B, area_n, max_gain_A, max_gain_B, init_cut);
    //cout << "bucket_A: "<<endl;
    // for (auto& i : bucket_A)
    // {
    //     cout << "gain: " <<i.first << endl;
    //     for (auto& j : i.second)
    //     {
    //         cout << *(j.second);
    //     }
    // }
    cout << "bucket_B: "<<endl;
    for (auto& i : bucket_B)
    {
        cout << "gain: " <<i.first << endl;
        for (auto& j : i.second)
        {
            cout << *(j.second);
        }
    }
    // for(auto &i : nets){
    //     Net net = i;
    //     cout << i.name <<": ";
    //     for(auto &j : i.cells){
    //         cout<<(j->name)<<",";
    //     }
    //     cout << '\n';
    // }
    // for(auto &j : cells_key){
    //     cout<<"set_A: "<<set_A[j];
    // }
    //cout <<"end"<<endl;
    // for(auto &i : nets){
    //     cout << i;
    // }
}

