#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>



int KNIGHT[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
          {0, 0, 0, 0, 0, 0, 0, 0},
          {0, 2, 2, 2, 2, 2, 2, 0},
          {0, 2, 2, 2, 2, 2, 2, 0},
          {0, 2, 2, 2, 2, 2, 2, 0},
          {0, 2, 2, 2, 2, 2, 2, 0},
          {0, 0, 0, 0, 0, 0, 0, 0},
          {0, 0, 0, 0, 0, 0, 0, 0}};

char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
int last_value = 0;
int w_king_x = 0;
int w_king_y = 0;
int b_king_x = 0;
int b_king_y = 0;
int board[8][8];

int KING_X[8] = {0, 1, 0, -1, 1, -1, 1, -1};
int KING_Y[8] = {1, 0, -1, 0, 1, -1, -1, 1};

int QUEEN_X[8] = {0, 1, 0, -1, 1, -1, 1, -1};
int QUEEN_Y[8] = {1, 0, -1, 0, 1, -1, -1, 1};

int ROOK_X[4] = {0, 1, 0, -1};
int ROOK_Y[4] = {1, 0, -1, 0};

int BISH_X[4] = {1, -1, 1, -1};
int BISH_Y[4] = {1, -1, -1, 1};

int KNIGHT_X[8] = {2, 1, -1, -2, -2, -1, 1, 2};
int KNIGHT_Y[8] = {1, 2, 2, 1, -1, -2, -2, -1};

int PAWN_X[4] = {0, 0, 1, -1};
int PAWN_Y[4] = {1, 2, 1, 1};

typedef struct move_entry{
    char from[3];
    char to[3];
    char piece[10];
    int capture;
    struct move_entry* next;
} move_t;

typedef struct move_score{
    struct move_entry* move;
    int score;
} score_t;

//move_t* head = NULL;

/* convert e4 -> 4, 4 */
void letter_to_index(char* s, int* x, int* y){
    *x = (int)s[0] -97;
    *y = 8 - atoi(&s[1]);
}

/* The opposite of letter_to_index */
void index_to_letter(int x, int y, char s[3]){
    char a = (char) (x + 97);
    char b = (char) (abs(y-8) + 48);

    s[0] = a;
    s[1] = b;
    s[2] = '\0';
}

/* Checks if turn matches with piece */
int correct_color(int y, int x, int turn){
    return ((turn * board[y][x]) > 0);
}

/* Checks if indexes are outside board */
int out_of_bounds(int y, int x){
    //printf("Checking out of bounds for %d, %d\n", x, y);
    if (x < 0 || x > 7){
        return 1;
    }
    if (y < 0 || y > 7){
        return 1;
    }
    return -1;
}


/* Pushes a new move on the board */
void move(move_t* m){
    int x0, y0, x1, y1;
    
    letter_to_index(m->from, &x0, &y0);
    letter_to_index(m->to, &x1, &y1);
    //printf("moving from X:%d, y: %d - to x: %d, y: %d\n", x0, y0, x1, y1);
    m->capture = board[y1][x1];
    board[y1][x1] = board[y0][x0];
    board[y0][x0] = 0;
    if (board[y1][x1] == 100){
        w_king_x = x1;
        w_king_x = y1;
    }
    else if (board[y1][x1] == -100){
        b_king_x = x1;
        b_king_y = y1;
    }

}

/* pops the last move off the board */
void pop(move_t* m){
    int x0, y0, x1, y1;
    letter_to_index(m->to, &x0, &y0);
    letter_to_index(m->from, &x1, &y1);

    board[y1][x1] = board[y0][x0];
    board[y0][x0] = m->capture;
    if (board[y1][x1] == 100){
        w_king_x = x1;
        w_king_x = y1;
    }
    else if (board[y1][x1] == -100){
        b_king_x = x1;
        b_king_y = y1;
    }
}

int get_board_score(){
    int sum = 0;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (abs(board[i][j]) == 100){
                int hax = 0;
                hax = board[i][j];
                sum += hax;
            }
            else if (board[i][j] != 0){
                int hax = 0;
                if (board[i][j] > 0){
                    hax = (board[i][j]*100) + (KNIGHT[i][j]);
                    //printf("hax white: %d\n", hax);
                }
                else{
                    hax = (board[i][j]*100) - (KNIGHT[i][j]);
                    //printf("hax black: %d\n", hax);
                }
                sum += hax;
            }
        }
    }
    return sum;
}

int is_check(int x, int y, int turn){
    //printf("checking if check on x: %d, y: %d\n", x, y);
    int current_x, current_y, current_val;
    int dst = 7;
    for (int index = 0; index < sizeof(QUEEN_Y) / sizeof(int); index++){
        for (int d = 1; d < dst+1; d++){
            current_x = x + (QUEEN_X[index] * d);
            current_y = y + (QUEEN_Y[index] * d);

            if (out_of_bounds(current_y, current_x) == 1){
                break;
            }
            current_val = board[current_y][current_x];
            if (current_val * turn > 0){
                break;
            }
            if (current_val * turn < 0){
                if (abs(current_val) == 9){
                    return 1;
                }
                if (abs(current_val) == 5){
                    if (QUEEN_Y[index] == 0 || QUEEN_X[index] == 0){
                        return 1;
                    }
                }
                if (abs(current_val) == 3){
                    if (QUEEN_Y[index] != 0 && QUEEN_X[index] != 0){
                        return 1;
                    }
                }
                if (abs(current_val) == 1){
                    if (QUEEN_Y[index] == 1 && QUEEN_X[index] == 1 && abs(d) == 1){
                        return 1;
                    }
                }
                if (abs(current_val) == 100){
                    if (d == 1){
                        return 1;
                    }
                }
            }
        }
    }
    for (int index = 0; index < sizeof(KNIGHT_Y) / sizeof(int); index++){
        current_x = x + KNIGHT_X[index];
        current_y = y + KNIGHT_Y[index];

        if (out_of_bounds(current_y, current_x) == 1){
            //printf("Out of bounds with\n");
            continue;
        }
        current_val = board[current_y][current_x];
        if (current_val * turn > 0){
            continue;
        }
        if (current_val * turn < 0){
            if (abs(current_val == 3)){
                return 1;
            }
        }
    }
    //printf("NOT IN CHECK HER \n");
    return 0;
    
}


void push_move(char from[3], char to[3], char* piece, move_t** head, int turn){
    //printf("pushing move\n");
    move_t* node = (move_t*) malloc(sizeof(move_t));
    strcpy(node->from, from);
    strcpy(node->to, to);
    strcpy(node->piece, piece);
    //move(node);
    //if (turn == 1 && is_check(w_king_x, w_king_y, turn) == 1){
        //pop(node);
        //return;
    //}
    //if (turn == -1 && is_check(b_king_x, b_king_y, turn) == 1){
        //pop(node);
        //return;
    //}
    //pop(node);
    node -> next = *head;
    *head = node;

}



/* Gets all legal pawn moves */
int get_legal_pawn(int turn, int y, int x, move_t** head){
    //printf("Finding pawn moves at x: %d, y: %d\n", x, y);
    int dst = 1;
    if (y == 1 || y == 6){
        dst = 1;
    }
    int current_x, current_y, current_val;
    for (int index = 0; index < sizeof(PAWN_Y) / sizeof(int); index++){
        if (y != 1 && y != 6){
            if (PAWN_Y[index] == 2){
                continue;
            }
        }
        if (turn == 1){
            current_x = x + (PAWN_X[index]);
            current_y = y + (PAWN_Y[index] * -1);
        }
        else{
            current_x = x + (PAWN_X[index]);
            current_y = y + (PAWN_Y[index]);
        }

        if (PAWN_Y[index] == 2){
            if (board[current_y -turn*-1][current_x] != 0){
                continue;
            }
        }

        if (out_of_bounds(current_y, current_x) == 1){
            //printf("Out of bounds with\n");
            continue;
        }
        current_val = board[current_y][current_x];

        if (PAWN_Y[index] != 0 && PAWN_X[index] != 0){
            if (current_val * turn >= 0){
                continue;
            }
        }
        if (PAWN_Y[index] != 0 && PAWN_X[index] == 0){
            if (current_val * turn < 0){
                continue;
            }

        }

        //printf("current_x %d, current_y %d, current_val: %d\n", current_x, current_y, current_val);
        if (current_val * turn > 0){
            continue;
        }
        else{
            char from[3];
            char to[3];
            index_to_letter(x, y, from);
            index_to_letter(current_x, current_y, to);
            push_move(from, to, "pawn", head, turn);
            //printf("New pawn move from: %s, to: %s\n", m.from, m.to);

        }
    }
}

int get_legal_knight(char turn, int y, int x, move_t** head){
    //printf("Finding knight moves at x: %d, y: %d\n", x, y);

    int current_x, current_y, current_val;
    for (int index = 0; index < sizeof(KNIGHT_Y) / sizeof(int); index++){
        current_x = x + KNIGHT_X[index];
        current_y = y + KNIGHT_Y[index];

        if (out_of_bounds(current_y, current_x) == 1){
            //printf("Out of bounds with\n");
            continue;
        }
        current_val = board[current_y][current_x];
        if (current_val * turn > 0){
            continue;
        }
        else{
            char from[3];
            char to[3];
            index_to_letter(x, y, from);
            index_to_letter(current_x, current_y, to);
            push_move(from, to, "knight", head, turn);
            //printf("New knight move from: %s, to: %s\n", m.from, m.to);

        }
    }

}
int get_legal_bish(char turn, int y, int x, move_t** head){
    //printf("Finding bish moves at x: %d, y: %d\n", x, y);
    int dst = 7;
    int current_x, current_y, current_val;
    for (int index = 0; index < sizeof(BISH_Y) / sizeof(int); index++){
        for (int d = 1; d < dst+1; d++){
            current_x = x + (BISH_X[index] * d);
            current_y = y + (BISH_Y[index] * d);

            if (out_of_bounds(current_y, current_x) == 1){
                //printf("Out of bounds with\n");
                break;
            }
            current_val = board[current_y][current_x];
            //printf("current_x %d, current_y %d, current_val: %d\n", current_x, current_y, current_val);
            if (turn * current_val > 0){
                break;
            }
            else{
                char from[3];
                char to[3];
                index_to_letter(x, y, from);
                index_to_letter(current_x, current_y, to);
                push_move(from, to, "bish", head, turn);
                if (current_val != 0){
                    break;
                }
                //printf("New pawn move from: %s, to: %s\n", m.from, m.to);

            }
        }
    }

}
int get_legal_rook(char turn, int y, int x, move_t** head){
    //printf("Finding rook moves at x: %d, y: %d\n", x, y);
    int dst = 7;
    int current_x, current_y, current_val;
    for (int index = 0; index < sizeof(ROOK_Y) / sizeof(int); index++){
        for (int d = 1; d < dst+1; d++){
            current_x = x + (ROOK_X[index] * d);
            current_y = y + (ROOK_Y[index] * d);
            //printf("ROOK: current_x %d, current_y %d\n", current_x, current_y);

            if (out_of_bounds(current_y, current_x) == 1){
                //printf("Out of bounds with\n");
                break;
            }
            current_val = board[current_y][current_x];
            //printf("ROOK: vurrent_val %d\n", current_val);
            if (current_val * turn > 0){
                break;
            }
            else{
                char from[3];
                char to[3];
                index_to_letter(x, y, from);
                index_to_letter(current_x, current_y, to);
                push_move(from, to, "rook", head, turn);
                if (current_val != 0){
                    break;
                }
                //printf("New pawn move from: %s, to: %s\n", m.from, m.to);

            }
        }
    }

}
int get_legal_king(char turn, int y, int x, move_t** head){
    //printf("Finding KING moves at x: %d, y: %d\n", x, y);
    int current_x, current_y, current_val;
    for (int index = 0; index < sizeof(KING_Y) / sizeof(int); index++){
        current_x = x + KING_X[index];
        current_y = y + KING_Y[index];

        if (out_of_bounds(current_y, current_x) == 1){
            //printf("Out of bounds with\n");
            continue;
        }
        current_val = board[current_y][current_x];
        //printf("current_x %d, current_y %d, current_val: %d\n", current_x, current_y, current_val);
        if (current_val * turn > 0){
            continue;
        }
        else{
            char from[3];
            char to[3];
            index_to_letter(x, y, from);
            index_to_letter(current_x, current_y, to);
            push_move(from, to, "king", head, turn);
            //printf("New pawn move from: %s, to: %s\n", m.from, m.to);

        }
    }

}
int get_legal_queen(char turn, int y, int x, move_t** head){
    //printf("Finding KING moves at x: %d, y: %d\n", x, y);
    int current_x, current_y, current_val;
    int dst = 7;
    for (int index = 0; index < sizeof(QUEEN_Y) / sizeof(int); index++){
        for (int d = 1; d < dst+1; d++){
            current_x = x + (QUEEN_X[index] * d);
            current_y = y + (QUEEN_Y[index] * d);

            if (out_of_bounds(current_y, current_x) == 1){
                //printf("Out of bounds with\n");
                break;
            }
            current_val = board[current_y][current_x];
            //printf("current_x %d, current_y %d, current_val: %d\n", current_x, current_y, current_val);
            if (current_val * turn > 0){
                break;
            }
            else{
                char from[3];
                char to[3];
                index_to_letter(x, y, from);
                index_to_letter(current_x, current_y, to);
                push_move(from, to, "queen", head, turn);
                if (current_val != 0){
                    break;
                }
                //printf("New pawn move from: %s, to: %s\n", m.from, m.to);

            }
        }
    }

}


void legal_moves(int turn, move_t** head){
    //printf("Finding legal moves \n");
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (! correct_color(i, j, turn)){
                continue;
            }
            if (abs(board[i][j]) == 1){
                get_legal_pawn(turn, i, j, head);
            }
            if (abs(board[i][j]) == 3){
                get_legal_knight(turn, i, j, head);
            }
            if (abs(board[i][j]) == 4){
                get_legal_bish(turn, i, j, head);
            }
            if (abs(board[i][j]) == 5){
                get_legal_rook(turn, i, j, head);
            }
            if (abs(board[i][j]) == 100){
                get_legal_king(turn, i, j, head);
            }
            if (abs(board[i][j]) == 9){
                get_legal_queen(turn, i, j, head);
            }
        }
    }
}




/* Prints the board prettyly */
void print_board(){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            printf("%d\t|", board[i][j]);
        }
            
        printf("\n");
    }
}

/* Creates a board form a fen-string */
void fen_to_board(char* fen){
    int y = 0;
    int x = 0;
    for (int i = 0; i < strlen(fen); i++){
        char current = fen[i];
        if (current == '/'){
            x = 0;
            y += 1;
            continue;
        }
        if (current == ' '){
            printf("braking here\n");
            break;
        }

        if (isdigit(current)){
            x += (int)atoi(&current);
            continue;
        }

        if (current == 'r'){
            board[y][x] = -5;
        }
        else if (current == 'p'){
            board[y][x] = -1;
        }
        else if (current == 'k'){
            b_king_x = x;
            b_king_y = y;
            board[y][x] = -100;
        }
        else if (current == 'q'){
            board[y][x] = -9;
        }
        else if (current == 'b'){
            board[y][x] = -4;
        }
        else if (current == 'n'){
            board[y][x] = -3;
        }
        else if (current == 'R'){
            board[y][x] = 5;
        }
        else if (current == 'P'){
            board[y][x] = 1;
        }
        else if (current == 'K'){
            w_king_x = x;
            w_king_y = y;
            board[y][x] = 100;
        }
        else if (current == 'Q'){
            board[y][x] = 9;
        }
        else if (current == 'B'){
            board[y][x] = 4;
        }
        else if (current == 'N'){
            board[y][x] = 3;
        }
        x += 1;

    }

}


void create_ret_string(char* ret, move_t* head){
    int index = 0;
    move_t* current = head;
    while (current != NULL){
        printf("current move from: %s, to: %s :: %s\n", current->from, current->to, current->piece);
        strcpy(&ret[index], current->from);
        strcpy(&ret[index+2], current->to);
        ret[index+4] = '|';
        current = current -> next;
        index += 5;
    }
    ret[index] = '\0';

    
}

score_t* rec(int depth, int turn, int og_depth, int alpha, int beta){
    //printf("calling rec here! depth: %d, turn:%d, og_depth: %d, alpha: %d, beta: %d\n", depth, turn, og_depth, alpha, beta);
    //print_board();
    //printf("--------------------------------------------------------------------\n");
    if (depth == 0){
        //print_board();
        //printf("--------------------------------------------------------------------\n");
        score_t* ret = (score_t*)malloc(sizeof(score_t));
        ret -> score = get_board_score();
        //printf("score: %d\n", ret->score);
        //printf("move from: %s to: %s\n", ret->move->from, ret->move->to);
        ret->move = NULL;
        return ret;
    }
    move_t* head = NULL;
    legal_moves(turn, &head);
    move_t* current = head;

    score_t* winner = (score_t*) malloc(sizeof(score_t));
    winner -> score = 0;
    winner -> move = current;
    
    //printf("we get here depth: %d\n", depth);
    int score_found = -1;
    while (current != NULL){
        //printf("current_move: %s, %s\n", current -> from, current -> to);
        //printf("move from: %s to: %s\n", current->from, current->to);
        move(current);
        score_t* new_score;
        if (turn == -1){
            new_score = rec(depth-1, turn*-1, og_depth, alpha, beta);
            pop(current);
            //printf("black new score: %d winner score: %d\n", new_score->score, winner->score);
            if (score_found == -1 || new_score -> score < winner -> score){
                //printf("updates winner\n");
                winner -> score = new_score -> score;
                winner -> move = current;
                score_found = 1;
            }
            if (new_score -> score < beta){
                beta = new_score -> score;
            }
            free(new_score);
            if (beta <= alpha){
                break;
            }
        }
        else{
            new_score = rec(depth-1, turn*-1, og_depth, alpha, beta);
            pop(current);
            //printf("white new score: %d winner score: %d\n", new_score->score, winner->score);
            //printf("new_score: %d\n", new_score->score);
            if (score_found == -1 || new_score -> score > winner -> score){
                //printf("updates winner\n");
                winner -> score = new_score -> score;
                winner -> move = current;
                score_found = 1;
            }
            if (new_score -> score > alpha){
                alpha = new_score -> score;
            }
            free(new_score);
            if (beta <= alpha){
                break;
            }

        }
    current = current -> next;
    }
    current = head;
    while (current != NULL){
        if (winner->move != current){
            free(current);
        }
        current = current->next;
    }
    if (winner == NULL){
        winner = (score_t*) malloc(sizeof(score_t));
        if (turn == -1){
            winner->score = 1000000 *(depth+1);
            winner->move = NULL;
        }
        else if (turn == 1){
            winner->score = -1000000 *(depth+1);
            winner->move = NULL;
        }
    } 
    return winner;
}

/* Main function */
int main(){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            board[i][j] = 0;
        }
    }
    fen_to_board(START_FEN);
    print_board();
    /*
    move_t* head = NULL;
    legal_moves(1, &head);
    move_t* current = head;
    while (current != NULL){
        printf("current from: %s, to: %s\n", current->from, current->to);
        current = current -> next;
    }
    */


    //char* ret = malloc(400);
    //move_t* head = NULL;
    //legal_moves(1, &head);
    int depth = 6;
    printf("Starting search with depth: %d\n", depth);
    int og_depth = depth;
    int turn = 1;
    score_t* best = rec(depth, turn, og_depth, -10000, 10000);
    printf("done here\n");
    printf("best move [ score: %d, from: %s, to: %s\n", best->score, best->move->from, best->move->to);
    int start_score = get_board_score();
    printf("start score: %d\n", start_score);
    print_board();

    char* ret = malloc(5);
    ret[0] = best->move->from[0];
    ret[1] = best->move->from[1];
    ret[2] = best->move->to[0];
    ret[3] = best->move->to[1];
    ret[4] = '\0';

    printf("ret string: %s\n", ret);
}

char* hax(char* fen, int turn, int depth){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            board[i][j] = 0;
        }
    }
    fen_to_board(fen);
    

    //int depth = 2;
    int og_depth = depth;
    printf("starting stuff turn: %d\n", turn);
    int start_score = get_board_score();
    printf("strudel score: %d\n", start_score);
    score_t* best = rec(depth, turn, og_depth, -10000, 10000);
    printf("REC DONE\n");
    printf("best move: from: %s, to: %s\n", best->move->from, best->move->to);
    printf("best move [ score: %d, from: %s, to: %s\n", best->score, best->move->from, best->move->to);
    print_board();

    char* ret = malloc(5);
    ret[0] = best->move->from[0];
    ret[1] = best->move->from[1];
    ret[2] = best->move->to[0];
    ret[3] = best->move->to[1];
    ret[4] = '\0';


    printf("ret string: %s\n", ret);
    return ret;

}
