#include "headers.h"
#include "parser.h"

/*****************************************************************************
*   operator overload: implement
*****************************************************************************/

bool operator == (const Cell &cell_r, const string &cell_name)
{
    return cell_r.name == cell_name;
}

bool operator < (const Cell &cell_r, const Cell &cell_l)
{
    return cell_r.size < cell_l.size;
}

ostream  &operator<<(ostream &os, const Cell &cell)
{
    os << cell.name << ":" << cell.size << " nets: ";
    for(auto iter=cell.nets.begin(); iter!=cell.nets.end(); iter++)
    {
        os << (*iter) <<", ";
    }
    os <<"gain: "<<cell.gain<<" set: " <<cell.set<<" lock: "<<cell.lock<<endl;
    return os;
}

ostream  &operator<<(ostream &os, const Net &net)
{
    os << net.name << ":" << " cells: ";
    for(auto iter=net.cells.begin(); iter!=net.cells.end(); iter++)
    {
        os << *(*iter) <<", ";
    }
    os <<"A:B = "<< net.A_n <<":"<<net.B_n<<endl;
    return os;
}


/*****************************************************************************
*   Function: implement
*****************************************************************************/

/*****************************************************************************
*   function name: cell_parser
*   Description: Get and create cell from .cell to cell_Hashtable<string, Cell>
*****************************************************************************/
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

/*****************************************************************************
*   function name: net_parser
*   Description: Get and create net from .net to Vector<Net>
*****************************************************************************/
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

/*****************************************************************************
*   function name: create_net_array
*   Description: For every cell in cell_hashtable, create net array
*****************************************************************************/
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

/*****************************************************************************
*   function name: get_cell_hash_key
*   Description: Get key from cell hashtable
*****************************************************************************/
vector<string> get_cell_hash_key(vector<Cell> &cells_vec)
{
    vector<string> cells_key;
    for(auto &i : cells_vec){
        string cell_key = i.name;
        cells_key.push_back(cell_key);
    }
    return cells_key;
}