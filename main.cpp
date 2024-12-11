#include <iostream>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <chrono>

#include "snort.h"

/*
todo:
if there are no cells with unclaimed children, the outcome is determinable

transposition table can be added by initiating the graph structure with a set of random numbers,
one for each node, this will be negated for unclaimed nodes, the equality BETWEEN graphs is not in question
each node has 1 random hashing number.

getting the hash of the graph is done by:
for each node:
    sum^=hash*claimed_by
    for each child:
        sum^=hash
*/

//Graph tools
void ascii(const std::vector<Node> &graph,int n,int m) {
    char chars[3]={'X','_','O'};
    for (int i=0;i<graph.size();i++) {
        if (i%m==0){std::cout<<"\n";}
        std::cout<<chars[graph[i].claimed_by+1]<<" ";
    }
}
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
bool one_unclaimed_rule(const std::vector<Node> &graph) {
    //if there is 1 cell with unclaimed children and all other empty cells have mixed claimed children, side to move wins
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
int no_unclaimed_rule_naive(const std::vector<Node> &graph,int turn) {
    //return 1 for p1 win, -1 for p2 win, 0 for ongoing game
    int white_count=0;
    int black_count=0;
    for (const Node &node:graph) {
        if (node.claimed_by==0) {
            bool white=true;
            bool black=true;
            bool unclaimed=true;
            bool black_connected=false;
            bool white_connected=false;
            for (const int &idx:node.child_nodes) {
                if (graph[idx].claimed_by!=0){unclaimed=false;}
                if (graph[idx].claimed_by==1){black=false;}
                else if (graph[idx].claimed_by==-1){white=false;}
                if (graph[idx].claimed_by==0) {
                    bool black_=true;
                    bool white_=true;
                    for (const int &idx_:graph[idx].child_nodes) {
                        if (graph[idx_].claimed_by==1){black_=false;}
                        else if (graph[idx_].claimed_by==-1){white_=false;}
                    }
                    if (white_){white_connected=true;}
                    else if (black_){black_connected=true;}
                }

            }
            if (unclaimed){return 0;}
            if (white){white_count++;}
            else if (black){black_count++;}
            if (white&&black_connected){return 0;}
            if (black&&white_connected){return 0;}
        }
    }
    return std::max(std::min((white_count-black_count)*turn,1),-1);
}

//Heuristic solver
constexpr double heuristic_coefficients[4]={1,.9,-1.4,1.5};
int heuristicsolve(Game &pos) {
    std::vector<int> legal_moves=pos.legal_moves();
    std::vector<double> scores;
    for (int &node:legal_moves) {
        double score=0.0;
        for (int &idx:pos.graph[node].child_nodes) {
            if (pos.graph[idx].claimed_by==0){score+=heuristic_coefficients[0];}
            for (int &idx_:pos.graph[idx].child_nodes) {
                if (pos.graph[idx_].claimed_by==0){score+=heuristic_coefficients[1];}
                else if (pos.graph[idx_].claimed_by==pos.turn){score+=heuristic_coefficients[2];}
                else if (pos.graph[idx_].claimed_by==-pos.turn){score+=heuristic_coefficients[3];}
            }
        }
        scores.push_back(score);
    }
    ascii(pos.graph,5,4);
    std::cout<<"\n";
    int c=0;
    for (int i=0;i<pos.graph.size();i++) {
        if (i%4==0){std::cout<<"\n";}
        if (legal_moves[c]==i&&c<legal_moves.size()) {
            if (scores[c]>=0){std::cout<<" ";}
            std::cout<<scores[c]<<" ";
            c++;
        }
        else{std::cout<<" 0 ";}
    }
    std::cout<<"\n";
    int idx=static_cast<int>(std::distance(scores.begin(),std::ranges::max_element(scores)));
    return legal_moves[idx];
}

//Pure solver
double error_count=0;
double beta_count=0;
int solve(Game &pos,int m,int alpha=-1, const int &beta=1) {
    //if (one_unclaimed_rule(pos.graph)){return 1;} //this save us one recursion step in a very small set of positions, its currently detrimental to the speed of the program.
    //if (pos.legal_moves().empty()) {return -1;} //passes all tests without this, I think this was a bandaid fix for a bug.
    const int rule=no_unclaimed_rule_naive(pos.graph,pos.turn);
    if (rule!=0) {return rule;}
    std::vector<int> legal_moves=pos.legal_moves();
    if (legal_moves.empty()) {return -1;}
    //move ordering
    std::vector<double> scores;
    for (int &node:legal_moves) {
        double score=0.0;
        for (int &idx:pos.graph[node].child_nodes) {
            if (pos.graph[idx].claimed_by==0){score+=heuristic_coefficients[0];}
            for (int &idx_:pos.graph[idx].child_nodes) {
                if (pos.graph[idx_].claimed_by==0){score+=heuristic_coefficients[1];}
                else if (pos.graph[idx_].claimed_by==pos.turn){score+=heuristic_coefficients[2];}
                else if (pos.graph[idx_].claimed_by==-pos.turn){score+=heuristic_coefficients[3];}
            }
        }
        scores.push_back(score);
    }
    std::vector<int> indices(legal_moves.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::ranges::sort(indices, [&](int i, int j) {return scores[i]>scores[j];});
    std::vector<int> sorted_moves(legal_moves.size());
    for (size_t i = 0; i < indices.size(); ++i) {sorted_moves[i]=legal_moves[indices[i]];}
    legal_moves=std::move(sorted_moves);
    int value=-1;
    for (int m=0;m<legal_moves.size();m++) {
        pos.make_move(legal_moves[m]);
        value=std::max(value,-solve(pos,m,-beta,-alpha));
        pos.undo_move();
        if (value>alpha) {
            if (value>=beta) {
                if (m>0){error_count++;}
                beta_count++;
                break;
            }
            alpha=value;
        }
    }

    //debug
    if (value!=rule&&rule!=0) {
        std::cout<<"true value:"<<value<<" predicted:"<<rule<<"\n";
        ascii(pos.graph,1,m);
        std::cout<<"\n";
    }

    return value;
}

//Misc
void gen_dataset(const int &n) {
    for (int i=1;i<=n;i++) {
        for (int j=1;j<=n;j++) {
            auto pos=Game(grid_gen(i,j));
            std::cout<<"{"<<i<<","<<j<<","<<solve(pos,n)<<"},\n";
        }
    }
}
void test() {
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
        if (solve(pos,point[1])==point[2]){std::cout<<"test complete!\n";}
        else{std::cout<<"test failed!\n";}
    }
    auto t1=std::chrono::high_resolution_clock::now();
    int count=std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    std::cout<<"time:"<<count<<"\n";
}
void move_values(Game &pos,int n,int m) {
    ascii(pos.graph,n,m);
    std::cout<<"\n";
    std::vector<int> legal_moves=pos.legal_moves();
    std::vector<int> scores={};
    for (int move:legal_moves) {
        pos.make_move(move);
        scores.push_back(-solve(pos,m));
        pos.undo_move();
    }
    int c=0;
    for (int i=0;i<pos.graph.size();i++) {
        if (i%m==0){std::cout<<"\n";}
        if (legal_moves[c]==i&&c<legal_moves.size()) {
            if (scores[c]>=0){std::cout<<" ";}
            std::cout<<scores[c]<<" ";
            c++;
        }
        else{std::cout<<" 0 ";}
    }
    std::cout<<"\n\n";

}
int main() {
    int n=3;int m=3;
    auto pos=Game(grid_gen(n,m));
    move_values(pos,n,m);
    std::cout<<error_count<<" "<<beta_count<<" "<<error_count/(beta_count+1);


    return 0;
}
