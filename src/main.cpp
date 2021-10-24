#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <deque>
#include <set>
using namespace std;

struct Cell
{
    string name = "";
    int size = 0;
    int gain = 0;
    bool operator == (const string &cell_name) const
    {
        return this->name == cell_name;
    }
    bool operator < (const Cell &cell) const
    {
        return this->size < cell.size;
    }
    friend  ostream  &operator<<(ostream &os, const Cell &cell){  //声明为友元，重载输出运算符
        os << cell.name << ":" << cell.size;
        return os;
    }
};

struct Net
{
    string name = "";
    vector<Cell*> cells;
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

inline bool constraint_check(int area_A, int area_B, int area_n) { return abs(area_A - area_B) < (area_n/10); }

void init_solution(vector<Cell> cells_vec, unordered_map<string,Cell> &set_A, unordered_map<string,Cell> &set_B, int &n, int &area_A, int &area_B)
{
    for(auto iter=cells_vec.begin(); iter!=cells_vec.end();iter++)
    {
        n += (iter->size);
    }
    area_A = n;
    int i=0;
    string cells_key;
    Cell cell;
    while(constraint_check(area_A,area_B,n) != true)
    {
        cells_key = cells_vec[i].name;
        cell = set_A[cells_key];
        set_B.insert(make_pair(cells_key,cell));
        area_B += cell.size;
        set_A.erase(cells_key);
        area_A -= cell.size;
        i++;
    }
}

int main(int argc, char *argv[])
{
    char *cell_filename = argv[2];
    unordered_map<string,Cell> cells_hash;
    vector<Cell> cells_vec;
    cell_parser(cell_filename,cells_hash,cells_vec);
    sort(cells_vec.begin(),cells_vec.end());
    char *net_filename = argv[1];
    vector<Net> nets;
    nets=net_parser(net_filename,cells_hash);
    unordered_map<string,Cell> set_A = cells_hash;
    unordered_map<string,Cell> set_B;
    int area_n = 0;
    int area_A = 0;
    int area_B = 0;
    init_solution(cells_vec, set_A, set_B, area_n, area_A, area_B);
    cout << area_n << area_A << area_B;
    // for(auto &i : nets){
    //     Net net = i;
    //     cout << i.name <<": ";
    //     for(auto &j : i.cells){
    //         cout<<(j->name)<<",";
    //     }
    //     cout << '\n';
    // }

    // for(auto &i : cells_vec){
    //     Cell cell = i;
    //     cout << i.name<<":"<<i.size ;
    //     cout << '\n';
    // }
}

