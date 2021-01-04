#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"
#include "../include/rules.h"
using namespace std;
int sum_eat;
#define ROW 5
#define COL 6

class Re_Board{
    public:
        Cell cells[ROW][COL];                       // The 5*6 board whose index (0,0) is start from the upper left corner
        void cell_reaction_marker();                // After the explosion, mark all the cell that  will explode in next iteration
        bool cell_is_full(Cell* cell);              // Check wether the cell is full of orbs and set the explosion variable to be true
        void add_orb(int i, int j, char color);     // Add orb into the cell (i, j)
        void cell_reset(int i, int j);              // Reset the cell to the initial state (color = 'w', orb_num = 0)
        void cell_explode(int i, int j);            // The explosion of cell (i, j). It will add the orb into neighbor cells
        void cell_chain_reaction(Player player);    // If there is aa explosion, check wether the chain reaction is active

    public:
        Re_Board();
        Re_Board(Board board);
        
        // The basic functions of the Board
        int get_orbs_num(int i, int j);
        int get_capacity(int i, int j);
        char get_cell_color(int i, int j);
        bool place_orb(int i, int j, Player* player);      // Use this function to place a orb into a cell
        bool win_the_game(Player player);                  // The function that is used to check wether the player wins the game after his/her placemnet operation
        bool opponent_place_orb(int i, int j, Player* player);//多新增對手的
        bool opponent_win_the_game(Player player);//多新增對手的
};
Re_Board::Re_Board(){
    ////// Initialize the borad with correct capacity //////
    // The corners of the board
    cells[0][0].set_capacity(3), cells[0][5].set_capacity(3), cells[4][0].set_capacity(3), cells[4][5].set_capacity(3);

    // The edges of the board
    cells[0][1].set_capacity(5), cells[0][2].set_capacity(5), cells[0][3].set_capacity(5), cells[0][4].set_capacity(5),
    cells[1][0].set_capacity(5), cells[2][0].set_capacity(5), cells[3][0].set_capacity(5),
    cells[1][5].set_capacity(5), cells[2][5].set_capacity(5), cells[3][5].set_capacity(5),
    cells[4][1].set_capacity(5), cells[4][2].set_capacity(5), cells[4][3].set_capacity(5), cells[4][4].set_capacity(5);
}

Re_Board::Re_Board(Board board) {
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            //cout<<board.get_cell_color(i, j)<<" 設顏色"<<endl;
            cells[i][j].set_color(board.get_cell_color(i, j));
            cells[i][j].set_capacity(board.get_capacity(i, j));
            cells[i][j].set_orbs_num(board.get_orbs_num(i, j));
        }
    }
}
bool Re_Board::place_orb(int i, int j, Player *player){
    if(!index_range_illegal(i, j) && !placement_illegal(*player, cells[i][j])){
        int temp = cells[i][j].get_orbs_num();
        temp += 1;
        cells[i][j].set_orbs_num(temp);
        cells[i][j].set_color(player->get_color());
    }
    else{
        player->set_illegal();
        return false;
    }
    if(cell_is_full(&cells[i][j])){
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(*player);
    }
    
    return true;
}
bool Re_Board::opponent_place_orb(int i, int j, Player *player){//仿place_orb
    char c = (player->get_color() == 'r') ? 'b' : 'r';//opponent color
    sum_eat=0;
    if(!(i > 4) && !(j > 5) && cells[i][j].get_color()!=player->get_color()){//界線內且非玩家色
        int temp = cells[i][j].get_orbs_num();
        temp += 1;
        cells[i][j].set_orbs_num(temp);
        cells[i][j].set_color(c);
    }
    else{
        player->set_illegal();
        return false;
    }
    if(cell_is_full(&cells[i][j])){
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(*player);
    }
    return true;
}

void Re_Board::cell_reaction_marker(){
    // Mark the next cell whose number of orbs is equal to the capacity
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            cell_is_full(&cells[i][j]);
        }
    }
}

bool Re_Board::cell_is_full(Cell* cell){ //true:cell滿了
    if(cell->get_orbs_num() >= cell->get_capacity()){
        cell->set_explode(true);
        return true;
    }
    else return false;
}

void Re_Board::add_orb(int i, int j, char color){ //更新cell(數字+1,顏色不變)
    int orb_num = cells[i][j].get_orbs_num();
    orb_num ++;
    cells[i][j].set_orbs_num(orb_num);
    cells[i][j].set_color(color);
}

void Re_Board::cell_reset(int i, int j){ //爆後顏色和數字歸零,explode回歸false
    cells[i][j].set_orbs_num(0);
    cells[i][j].set_explode(false);
    cells[i][j].set_color('w');
}

void Re_Board::cell_explode(int i, int j){
    char neighbor_color;
    char color = cells[i][j].get_color();

    cell_reset(i, j);
    if( i + 1 < ROW){ //非最下列 ->下面一個加1
        neighbor_color = cells[i+1][j].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i+1][j].get_orbs_num();
        }
        add_orb(i+1, j, color);
    }

    if( j + 1 < COL){ //非最右行
        neighbor_color = cells[i][j+1].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i][j+1].get_orbs_num();
        }
        add_orb(i, j+1, color);
    }

    if( i - 1 >= 0){ //非最上列
        neighbor_color = cells[i-1][j].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i-1][j].get_orbs_num();
        }
        add_orb(i-1, j, color);
    }

    if( j - 1 >= 0){ //非最左行
        neighbor_color = cells[i][j-1].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i][j-1].get_orbs_num();
        }
        add_orb(i, j-1, color);
    }
    if( i + 1 < ROW && j - 1 >= 0){
        neighbor_color = cells[i+1][j-1].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i+1][j-1].get_orbs_num();
        }
        add_orb(i+1, j-1, color);
    }
    if( i - 1 >= 0 && j - 1 >= 0){
        neighbor_color = cells[i-1][j-1].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i-1][j-1].get_orbs_num();
        }
        add_orb(i-1, j-1, color);
    }
    if( i + 1 < ROW && j + 1 < COL){
        neighbor_color = cells[i+1][j+1].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i+1][j+1].get_orbs_num();
        }
        add_orb(i+1, j+1, color);
    }
    if( i - 1 >= 0 && j + 1 < COL){
        neighbor_color = cells[i-1][j+1].get_color();
        if(neighbor_color != color){
            sum_eat += cells[i-1][j+1].get_orbs_num();
        }
        add_orb(i-1, j+1, color);
    }
}

void Re_Board::cell_chain_reaction(Player player){
    bool chain_reac = true;
    while(chain_reac){
        chain_reac = false;
        for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                if(cells[i][j].get_explode()){
                    cell_explode(i ,j);
                    chain_reac = true;
                }
            }
        }
        if(win_the_game(player)){
            return;
        }
        cell_reaction_marker();
    }
}

bool Re_Board::win_the_game(Player player){
    
    char player_color = player.get_color();
    bool win = true;

    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(cells[i][j].get_color() == player_color || cells[i][j].get_color() == 'w') continue;
            else{
                win = false;
                break;
            }
        }
        if(!win) break;
    }
    return win;
}
bool Re_Board::opponent_win_the_game(Player player){//仿win_the_game
    char player_color = player.get_color();
    bool win = true;

    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(cells[i][j].get_color() != player_color) continue;//改成非玩家色就繼續
            else{
                win = false;
                break;
            }
        }
        if(!win) break;
    }
    return win;
}

int Re_Board::get_orbs_num(int i, int j){
    return cells[i][j].get_orbs_num();
}

int Re_Board::get_capacity(int i, int j){
    return cells[i][j].get_capacity();
}

char Re_Board::get_cell_color(int i, int j){
    return cells[i][j].get_color();
}
/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions
 * 1. ~ 4. are listed in next block)
 *
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 *
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/
void me_try_each_cell(Re_Board,Re_Board,Player);
void opponent_try_each_cell(Re_Board,Re_Board,Player,int,int);
bool check_lose();
void check_capacity(Re_Board,int,int,int&,int&);
int tree[5][6][2];
int row, col;
void algorithm_A(Board board, Player player, int index[]){
    row=0;
    col=0;
    Re_Board re_board(board);
    Re_Board re_board_orign(board);//存原始board方便place_orb
    me_try_each_cell(re_board,re_board_orign, player);//測試每個cell
    int max=-1000000000;
    bool lose=true;//判斷是不是輸
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            if(tree[i][j][0] == 1){//贏
                index[0] = i;
                index[1] = j;
                lose=false;
                return;
            }
            else if(tree[i][j][0] == 0){//打平
                if(tree[i][j][1]>max){//吃比max更多的orb
                    max=tree[i][j][1];
                    row = i;
                    col = j;
                }
                else if (tree[i][j][1]==max){ //吃跟max一樣多的orb
                     check_capacity(re_board,i,j,row,col);//測哪個離臨界點更近
                }
                index[0] = row;
                index[1] = col;
                lose=false;
            }
        }
    }
    if(lose){
        index[0] = row;
        index[1] = col;
    }
};

void me_try_each_cell(Re_Board re_board,Re_Board re_board_orign,Player player){
    //測試每個cell
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            sum_eat=0;
            if(re_board.place_orb(i,j,&player)==true){//可以放置orb
                if(re_board.win_the_game(player)){//贏就直接回傳
                    tree[i][j][0]=1;//下！！！
                    return;
                }
                else if(re_board.opponent_win_the_game(player)){//對手贏
                    tree[i][j][0]=-1;//不能下！！！
                }
                else{
                    tree[i][j][0]=0;//平手
                    tree[i][j][1]=sum_eat;//計算效益數為可以吃掉的棋子數量
                    opponent_try_each_cell(re_board,re_board, player,i,j);//換對手下
                }
                re_board=re_board_orign;//還原板子成下棋以前的樣子
            }
            else{
                tree[i][j][0]=-1;//不能下
            }
        }
    }
}
void opponent_try_each_cell(Re_Board re_board,Re_Board re_board_orign,Player player,int last_step_x,int last_step_y){
    for(int i = 0; i < ROW; i++) {
         for(int j = 0; j < COL; j++) {
             sum_eat=0;
             if(re_board.opponent_place_orb(i,j,&player)==true){//對方可以放置orb
                 if(re_board.win_the_game(player)){//自己會贏
                     tree[last_step_x][last_step_y][1]+=20000;
                 }
                 else if(re_board.opponent_win_the_game(player)){//對方下這就贏了
                     tree[last_step_x][last_step_y][1]-=999999999;
                 }
                 else{
                     tree[last_step_x][last_step_y][1]-=sum_eat;//這裡的sum_eat會變成對方可吃掉自己的棋數
                 }
                 re_board=re_board_orign;//還原板子成下棋以前的樣子
             }
         }
    }
}
void check_capacity(Re_Board board,int i, int j,int &row,int &col){//容量-當格的棋子數量
    if((board.get_capacity(i,j)-board.get_orbs_num(i,j))<(board.get_capacity(row,col)-board.get_orbs_num(row,col))){
        row = i;
        col = j;
    }
}
/*void me_last_try_each_cell(Re_Board re_board,Re_Board re_board_orign,Player player){
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            if(re_board.place_orb(i,j,&player)==true){
                if(re_board.win_the_game(player)){
                    tree[i][j][2]=1;
                }
                else if(re_board.opponent_win_the_game(player)){
                    tree[i][j][2]=-1;
                }
                else{
                    tree[i][j][2]=0;
                }
                re_board=re_board_orign;
            }
        }
    }
}*/

