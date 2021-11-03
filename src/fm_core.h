#ifndef FM_CORE_H
#define FM_CORE_H

/*****************************************************************************
*   structure
*****************************************************************************/
struct Solution
{
    Cell* p_cell;
    int n_cut;
};

/*****************************************************************************
*   operator overload: declaration
*****************************************************************************/
ostream  &operator<<(ostream &os, const Solution &solution);

/*****************************************************************************
*   function: declaration
*****************************************************************************/
void init_solution(vector<string> cells_key, unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, int &n, int &area_A, int &area_B);

void init_gain(unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, vector<Net> &nets, int &init_cut);

void create_bucket(unordered_map<string,Cell> &cells_hash, map<int,unordered_map<string,Cell*>,greater<int>> &bucket);

int get_max(map<int,unordered_map<string, Cell*>,greater<int>> &bucket);

Cell* find_cell(map<int,unordered_map<string, Cell*>,greater<int>> &bucket, int &area_A, int &area_B, int &area_n);

void update_gain(map<int,unordered_map<string,Cell*>,greater<int>> &bucket, unordered_map<string,Net*> &nets_hash, Cell* &
max_cell);

void best_result(unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B, vector<Solution> &solution, int init_cut, int best_step);

int FM(map<int,unordered_map<string,Cell*>,greater<int>> &bucket, unordered_map<string,Net*> &nets_hash, int &area_A, int &area_B, int &area_n, int init_cut, vector<Solution> &solution, int iter, unordered_map<string,Cell*> &set_A, unordered_map<string,Cell*> &set_B);

void result_output(char *filename, int best_cut, unordered_map<string,Cell*> set_A, unordered_map<string,Cell*> set_B);

#endif