-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION projet" to load this file. \quit


/* Définition des fonctions d'entrées et de sorties qui permettent de stocker des données de chaque type et de les restituer*/
 
 /* LANGUAGE C indique que la fonction est écrite en C */
 /* IMMUTABLE signifie  que la fonction renvoit la meme sortie pour les memes entrées */
 /* STRICT indique que la fonction renverra NULL si l'un des arguments est NULL.*/
 /* PARALLEL SAFE signifie que la fonction peut etre executée dans une requete parallele*/

CREATE OR REPLACE FUNCTION chessgame_in(cstring)
  RETURNS chessgame
  LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'chessgame_in';
 
CREATE OR REPLACE FUNCTION chessgame_out(chessgame)
  RETURNS cstring
  LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'chessgame_out';

 CREATE OR REPLACE FUNCTION chessboard_in(cstring)
  RETURNS chessboard
  LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'chessboard_in';
 
CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
  RETURNS cstring
  LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'chessboard_out';

/*Creation des types chessgame et chessboard*/
CREATE TYPE chessboard(
  internallength = 100,
  input          = chessboard_in,
  output         = chessboard_out,
  alignment      = char,
  storage        =plain
);

 
CREATE TYPE chessgame (
  internallength = 1200,
  input          = chessgame_in,
  output         = chessgame_out,
  alignment      = char,
  storage        =plain
);

/* Définitions des fonctions de comparaison de deux objets de type chessgame
CREATE FUNCTION chessgame_eq(chessgame, chessgame)
  RETURNS boolean
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE  AS '$libdir/projet', 'chessgame_eq';
CREATE FUNCTION chessgame_more_move(chessgame, chessgame)
  RETURNS boolean
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE AS '$libdir/projet', 'chessgame_more_move';
CREATE FUNCTION chessgame_less_move(chessgame, chessgame)
  RETURNS boolean
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE  AS '$libdir/projet', 'chessgame_less_move';
*/

-- Function to get the board state
CREATE OR REPLACE FUNCTION getBoard(chessgame chessgame, half_moves INTEGER)
RETURNS cstring
LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'getBoard';
CREATE OR REPLACE FUNCTION getFirstMoves(chessgame chessgame, half_moves INTEGER)
RETURNS cstring
LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'getFirstMoves';

CREATE OR REPLACE FUNCTION hasBoard(chessgame, chessboard, integer)
RETURNS BOOLEAN
LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'hasBoard';

CREATE OR REPLACE FUNCTION hasOpening(chessgame, chessgame)
RETURNS BOOLEAN
LANGUAGE C IMMUTABLE STRICT AS '$libdir/projet', 'hasOpening';


/*CREATE OPERATOR =(
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_eq
);
CREATE OPERATOR <(
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_less_move
);
CREATE OPERATOR >(
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_more_move
);

CREATE OPERATOR CLASS chessgame_ops
   DEFAULT FOR TYPE chessgame USING btree AS 
   OPERATOR 1 = (chessgame, chessgame),
   OPERATOR 2 < (chessgame, chessgame),
   OPERATOR 3 > (chessgame, chessgame);
   */
