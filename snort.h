#ifndef SNORT_H
#define SNORT_H
#include <vector>

//SNORT code
struct Node {
    int claimed_by=0;
    std::vector<int> child_nodes={};
};
class Game {
    std::vector<int> history={};
public:
    int turn=1;
    std::vector<Node> graph={};
    Game() =default;
    explicit Game(std::vector<Node> graph):graph(std::move(graph)){};
    [[nodiscard]] int half_move() const {return static_cast<int>(history.size());}
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
        for (int i=0;i<static_cast<int>(graph.size());i++) {
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

#endif //SNORT_H
