SNORT *------------------

  named after its creator S. Norton, is a game based on the story of two
farmers, who rent fields in a pasture for their herds. Mr. Black raises bulls and Mr. White
raises cows. Mr. Black claims a field first and Mr. White claims second, and they take
turns claiming the remaining fields. Because of the nature of bulls and cows, they cannot
put the two herds in the same field or in fields next to each other. Thus if the field
adjacent to a field claimed by Mr. Black is empty, that field will be reserved for only Mr.
Black to rent. The same is true for the field adjacent to a field claimed by Mr. White.
If a field is next to Mr. Black’s and Mr. White’s at the same time, then neither farmer
can claim that field. The farmer who is the last one able to claim an open space wins
the Snort game. Since a player can gain one field in each move, if Mr. Black wins, he has
more fields than Mr. White. If Mr. White wins, then he has at least as many fields as
Mr. Black. Again we should note that in Snort the winner is the player who has the last
move, not the player with more fields claimed.


```
int main() {
    auto game=Game(grid_gen(5,4));
    auto moves=game.legal_moves(); //generate legal moves
    game.make_move(moves[0]); //make moves
    game.undo_move(); //undo moves
    if (moves.empty()){is_gameover;} //move list is empty meaning game is in terminal state
    //moves are in the form of an integer, they index a vector of nodes. if move==0 move is acting on the 0th node.

    return 0;
}
```
