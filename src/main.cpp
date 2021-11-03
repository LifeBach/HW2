#include "headers.h"
#include "parser.h"
#include "fm_core.h"

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
    //cout <<"best cut: "<< best_cut << " time: "<< (double) (end-start)/CLOCKS_PER_SEC<<endl;
}