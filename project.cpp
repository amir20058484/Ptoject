#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct actions
{
    int player_id;
    string Action;
    string piece ;
    int target_x;
    int target_y;
};
struct PieceInfo
{
    char type;
    int owner;
    int health;
    bool shildwall;
};

class Board
{
private:
    PieceInfo B[8][8];

public:
    Board();
    void change_board(int, int, char, int, int, bool);
    void print();
    void damage_piece(int, int, int);
    PieceInfo get_piece(int i, int j) { return B[i][j]; }
    void shild_cooldown(int);
};

void Board::shild_cooldown(int player_id)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (B[i][j].owner == player_id)
                B[i][j].shildwall = false;
        }
    }
}

void Board::change_board(int i, int j, char A, int owner, int health, bool shild)
{
    B[i][j].type = A;
    B[i][j].owner = owner;
    B[i][j].health = health;
    B[i][j].shildwall = shild;
}

void Board::damage_piece(int i, int j, int damage)
{
    if (B[i][j].type != '.' && B[i][j].health > 0)
    {
        if (B[i][j].shildwall)
        {
            damage /= 2;
        }
        B[i][j].health -= damage;

        if (B[i][j].health < 0)
        {
            B[i][j].health = 0;
        }

        if (B[i][j].health == 0)
        {
            B[i][j].type = '.';
        }
    }
}

Board::Board()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            B[i][j].shildwall = false;
            if (i == 0)
            {
                switch (j)
                {
                case 0:
                    B[i][j].type = 'T';
                    B[i][j].owner = 1;
                    B[i][j].health = 100;
                    break;
                case 2:
                    B[i][j].type = 'M';
                    B[i][j].owner = 1;
                    B[i][j].health = 100;
                    break;
                case 4:
                    B[i][j].type = 'D';
                    B[i][j].owner = 1;
                    B[i][j].health = 100;
                    break;
                case 6:
                    B[i][j].type = 'A';
                    B[i][j].owner = 1;
                    B[i][j].health = 100;
                    break;
                default:
                    B[i][j].type = '.';
                    B[i][j].owner = 0;
                    B[i][j].health = 0;
                    break;
                }
            }
            else if (i == 7)
            {
                switch (j)
                {
                case 0:
                    B[i][j].type = 't';
                    B[i][j].owner = 2;
                    B[i][j].health = 100;
                    break;
                case 2:
                    B[i][j].type = 'm';
                    B[i][j].owner = 2;
                    B[i][j].health = 100;
                    break;
                case 4:
                    B[i][j].type = 'd';
                    B[i][j].owner = 2;
                    B[i][j].health = 100;
                    break;
                case 6:
                    B[i][j].type = 'a';
                    B[i][j].owner = 2;
                    B[i][j].health = 100;
                    break;
                default:
                    B[i][j].type = '.';
                    B[i][j].owner = 0;
                    B[i][j].health = 0;
                    break;
                }
            }
            else
            {
                B[i][j].type = '.';
                B[i][j].owner = 0;
                B[i][j].health = 0;
            }
        }
    }
}

void Board::print()
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            cout << B[i][j].type << " ";
        }
        cout << endl;
    }
}

class piece
{
private:
    pair<int, int> location;
    int health;
    int player_id;

    // protected:
    //     bool shild_wall;

public:
    void set_location(int, int);
    int get_line();
    int get_col();
    void set_health(int);
    int get_health() const { return health; };
    void set_player_id(int id) { player_id = id; }
    int get_player_id() { return player_id; }
    virtual bool move(int, int, Board &) = 0;
};

void piece::set_health(int h)
{
    health = h;
}

int piece::get_line()
{
    return location.first;
}

int piece::get_col()
{
    return location.second;
}

void piece::set_location(int i, int j)
{
    location.first = i;
    location.second = j;
}

class master : public piece
{

public:
    bool move(int, int, Board &) override;
    master(int, int, int);
};

class archer : public piece
{

public:
    bool move(int, int, Board &) override;
    archer(int, int, int);
    bool power_strike(Board &board, int, int);
};

class defender : public piece
{
private:
    int shild_use;

public:
    bool move(int, int, Board &) override;
    defender(int, int, int);
    void set_shild_use(int use) { shild_use = use; }
    int get_shild_use() { return shild_use; }
    bool special_ability(Board &);
};

class tank : public piece
{

public:
    bool move(int, int, Board &) override;
    tank(int, int, int);
    bool precision_shot(Board &board, int, int);
};

master::master(int i, int j, int player)
{
    set_location(i, j);
    set_health(100);
    set_player_id(player);
}

archer::archer(int i, int j, int player)
{
    set_location(i, j);
    set_health(100);
    set_player_id(player);
}

defender::defender(int i, int j, int player)
{
    set_location(i, j);
    set_health(100);
    set_player_id(player);
    set_shild_use(0);
}

tank::tank(int i, int j, int player)
{
    set_location(i, j);
    set_health(100);
    set_player_id(player);
}

bool master::move(int i, int j, Board &board)
{
    int current_line = get_line();
    int current_col = get_col();
    PieceInfo tempp = board.get_piece(i, j);

    if (abs(i - current_line) <= 1 && abs(j - current_col) <= 1 && tempp.type == '.')
    {

        PieceInfo temp = board.get_piece(current_line, current_col);

        int current_health = temp.health;
        bool shild = temp.shildwall;

        board.change_board(current_line, current_col, '.', 0, 0, false);
        set_location(i, j);
        if (get_player_id() == 1)
        {
            board.change_board(i, j, 'M', get_player_id(), current_health, shild);
        }
        else
        {
            board.change_board(i, j, 'm', get_player_id(), current_health, shild);
        }
        return true;
    }
    return false;
}

bool archer::move(int i, int j, Board &board)
{
    int current_line = get_line();
    int current_col = get_col();
    PieceInfo tempp = board.get_piece(i, j);

    if (((abs(i - current_line) <= 1 && abs(j - current_col) <= 1) || (abs(i - current_line) == 2 && j == current_col) || (abs(j - current_col) == 2 && i == current_line) || (abs(i - current_line) == 2 && abs(j - current_col) == 2)) && tempp.type == '.')
    {
        if (abs(i - current_line) == 2 || abs(j - current_col) == 2)
        {
            int step_line = (i - current_line) / 2;
            int step_col = (j - current_col) / 2;
            int mid_line = current_line + step_line;
            int mid_col = current_col + step_col;

            PieceInfo mid_piece = board.get_piece(mid_line, mid_col);
            if (mid_piece.type != '.')
            {
                cout<<"There is an obstacle in the way!"<<endl;
                return false;
            }
        }
        PieceInfo temp = board.get_piece(current_line, current_col);
        int current_health = temp.health;
        bool shild = temp.shildwall;

        board.change_board(current_line, current_col, '.', 0, 0, false);
        set_location(i, j);
        if (get_player_id() == 1)
        {
            board.change_board(i, j, 'A', get_player_id(), current_health, shild);
        }
        else
        {
            board.change_board(i, j, 'a', get_player_id(), current_health, shild);
        }
        return true;
    }
    return false;
}

bool defender::move(int i, int j, Board &board)
{
    int current_line = get_line();
    int current_col = get_col();
    PieceInfo tempp = board.get_piece(i, j);

    if ((abs(i - current_line) <= 1 && abs(j - current_col) <= 1) && tempp.type == '.')
    {
        PieceInfo temp = board.get_piece(current_line, current_col);

        int current_health = temp.health;
        bool shild = temp.shildwall;
        board.change_board(current_line, current_col, '.', 0, 0, false);
        set_location(i, j);
        if (get_player_id() == 1)
        {
            board.change_board(i, j, 'D', get_player_id(), current_health, shild);
        }
        else
        {
            board.change_board(i, j, 'd', get_player_id(), current_health, shild);
        }
        return true;
    }
    return false;
}

bool tank::move(int i, int j, Board &board)
{
    int current_line = get_line();
    int current_col = get_col();
    PieceInfo tempp = board.get_piece(i, j);
    if (((current_col == j && abs(i - current_line) <= 2) || (current_line == i && abs(j - current_col) <= 2)) &&
        tempp.type == '.')
    {
        if (abs(i - current_line) == 2 || abs(j - current_col) == 2)
        {
            int step_line = (i - current_line) / 2;
            int step_col = (j - current_col) / 2;
            int mid_line = current_line + step_line;
            int mid_col = current_col + step_col;

            PieceInfo mid_piece = board.get_piece(mid_line, mid_col);
            if (mid_piece.type != '.')
            {
                cout<<"There is an obstacle in the way!"<<endl;
                return false;
            }
        }
        PieceInfo temp = board.get_piece(current_line, current_col);
        int current_health = temp.health;
        bool shild = temp.shildwall;

        board.change_board(current_line, current_col, '.', 0, 0, false);

        set_location(i, j);

        if (get_player_id() == 1)
        {
            board.change_board(i, j, 'T', get_player_id(), current_health, shild);
        }
        else
        {
            board.change_board(i, j, 't', get_player_id(), current_health, shild);
        }
        return true;
    }
    cout << "You cannot make this move" << endl;
    return false;
}

bool archer::power_strike(Board &board, int target_x, int target_y)
{
    int x = get_line();
    int y = get_col();
    if (abs(target_x - x) <= 1 && abs(target_y - y) <= 1)
    {
        PieceInfo target = board.get_piece(target_x, target_y);
        if (target.type != '.' && target.owner != get_player_id())
        {
            board.damage_piece(target_x, target_y, 50);
            cout << "Power strike successful on target at (" << target_x << ", " << target_y << ")." << endl;
            return true;
        }
    }
    cout << "Target out of range or not valid for power strike." << endl;
    return false;
}
bool tank::precision_shot(Board &board, int target_x, int target_y)
{
    int x = get_line();
    int y = get_col();
    if ((x == target_x && abs(y - target_y) <= 3) || (y == target_y && abs(x - target_x) <= 3))
    {
        PieceInfo target = board.get_piece(target_x, target_y);
        if (target.type != '.' && target.owner != get_player_id())
        {
            board.damage_piece(target_x, target_y, 40);
            cout << "Precision shot successful on target at (" << target_x << ", " << target_y << ")." << endl;

            return true;
        }
    }
    cout << "Target out of range or not valid for precision shot." << endl;
    return false;
}

bool defender::special_ability(Board &board)
{
    int x = get_line();
    int y = get_col();
    if (get_player_id() == 1)
    {
        board.change_board(x, y, 'D', get_player_id(), get_health(), true);
    }
    else
    {
        board.change_board(x, y, 'd', get_player_id(), get_health(), true);
    }
    
    // بررسی همه مهره‌های مجاور
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            int nx = x + dx;
            int ny = y + dy;

            // بررسی اینکه مختصات جدید داخل صفحه باشد و مهره متعلق به بازیکن باشد
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
            {
                PieceInfo neighbor = board.get_piece(nx, ny);

                // اگر مهره متعلق به همان بازیکن است و شیلد فعال نیست
                if (neighbor.owner == get_player_id() && neighbor.type != 'D' && neighbor.type != 'd' && !board.get_piece(nx, ny).shildwall)
                {
                    // فعال‌سازی شیلد برای مهره مجاور
                    board.change_board(nx, ny, neighbor.type, neighbor.owner, neighbor.health, true); // شیلد فعال می‌شود
                    cout << "shild is active now for next round!" << endl;
                }
            }
        }
    }

    return true;
}

void process_move_input(Board &board, vector<actions>& action_history , piece *&player_piece, int player_id,
                        archer &p1_archer, archer &p2_archer,
                        master &p1_master, master &p2_master,
                        defender &p1_defender, defender &p2_defender,
                        tank &p1_tank, tank &p2_tank)
{
    string input;
    int x, y;

    cout << "Please choose your piece (A, M, T, D): ";
    cin >> input;
    cout << "Enter target x and y: ";
    cin >> x >> y;
    action_history.push_back({player_id , "Move" , input , x , y});
    switch (toupper(input[0]))
    {
    case 'A':
        player_piece = (player_id == 1) ? &p1_archer : &p2_archer;
        break;
    case 'M':
        player_piece = (player_id == 1) ? &p1_master : &p2_master;
        break;
    case 'T':
        player_piece = (player_id == 1) ? &p1_tank : &p2_tank;
        break;
    case 'D':
        player_piece = (player_id == 1) ? &p1_defender : &p2_defender;
        break;
    default:
        cout << "Invalid piece selection!" << endl;
        return;
    }

    if (!player_piece->move(x, y, board))
    {
        cout << "Invalid move! Try again." << endl;
    }
    else
    {
        cout << "Move successful!" << endl;
    }
}

void process_attack_input(Board &board, vector<actions>& action_history ,int player_id, archer &p1_archer, archer &p2_archer,
                          tank &p1_tank, tank &p2_tank)
{
    string input, piece_type;
    int target_x, target_y;

    // 1️⃣ دریافت نوع مهره برای حمله
    cout << "Choose piece for attack (A: Archer, T: Tank): "<<endl;
    cin >> piece_type;
    cout << "Enter target x and y for attack: "<<endl;
    cin >> target_x >> target_y;
    tank *attacking_piece_tank = nullptr;
    archer *attacking_piece_archer = nullptr;
    action_history.push_back({player_id , "Attack" , piece_type , target_x , target_y});
    if (piece_type == "A")
    {
        attacking_piece_archer = (player_id == 1) ? &p1_archer : &p2_archer;
        if (attacking_piece_archer->power_strike(board, target_x, target_y))
        {
            cout << "attacking sucessfull" << endl;
        }
        else
        {
            cout << "attack was failed!" << endl;
        }
    }
    else if (piece_type == "T")
    {
        attacking_piece_tank = (player_id == 1) ? &p1_tank : &p2_tank;
        if (attacking_piece_tank->precision_shot(board, target_x, target_y))
        {
            cout << "attacking sucessfull" << endl;
        }
        else
        {
            cout << "attack was failed!" << endl;
        }
    }
    else
    {
        cout << "Invalid piece selection!" << endl;
        return;
    }
}

void process_turn(Board &board, int round,vector<actions>& action_history, archer &p1_archer, archer &p2_archer,
                  master &p1_master, master &p2_master, defender &p1_defender, defender &p2_defender,
                  tank &p1_tank, tank &p2_tank)
{
    int player_id = (round % 2 == 1) ? 1 : 2;
    board.shild_cooldown(player_id);
    cout << "Player " << player_id << "'s turn!" << endl;
    int index = round-1 ;


    string action;
    cout << "Enter command (move/attack/ability/exit): "<<endl;
    cin >> action;

    if (action == "exit")
        return;

    if (action == "move")
    {
        piece *selected_piece = nullptr;
        process_move_input(board,action_history , selected_piece, player_id, p1_archer, p2_archer, p1_master, p2_master, p1_defender, p2_defender, p1_tank, p2_tank);

    }
    else if (action == "attack")
    {
        process_attack_input(board, action_history ,player_id, p1_archer, p2_archer, p1_tank, p2_tank);
    }
    else if (action == "ability")
    {
        
        if (player_id == 1)
        {
            action_history.push_back({player_id , "ability" , "D" , p1_defender.get_line() , p1_defender.get_col()});
            if (p1_defender.get_health() <= 0)
            {
                cout << "You cannot use ability because your defender is dead!" << endl;
                return;
            }
            if (round - p2_defender.get_shild_use() >= 4 || p2_defender.get_shild_use() == 0)
            {
                p1_defender.set_shild_use(round);
                p1_defender.special_ability(board);
            }
            else
            {
                cout << "You should wait for recharge defender!"<<endl;
            }
        }
        else
        {
            action_history.push_back({player_id , "ability" , "D" , p2_defender.get_line() , p2_defender.get_col()});
            if (p2_defender.get_health() <= 0)
            {
                cout << "You cannot use ability because your defender is dead!" << endl;
                return;
            }
            if (round - p2_defender.get_shild_use() >= 4 || p2_defender.get_shild_use() == 0)
            {
                p2_defender.set_shild_use(round);
                p2_defender.special_ability(board);
            }
        }
    }
    else
    {
        cout << "Invalid action!" << endl;
    }
}

void print_his(vector<actions>& action_history){
    for(int i=0 ; i< action_history.size() ; i++){
        cout<<"Player "<<action_history[i].player_id<<": ";
        if(action_history[i].Action == "Move" || action_history[i].Action == "Attack"){
            cout<<action_history[i].Action<<" "<<action_history[i].piece<<" to ("<<action_history[i].target_x<<","<<action_history[i].target_y<<")"<<endl;
        }else{
            cout<<"Use ability on : "<<"("<<action_history[i].target_x<<","<<action_history[i].target_y<<")"<<endl;
        }
    }
}

int main()
{

    bool shild_player1, shild_player2;
    Board board;
    master player1_master(0, 2, 1);
    master player2_master(7, 2, 2);
    archer player1_archer(0, 6, 1);
    archer player2_archer(7, 6, 2);
    defender player1_defender(0, 4, 1);
    defender player2_defender(7, 4, 2);
    tank player1_tank(0, 0, 1);
    tank player2_tank(7, 0, 2);
    vector<actions> action_history;
    board.print();
    int Round = 1;
    while (cin)
    {
        if (Round == 60 || player1_master.get_health() <= 0 || player2_master.get_health() <= 0)
        {
            cout << "Game Over!" << endl;
            return 0;
        }
        process_turn(board, Round, action_history , player1_archer, player2_archer, player1_master, player2_master, player1_defender, player2_defender, player1_tank, player2_tank);
        Round++;
        board.print();
        // cout << "--------------------------" << endl;
        // for (int i = 0; i < 8; i++)
        // {
        //     for (int j = 0; j < 8; j++)
        //     {
        //         PieceInfo temp = board.get_piece(i, j);
        //         cout << "(" << temp.shildwall << "," << temp.health << ")" << " ";
        //     }
        //     cout << endl;
        // }
    }
    print_his(action_history);
    return 0;
}
