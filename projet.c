#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgres.h>
#include <float.h>
#include <math.h>
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "smallchesslib.h"
#include <fmgr.h>
#define DatumGetBoardP(X)  ((SCL_Board *) DatumGetPointer(X))
#define BoardPGetDatum(X)  PointerGetDatum(X)
#define PG_GETARG_BOARD_P(n) DatumGetBoardP(PG_GETARG_DATUM(n))
#define PG_RETURN_BOARD_P(x) return BoardPGetDatum(x)
#define DatumGetChessgameP(X)  ((chessgame *) DatumGetPointer(X))
#define BoardPGetDatum(X)  PointerGetDatum(X)
#define PG_GETARG_BOARD_P(n) DatumGetBoardP(PG_GETARG_DATUM(n))
#define PG_RETURN_BOARD_P(x) return BoardPGetDatum(x)
PG_MODULE_MAGIC;



typedef struct
{
     char* record;
} chessgame;

typedef struct
{
     char san[69];
} chessboard;

PG_FUNCTION_INFO_V1(chessgame_in);

Datum chessgame_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);

    chessgame *result = (chessgame *) palloc(sizeof(chessgame));
    
    result->record=pstrdup(str);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(chessgame_out);

Datum chessgame_out(PG_FUNCTION_ARGS) {
    chessgame *val = (chessgame *) PG_GETARG_POINTER(0);
   //char* result= pstrdup(val->record);
   char* result= pstrdup(val->record);
    PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(chessboard_in);

Datum chessboard_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);

    chessboard *result = (chessboard *) palloc(sizeof(chessboard));
    strncpy(result->san, str, sizeof(result->san) - 1);
    result->san[sizeof(result->san) - 1] = '\0';

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(chessboard_out);

Datum chessboard_out(PG_FUNCTION_ARGS) {
    chessboard *val = (chessboard *) PG_GETARG_POINTER(0);

    PG_RETURN_CSTRING(val->san);
}

/*static bool
chessgame_eq_internal(chessgame *game1, chessgame *game2)
{
  return ( (strlen(game1->pgn)- strlen (game2->pgn))==0 &&strcmp(game1->pgn, game2->pgn)==0);
}

PG_FUNCTION_INFO_V1(chessgame_eq);
Datum
chessgame_eq(PG_FUNCTION_ARGS)
{
  chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
  chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
  bool result = chessgame_eq_internal(game1, game2);
  PG_FREE_IF_COPY(game1, 0);
  PG_FREE_IF_COPY(game2, 1);
  PG_RETURN_BOOL(result);
}

static bool
chessgame_less_move_internal(chessgame *game1, chessgame *game2)
{
  return ( (strlen(game1->pgn)- strlen (game2->pgn))<0);
}

PG_FUNCTION_INFO_V1(chessgame_less_move);
Datum
chessgame_less_move(PG_FUNCTION_ARGS)
{
  chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
  chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
  bool result = chessgame_less_move_internal(game1, game2);
  PG_FREE_IF_COPY(game1, 0);
  PG_FREE_IF_COPY(game2, 1);
  PG_RETURN_BOOL(result);
}
static bool
chessgame_more_move_internal(chessgame *game1, chessgame *game2)
{

  return ( (strlen(game1->pgn)- strlen (game2->pgn))>0);
}

PG_FUNCTION_INFO_V1(chessgame_more_move);
Datum
chessgame_more_move(PG_FUNCTION_ARGS)
{
  chessgame *game1 =(chessgame *) PG_GETARG_POINTER(0);
  chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
  bool result = chessgame_more_move_internal(game1, game2);
  PG_FREE_IF_COPY(game1, 0);
  PG_FREE_IF_COPY(game2, 1);
  PG_RETURN_BOOL(result);
}
*/

	
PG_FUNCTION_INFO_V1(getBoard);


SCL_Board * board_value(SCL_Record record, int moves){
	const char* initial_state= palloc(256);
	strcpy(initial_state,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    SCL_Board* board= palloc(sizeof(SCL_Board));
    board=NULL;
    SCL_boardFromFEN(*board, initial_state); 
	//free(initial_state);
	SCL_recordApply(record, *board, moves);
	return board;
	}


/**  La function getBoard  prend en 
 * 
 * 
 */
Datum getBoard(PG_FUNCTION_ARGS) {
	const SCL_Record* game = PG_GETARG_POINTER(0);
    int halfMoves = PG_GETARG_INT32(1);
  
	SCL_Board *board = board_value(*game, halfMoves);
	char* result=(char*) palloc(256);
	result='\0';
	SCL_boardToFEN(*board,result);
	PG_RETURN_CSTRING(result);
}
char* skipSpaces(char* str) {
    while (*str == ' ') {
        str++;
    }
    return str;
}

PG_FUNCTION_INFO_V1(getFirstMoves);
// Fonction pour obtenir les premiers N demi-mouvements d'un jeu d'échec
Datum getFirstMoves(PG_FUNCTION_ARGS) {
	const chessgame* game=PG_GETARG_POINTER(0);
	int numMoves=PG_GETARG_INT32(1);
    // Utiliser strtok pour diviser le record en mouvements individuels
    char* notationCopy = strdup(game->record);
    char* token = strtok(notationCopy, " ");
   char*  result=(char*) palloc(8*strlen(game->record));
    result[0]='\0';
    int i=0;
    while( token!= NULL) {
		   //token=skipSpaces(token);
            strcat(result, token);
            strcat(result, " ");  // Ajouter un espace entre les mouvements
            token = strtok(NULL, " ");
			i++;
			if (i==numMoves+ceil(numMoves/2)){
				break;
			}
   }
     free(notationCopy);

     PG_RETURN_CSTRING(result);
}
PG_FUNCTION_INFO_V1(hasOpening);

/**La fonction hasOpening permet de vérifier si un jeu d'échec( passé en premier argument) commence avec le meme ensemble de mouvements que le deuxième
   La fonction hasOpening_internal est sa représentation interne (ne pouvant pas etre appelée via le terminal). 
   * Elle cree deuxtableaux de caractères pour stocker le record de chaque game, effectue un slicing via strtok_r avec comme délimiteur l'espace " " pour 
   * récupérer vhaque demi-mouvement de chaque jeu et le comparer au suivant
   * ATTENTION: Les espacement de la notation SAN doivent etre respectés
*/
static bool;
hasOpening_internal(const chessgame* game1, const chessgame* game2){
	 char notationCopy1[256];
     char notationCopy2[256] ;
     const char* delimiter=" ";
     char* saveCopy1;
     char* saveCopy2;
     strcpy(notationCopy1, game1->record);   //copie les records des games dans les tableaux
     strcpy(notationCopy2, game2->record);
     char* tokengame1 = strtok_r(notationCopy1,delimiter, &saveCopy1); //effectue une première découpe
     char* tokengame2 = strtok_r(notationCopy2,delimiter, &saveCopy2);
     bool result=false;
    while( tokengame1!= NULL && tokengame2!= NULL ) {      //boucle while pour éffectuer en boucle les opérations découpe-comaparaisons
		   //token=skipSpaces(token);
		   ereport(NOTICE, errmsg("la chaine du game 1 à comparer avec l'autre est: %s",tokengame1));  //affichage au terminal des tokens à comparer 
		   ereport(NOTICE, errmsg("la chaine du game 2 à comparer avec l'autre est: %s",tokengame2));
           if (strcmp(tokengame1,tokengame2)!=0){      //s'il ya une différencee dans un seul demi-mouvement, alors la fonction doit renvoyer false et on arrte la comaraison via break
			   ereport(NOTICE, errmsg("non similarité"));
			   result=false;
			   break;
		   }
		   //Passage au demi-mouvement suivant
		  tokengame1 = strtok_r(NULL,delimiter, &saveCopy1);
          tokengame2 = strtok_r(NULL,delimiter, &saveCopy2);
	}
	
	if(tokengame2== NULL){
	     result= true;
	     ereport(NOTICE, errmsg(" similarité"));
	 }
	 return result;
}


Datum hasOpening(PG_FUNCTION_ARGS) {
	
	const SCL_Record* game1=PG_GETARG_POINTER(0);
	const SCL_Record* game2=PG_GETARG_POINTER(1);
    // Utiliser strtok pour diviser le record en mouvements individuels
     bool result=  hasOpening_internal(game1,game2);
	 PG_FREE_IF_COPY(game1, 0);
	 PG_FREE_IF_COPY(game2, 0);
     PG_RETURN_BOOL(result);
}
/*
INSERT INTO games (z) VALUES ('1. e4 e5 2. Nf3 Nc6 3. Bb5 {This opening is called the Ruy Lopez.} 3... a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 d6 8. c3 O-O 9. h3 Nb8 10. d4 Nbd7 11. c4 c6 12. cxb5 axb5 13. Nc3 Bb7 14. Bg5 b4 15. Nb1 h6 16. Bh4 c5 17. dxe5 Nxe4 18. Bxe7 Qxe7 19. exd6 Qf6 20. Nbd2 Nxd6 21. Nc4 Nxc4 22. Bxc4 Nb6 23. Ne5 Rae8 24. Bxf7+ Rxf7 25. Nxf7 Rxe1+ 26. Qxe1 Kxf7 27. Qe3 Qg5 28. Qxg5 hxg5 29. b3 Ke6 30. a3 Kd6 31. axb4 cxb4 32. Ra5 Nd5 33. f3 Bc8 34. Kf2 Bf5 35. Ra7 g6 36. Ra6+ Kc5 37. Ke1 Nf4 38. g3 Nxh3 39. Kd2 Kb5 40. Rd6 Kc5 41. Ra6 Nf2 42. g4 Bd3 43. Re6 1/2-1/2');
INSERT INTO games (z) VALUES ('1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 d6 8. c3 O-O 9. h3 Nb8 10. d4 Nbd7 11. c4 c6 12. cxb5 axb5 13. Nc3 Bb7 14. Bg5 b4 15. Nb1 h6 16. Bh4 c5 17. dxe5 Nxe4 18. Bxe7 Qxe7 19. exd6 Qf6 20. Nbd2 Nxd6 21. Nc4 Nxc4 22. Bxc4 Nb6 23. Ne5 Rae8 24. Bxf7+ Rxf7 25. Nxf7 Rxe1+ 26. Qxe1 Kxf7 27. Qe3 Qg5 28. Qxg5 hxg5 29. b3 Ke6 30. a3 Kd6 31. axb4 cxb4 32. Ra5 Nd5 33. f3 Bc8 34. Kf2 Bf5 35. Ra7 g6 36. Ra6+ Kc5 37. Ke1 Nf4 38. g3 Nxh3 39. Kd2 Kb5 40. Rd6 Kc5 41. Ra6 Nf2 42. g4 Bd3 43. Re6 1/2-1/2');
*/


static bool hasBoard_internal(const chessgame* game, const chessboard* board, int numMoves) {
    // Extract the board state from the chess game for the first N half-moves
    SCL_Board currentBoard;
    SCL_boardInit(&currentBoard);

    char* notationCopy = strdup(game->record);
    char* token = strtok(notationCopy, " ");

    int moveCount = 0;

    while (token != NULL && moveCount < numMoves) {
        // Apply the move to the current board
        applyMove(&currentBoard, token);

        token = strtok(NULL, " ");
        moveCount++;
    }

    free(notationCopy);

    // Check if the current board matches the specified board state
    char fen[256];
    SCL_boardToFEN(currentBoard, fen);
    char* targetFen = board->san;

    // You may need to implement a custom string comparison function
    if (strcmp(fen, targetFen) == 0) {
        return true;
    }

    return false;
}

PG_FUNCTION_INFO_V1(hasBoard);

Datum hasBoard(PG_FUNCTION_ARGS) {
    const chessgame* game = PG_GETARG_POINTER(0);
    const chessboard* board = PG_GETARG_POINTER(1);
    int numMoves = PG_GETARG_INT32(2);

    // Check if the chess game contains the specified board state
    bool result = hasBoard_internal(game, board, numMoves);

    PG_FREE_IF_COPY(game, 0);
    PG_FREE_IF_COPY(board, 1);

    PG_RETURN_BOOL(result);
}


