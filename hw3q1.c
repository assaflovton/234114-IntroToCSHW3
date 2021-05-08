/*-------------------------------------------------------------------------
	Include files:
-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdbool.h>

/*-------------------------------------------------------------------------
	Constants and definitions:
-------------------------------------------------------------------------*/
#define N 11
#define NUM_OF_PLAYERS 2
#define EMPTY_CELL '_'
#define SYMBPLAYERONE 'X'
#define SYMBPLAYERTWO 'O'
#define GAMEON 0
#define GAMEOVER 1
#define GAMEUNDO -1
#define FIXWINNER 3
#define	EVENCHECK 2
#define GETCOLUM -1
#define GETROW -2
#define FIXMATRIXCOOR -1
#define FIXUNDONEGATIVE -1
#define ISNEGATIVE -1
#define FIXTURNBYONE 1
#define POSITIVE 0







/*-------------------------------------------------------------------------
	Function declaration
-------------------------------------------------------------------------*/
void print_welcome();
void print_enter_board_size();
void print_board(char board[N][N], int n);
void print_player_turn(int player_index);
void print_error();
void print_winner(int player_index);
void print_tie();
char get_cur_player_symbol(int turns_played);
int turn(char board[][N], int turns_played_memory[], int board_size, int turns_played);
bool place_symbol_in_board(int row, int colum, int turns_played, char board[][N], int board_size, int turns_played_memory[]);
bool check_for_win_main_cross(char turn_symb, char board[][N], int board_size);
bool check_for_win_seconderay_cross(char turn_symb, char board[][N], int board_size);
bool check_for_win_row(int row, char turn_symb, char board[][N], int board_size);
bool check_for_win_colum(int colum, char turn_symb, char board[][N], int board_size);
int check_for_game_status(int row, int colum, char turn_symb, char board[][N], int board_size);
void game_loop(char board[][N], int turns_played_memory[], int board_size);
int utlize_board(char board[][N]);
void board_initialization_with_char(char board[][N], int board_size);
int cur_player_index(int turns_played);
void print_game_result(int game_status, int player_index);
int undo(int turns_to_undo, int turns_played, char board[][N], int turns_played_memory[]);
void undo_one_turn(int row, int colum, char board[][N]);
void result_is_tie(int game_status);


/*-------------------------------------------------------------------------
	Implementation
-------------------------------------------------------------------------*/
//5 lines
int main()
{
	char board[N][N];//the game board
	int board_size = utlize_board(board);
	int turns_played_memory[NUM_OF_PLAYERS * N * N];//every two cells reprsent a coordinate, the first
	game_loop(board, turns_played_memory, board_size);//a loop that runs the game
	return 0;
}

//while the board game is not full, calls the turn gunc and updates the game status accordingly. if board is full its a tie
//13 lines
void game_loop(char board[][N], int turns_played_memory[], int board_size)
{
	int turns_played = 0, game_status = GAMEON;//the game status can be one of 3:GAMEON- GAMEOVER GAMEUNDO
	while (turns_played < board_size * board_size) //there is at leat one cell empty
	{
		game_status = turn(board, turns_played_memory, board_size, turns_played);
		if (game_status <= GAMEUNDO) {
			turns_played += game_status;
			print_board(board, board_size);
			continue;
		}
		print_board(board, board_size);
		print_game_result(game_status, cur_player_index(turns_played));
		if (game_status == GAMEOVER)
			break;
		turns_played++;
	}
	result_is_tie(game_status);//board is full no one won.
}

//return game status after a turn made, 0 for gameon, 1 for gameover, any oter number is the number of turns to undo
//13 lines
int turn(char board[][N], int turns_played_memory[], int board_size, int turns_played)
{
	int row, colum, valid_turn = false;
	print_player_turn(cur_player_index(turns_played));
	while (!valid_turn)
	{
		scanf(" %d", &row);
		if (row <= ISNEGATIVE) //scanned number of turns to undo
		{
			valid_turn = undo(row * FIXUNDONEGATIVE, turns_played, board, turns_played_memory);//undo inpus is negative
			if (valid_turn)
				return row;
		}
		else//scanned coordinators input
		{
			scanf(" %d", &colum);
			valid_turn = place_symbol_in_board(FIXMATRIXCOOR + row, FIXMATRIXCOOR + colum, turns_played, board, board_size, turns_played_memory);
			if (valid_turn)
				return check_for_game_status(FIXMATRIXCOOR + row, FIXMATRIXCOOR + colum, get_cur_player_symbol(turns_played), board, board_size);
		}
	}
	return  GAMEON;
}

//place the symbol of the player in the requseted coordinates, check they within borders and the cell is empy, return true is move is possible
//7 lines
bool place_symbol_in_board(int row, int colum, int turns_played, char board[][N], int board_size, int turns_played_memory[])
{
	if (board[row][colum] == EMPTY_CELL && (row >= POSITIVE ) && (row < board_size) && (colum >= POSITIVE) &&(colum < board_size))
	{
		board[row][colum] = get_cur_player_symbol(turns_played);
		turns_played_memory[(turns_played + FIXTURNBYONE) * NUM_OF_PLAYERS + GETROW] = row;
		turns_played_memory[(turns_played + FIXTURNBYONE) * NUM_OF_PLAYERS + GETCOLUM] = colum;
		return true;
	}
	print_error();
	return false;
}

//get a number of turn to undo returns the number of turns that was undo
//7 lines
int undo(int turns_to_undo, int turns_played, char board[][N], int turns_played_memory[])
{
	if ((turns_to_undo % EVENCHECK) && (turns_to_undo <= turns_played))//uneven number
	{
		for (int i = 0; i < turns_to_undo; i++)
		{
			undo_one_turn(turns_played_memory[turns_played * NUM_OF_PLAYERS + GETROW],
				turns_played_memory[turns_played * NUM_OF_PLAYERS + GETCOLUM], board);
			//every turn coordinates is saved as [row][colum]=2 cells in turns_played_memory. all the turns of both players are in the same array.
			turns_played--;//updates the turn played counter
		}
		return turns_to_undo;
	}
	return 0;
}

//	//utilizes the cell back to be empty
//1 line
void undo_one_turn(int row, int colum, char board[][N])
{
	board[row][colum] = EMPTY_CELL;
}

//return the currnet player index X is player 1 ,O is player 2
//1 line
int cur_player_index(int turns_played)
{
	return turns_played % NUM_OF_PLAYERS + FIXTURNBYONE;
}

//return gameover if the same symbol occured in every cell along the main cross, if not return gameon
//7 lines
bool check_for_win_main_cross(char turn_symb, char board[][N], int board_size)
{
	int counter = 0;
	for (int i = 0; i < board_size; i++)
	{
		if (turn_symb == board[i][i])
		{
			counter++;
		}
	}
	if (counter == board_size)
	{
		return GAMEOVER;
	}
	return GAMEON;
}

//return gameover if the same symbol occured in every cell along the seconderay cross, if not return gameon
//7 lines
bool check_for_win_seconderay_cross(char turn_symb, char board[][N], int board_size)
{
	int counter = 0, board_colum = board_size;
	for (int i = 0; i < board_size; i++)
	{
		if (turn_symb == board[i][board_colum - i +FIXMATRIXCOOR])
			counter++;
	}
	if (counter == board_size)
	{
		return GAMEOVER;
	}
	return GAMEON;
}

//return gameover if the same symbol occured in every cell along the given row, if not return gameon
//7 lines
bool check_for_win_row(int row, char turn_symb, char board[][N], int board_size)
{
	int counter = 0;
	for (int i = 0; i < board_size; i++)
	{
		if (turn_symb == board[row][i])
		{
			counter++;
		}
	}
	if (counter == board_size)
	{
			return GAMEOVER;
	}
	return GAMEON;
}

//return gameover if the same symbol occured in every cell along the given colum, if not return gameon
//7 lines
bool check_for_win_colum(int colum, char turn_symb, char board[][N], int board_size)
{
	int counter = 0;
	for (int i = 0; i < board_size; i++)
	{
		if (turn_symb == board[i][colum])
		{
			counter++;
		}
	}
	if (counter == board_size) {
		return GAMEOVER;
	}
	return GAMEON;
}

//check if a win happened and updates the game status, if a win was made returns gameover otherwise gameon
//7 lines
int check_for_game_status(int row, int colum, char turn_symb, char board[][N], int board_size)
{
	int game_status = GAMEON;
	if (check_for_win_colum(colum, turn_symb, board, board_size) ||
		check_for_win_row(row, turn_symb, board, board_size) ||
		check_for_win_main_cross(turn_symb, board, board_size)  ||
		check_for_win_seconderay_cross(turn_symb, board, board_size))
		game_status = GAMEOVER;
	return game_status;
}

//check is the game staus is a tie when the board is full
//2 lines
void result_is_tie(int game_status)
{
	if (game_status == GAMEON)
		print_tie();
}

//print the result of the game
//2 lines
void print_game_result(int game_status, int player_index)
{
	if (game_status == GAMEOVER) {
		print_winner(FIXWINNER - player_index);
		//if player 2 made the last move player 1 won, else its player 1 that made the move and 2 won
	}

}

//scan the requsted borad size and call a func to utilze the board with the empy char
//7 lines
int utlize_board(char board[][N])
{
	int board_size = 0;
	print_welcome();
	print_enter_board_size();
	scanf(" %d", &board_size);
	board_initialization_with_char(board, board_size);
	print_board(board, board_size);
	return board_size;
}

//return the symbole of the current player based on the number of the turn that already have been played
//3 lines
char get_cur_player_symbol(int turns_played)
{
	if (turns_played % NUM_OF_PLAYERS) // X is 0, O is 1
		return SYMBPLAYERTWO;
	else
		return SYMBPLAYERONE;
}

//set a selected char to all cells in the matrix
//3 lines
void board_initialization_with_char(char board[][N], int board_size)
{
	for (int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size; j++)
			board[i][j] = EMPTY_CELL;
	}
}


//print welcome message
//1 lines
void print_welcome()
{
	printf("*** Welcome to AVOIDANCE TIC-TAC-TOE game ***\n\n");
}

//print message to enter board size
//1 lines
void print_enter_board_size()
{
	printf("Please enter board size (1 to %d):\n", N);
}


//print the board
//7 lines
void print_board(char board[N][N], int n)
{
	printf("\nCurrent board:\n");
	for (int i = 0; i < n; i++)
	{
		printf("|");
		for (int j = 0; j < n; j++)
		{
			printf("%c|", board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

//print a request for player with index "player_index" to move
//1 lines
void print_player_turn(int player_index)
{
	printf("\nPlayer ** %d **, enter next move:\n", player_index);
}

//print error message
//1 lines
void print_error()
{
	printf("Illegal move!!!, please try again:\n");
}

//print the winner
//1 lines
void print_winner(int player_index)
{
	printf("Player %d Wins! Hooray!\n", player_index);
}

//print message to announce there is a tie (no one wins)
//1 lines
void print_tie()
{
	printf("It's a tie!\n");
}
