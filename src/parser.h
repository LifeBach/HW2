#ifndef PARSER_H
#define PARSER_H

/*****************************************************************************
*   structure
*****************************************************************************/
struct Cell
{
    string name = "";
    int size = 0;
    int gain = 0;
    char set = 'A';
    bool lock = 0;
    vector<string> nets;
};

struct Net
{
    string name = "";
    list<Cell*> cells;
    int A_n = 0;
    int B_n = 0;
};

/*****************************************************************************
*   operator overload: declaration
*****************************************************************************/
bool operator == (const Cell &cell_r, const string &cell_name);

bool operator < (const Cell &cell_r, const Cell &cell_l);

ostream  &operator<<(ostream &os, const Cell &cell);

ostream  &operator<<(ostream &os, const Net &net);

/*****************************************************************************
*   function: declaration
*****************************************************************************/
void cell_parser(char *filename, unordered_map<string,Cell> &cells, vector<Cell> &cells_vec);

vector<Net> net_parser(char *filename, unordered_map<string,Cell> &cells);

void create_net_array(vector<Net> &nets, unordered_map<string,Net*> &nets_hash);

vector<string> get_cell_hash_key(vector<Cell> &cells_vec);

#endif