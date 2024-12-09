#include <iostream>
#include <chrono>

#include "snort.h"

/*
todo:
if there are no cells with unclaimed children, the outcome is determinable


if there is 1 cell with unclaimed children and all other empty cells have mixed claimed children, side to move wins
*/




//Graph tools
std::vector<Node> grid_gen(const int &n,const int &m) {
    std::vector<Node> graph={};
    constexpr int transforms[4][2]={
        {-1,0},
        {0,-1},
        {0,1},
        {1,0},
    };
    for (int i=0;i<m*n;i++){graph.push_back(Node(0,{}));}
    for (int y=0;y<n;y++) {
        for (int x=0;x<m;x++) {
            for (const int* transform:transforms) {
                if (0<=y+transform[0]&&y+transform[0]<n&&0<=x+transform[1]&&x+transform[1]<m) {
                    const int idx=y*m+x;
                    int idx_t=(y+transform[0])*m+(x+transform[1]);
                    graph[idx].child_nodes.push_back(idx_t);
                }
            }
        }
    }
    return graph;
}
bool one_unclaimed_rule(const std::vector<Node> &graph) { //if this returns true you can return a win for the sidetomove (return 1;)
    int count_unclaimed=0;
    for (const Node &node:graph) {
        bool unclaimed=true;
        bool plus_found=false;
        bool minus_found=false;
        for (const int &idx:node.child_nodes) {
            if (graph[idx].claimed_by!=0){unclaimed=false;}
            if (graph[idx].claimed_by==1){plus_found=true;}
            else if (graph[idx].claimed_by==-1){minus_found=true;}
        }
        if (!unclaimed&&(!plus_found||!minus_found)){return false;}
        if (unclaimed) {
            count_unclaimed+=1;
            if (count_unclaimed>1){return false;}
        }
    }
    return true;
}


//Pure solver
int solve(Game &pos,int alpha=-1, const int &beta=1) {
    if (one_unclaimed_rule(pos.graph)){return 1;}
    std::vector<int> legal_moves=pos.legal_moves();
    if (legal_moves.empty()) {return -1;}
    int value=-1;
    for (const int &move:legal_moves) {
        pos.make_move(move);
        value=std::max(value,-solve(pos,-beta,-alpha));
        pos.undo_move();
        if (value>alpha) {
            if (value>=beta){break;}
            alpha=value;
        }
    }
    return value;
}

void gen_dataset(const int &n) {
    for (int i=1;i<=n;i++) {
        for (int j=1;j<=n;j++) {
            auto pos=Game(grid_gen(i,j));
            std::cout<<"{"<<i<<","<<j<<","<<solve(pos)<<"},\n";
        }
    }
}

int main() {
    int data[16][3]={
        {1,1,1},
        {1,2,1},
        {1,3,1},
        {1,4,1},
        {2,1,1},
        {2,2,-1},
        {2,3,1},
        {2,4,-1},
        {3,1,1},
        {3,2,1},
        {3,3,1},
        {3,4,1},
        {4,1,1},
        {4,2,-1},
        {4,3,1},
        {4,4,-1}
    };
    auto t0=std::chrono::high_resolution_clock::now();
    for (int* point:data) {
        auto pos=Game(grid_gen(point[0],point[1]));
        if (solve(pos)==point[2]){std::cout<<"test complete!\n";}
        else{std::cout<<"test failed!\n";}
    }
    auto t1=std::chrono::high_resolution_clock::now();
    int count=std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    std::cout<<"time:"<<count<<"\n";


    return 0;
}
