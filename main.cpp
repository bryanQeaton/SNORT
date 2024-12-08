#include <iostream>
#include <utility>
#include <vector>

/*
todo:
okay so the rules of snort are you play on a graph.
players take turns claiming nodes.
players can only claim nodes that do not have child nodes of the opponents

if no valid moves exist on your turn, you lose

this game presents no draws.
*/


struct Node {
    int claimed_by=0;
    std::vector<int> child_nodes={};
};
class Game {
    int turn=1;
    std::vector<int> history={};
public:
    std::vector<Node> graph={};
    Game() =default;
    explicit Game(std::vector<Node> graph):graph(std::move(graph)){};
    void make_move(const int &idx) {
        Node entry=graph[idx];
        entry.claimed_by=turn;
        graph[idx]=entry;
        history.push_back(idx);
        turn=-turn;
    }
    void undo_move() {
        turn=-turn;
        Node entry=graph[history.back()];
        entry.claimed_by=0;
        graph[history.back()]=entry;
        history.pop_back();
    }
    [[nodiscard]] std::vector<int> legal_moves() const {
        std::vector<int> moves;
        for (int i=0;i<graph.size();i++) {
            Node entry=graph[i];
            bool legal=(entry.claimed_by==0);
            for (const int &child_node : entry.child_nodes) {
                Node child_entry=graph[child_node];
                if (child_entry.claimed_by==-turn) {
                    legal=false;
                    break;
                }
            }
            if (legal){moves.push_back(i);}
        }
        return moves;
    }
};
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
int solve(Game &pos,int depth,int alpha=-1,int beta=1) {
    if (depth<=0){return 0;}
    std::vector<int> legal_moves=pos.legal_moves();
    if (legal_moves.empty()) {return -1;}
    int value=-1;
    for (int move:legal_moves) {
        pos.make_move(move);
        value=std::max(value,-solve(pos,depth-1,-beta,-alpha));
        pos.undo_move();
        if (value>alpha) {
            if (value>=beta){break;}
            alpha=value;
        }
    }
    return value;
}

int main() {
    auto game=Game(grid_gen(5,4));
    std::cout<<solve(game,100);


    return 0;
}
