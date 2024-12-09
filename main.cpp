#include <iostream>
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
    sum^=hash*outcome
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
int no_unclaimed_rule_naive(const std::vector<Node> &graph) {
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
                bool white_=true;
                bool black_=true;
                if (graph[idx].claimed_by==0) {
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
    return white_count-black_count;
}
int no_unclaimed_rule(const std::vector<Node> &graph,int turn) {
    //return 1 for p1 win, -1 for p2 win, 0 for ongoing game
    //counts the number of empty nodes that have children claimed by white
    int white_count=0;
    //counts the number of empty nodes that have children claimed by black
    int black_count=0;
    //counts the number of empty nodes that have children claimed by
    //exclusively either side that are connected to nodes that have
    //children claimed by the opposite side
    int claimed_connected_count=0;
    //iterate over nodes in the graph
    for (const Node &node:graph) {
        //if a node is claimed by nobody then we are interest in it
        if (node.claimed_by==0) {
            //does the node have a white child
            bool white=true;
            //does the node have a black child
            bool black=true;
            //is the child node unclaimed
            bool unclaimed=true;
            //is the child of the child node is black
            bool black_connected=false;
            //iterate over child nodes
            for (const int &idx:node.child_nodes) {
                //if the child is unclaimed set flag
                if (graph[idx].claimed_by!=0){unclaimed=false;}
                //if the child is white set flag
                if (graph[idx].claimed_by==1){black=false;}
                //if the child is black set flag
                else if (graph[idx].claimed_by==-1){white=false;}
                //is the child of the child node white
                bool white_=true;
                //is the child of the child node black
                bool black_=true;
                //if the child node is unclaimed
                if (graph[idx].claimed_by==0) {
                    //iterate over children of the child node
                    for (const int &idx_:graph[idx].child_nodes) {
                        //if the child of the child is white set flag
                        if (graph[idx_].claimed_by==1){black_=false;}
                        //if the child of the child is black set flag
                        else if (graph[idx_].claimed_by==-1){white_=false;}
                    }
                    //if the child of the child node is black set flag
                    if (black_){black_connected=true;}
                }
            }
            //if a node is unclaimed return
            if (unclaimed){return 0;}
            //if a child node is white, incrememnt white count
            if (white){white_count++;}
            //if a child node is black, increment black count
            else if (black){black_count++;}
            //if a child node is white and one of its children is black increment
            //the number of empty nodes that have children claimed by
            //exclusively either side that are connected to nodes that have
            //children claimed by the opposite side
            if (white&&black_connected){claimed_connected_count++;}
        }
    }
    //if a single claimed_connect exists, the score is flipped,
    //if two exist, the score is the same. this can be done with a modulo operation
    //final score is multiplied by turn because it makes intuitive sense, prove it wrong and ill remove it
    if (claimed_connected_count%2!=0){return -(white_count-black_count)*turn;} //im not sure if *turn is needed
    //return final score given no fuckery
    return white_count-black_count;
}


//Pure solver
int solve(Game &pos,int m,int alpha=-1, const int &beta=1) {
    if (one_unclaimed_rule(pos.graph)){return 1;}
    if (pos.legal_moves().empty()) {return -1;}
    int rule=std::max(std::min(no_unclaimed_rule(pos.graph,pos.turn),1),-1);
    if (rule!=0) {
        return pos.turn*rule;
    }
    std::vector<int> legal_moves=pos.legal_moves();
    if (legal_moves.empty()) {return -1;}
    int value=-1;
    for (const int &move:legal_moves) {
        pos.make_move(move);
        value=std::max(value,-solve(pos,m,-beta,-alpha));
        pos.undo_move();
        if (value>alpha) {
            if (value>=beta){break;}
            alpha=value;
        }
    }

    //debug
    if (value!=rule*pos.turn&&rule!=0) {
        std::cout<<"true value:"<<value<<" predicted:"<<pos.turn*rule<<"\n";
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
        if (solve(pos,point[1])==point[2]){std::cout<<"test complete!\n";}
        else{std::cout<<"test failed!\n";}
    }
    auto t1=std::chrono::high_resolution_clock::now();
    int count=std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    std::cout<<"time:"<<count<<"\n";


    return 0;
}
