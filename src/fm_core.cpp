#include "headers.h"
#include "parser.h"
#include "fm_core.h"

/*****************************************************************************
*   operator overload: implement
*****************************************************************************/
ostream  &operator<<(ostream &os, const Solution &solution)
{
    os << "cell:" << *(solution.p_cell) << "cut: " << solution.n_cut<<endl;
    return os;
}

/*****************************************************************************
*   Function: implement
*****************************************************************************/

/*****************************************************************************
*   function name: constraint_check
*   Description: check whether area is balance
*****************************************************************************/
inline bool constraint_check(int area_A, int area_B, int area_n) { return abs(area_A - area_B) < (area_n/10); }

/*****************************************************************************
*   function name: init_solution
*   Description: find an initial solution
*****************************************************************************/
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

/*****************************************************************************
*   function name: init_gain
*   Description: After get init solution, calculate init gain
*****************************************************************************/
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

/*****************************************************************************
*   function name: create_bucket
*   Description: Create bucket_list
*****************************************************************************/
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

/*****************************************************************************
*   function name: get_max
*   Description: Get_max gain in bucket list
*****************************************************************************/
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

/*****************************************************************************
*   function name: find_cell
*   Description: Find a cell of max gain to swap
*****************************************************************************/
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

/*****************************************************************************
*   function name: update_gain
*   Description: When finished swap, update gain of reletive cells
*****************************************************************************/
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

/*****************************************************************************
*   function name: best_result
*   Description: Trace back to recover the best result
*****************************************************************************/
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

/*****************************************************************************
*   function name: FM
*   Description: FM core algorithm
*****************************************************************************/
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

/*****************************************************************************
*   function name: result_output
*   Description: Output the final result
*****************************************************************************/
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