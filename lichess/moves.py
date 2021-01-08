import numba
import numpy as np
import multipliers
import time
from copy import deepcopy
import random
from py_board import Board
import chess
import functools

count = 0
game = None
board = None
def get_best_move(g, depth):
    global count, game, board
    game = deepcopy(g)
    count += 1
    turn = "w" if game.turn else "b"
    print("TUUUUUUUUUUUURN", turn)
    fen = game.board_fen()
    print("FEN: ", fen)
    board = None
    board = Board(fen.split()[0])
    print(board)
    new_game = deepcopy(game)
    depth = 2
    best = rec(depth, turn, depth, -10000, 10000, turn)[1]


    return best
    ret = {"best": {"from": best.uci()[:2], "to": best.uci()[2:]}}
    print(ret)
    return ret


def rec(depth, turn, og_depth, alpha, beta, og_turn):
    global game, board
    #print("STARTING FROM REC")
    #print(game)

    if depth == 0:
        if game.is_game_over():
            print("GAME OVER IN VARIANT")
            if game.result() == "1-0":
                print("GAME OVER - WHITE")
                return 100000 * (depth+1)
            elif game.result() == "0-1":
                print("GAME OVER - BLACK")
                return -100000 * (depth+1)
        if game.is_repetition():
            print("GAME OVER - DRAW")
            return 0

        current_score = get_board_score(board.grid)
        return current_score

            
    moves = game.legal_moves
    scores = []
    winner = (None, None)
    for m in moves:
        game.push(m)
        f, t = m.uci()[:2], m.uci()[2:]
        board.move(f, t)
        if turn == "b":
            new_score = rec(depth-1, "w", og_depth, alpha, beta, og_turn)
            #scores.append((new_score, m))
            #print(new_score)
            if not winner[0] or new_score < winner[0]:
                winner = (new_score, m)
            game.pop()
            board.pop()
            beta = min(beta, new_score)
            if beta <= alpha:
                break
        else:
            new_score = rec(depth-1, "b", og_depth, alpha, beta, og_turn)
            #scores.append((new_score, m))
            if not winner[0] or new_score > winner[0]:
                winner = (new_score, m)
            game.pop()
            board.pop()
            alpha = max(alpha, new_score)
            if beta <= alpha:
                break
    #print("RETURNING FROM REC")
    #print(game)
    #return winner
    #if turn == "b":
        #s = min(scores, key=lambda x:x[0])
    #else:
        #s = max(scores, key=lambda x:x[0])
    if not winner[0]:
        if turn =="b":
            print("FUCK YEEE ")
            return 100000 *(depth+1)
        elif turn =="w":
            print("FUCK YEEE ")
            return -100000 *(depth+1)
    
    elif depth != og_depth:
        s = winner[0] 
    else:
        s = winner
        print(s)
    return s

def sort_moves(moves, turn):
    print("SORTING MOVES")
    global game, board
    ret = []
    for m in moves:
        game.push(m)
        f, t = m.uci()[:2], m.uci()[2:]
        board.move(f, t)

        score = get_board_score(board.grid)
        ret.append((m,score))
        game.pop()
        board.pop()
    return [m for m, s in sorted(ret, key=lambda x: x[1], reverse=True if turn == "w" else False)]
    
def return_helper(turn, scores):
    if turn == "b":
        if not scores:
            s = 100000 * og_depth
        else:
            s = min(scores, key=lambda x:x[0])
    else:
        if not scores:
            s = -1000
        else:
            s = max(scores, key=lambda x:x[0])
    if depth != og_depth and scores:
        s = s[0] 
    else:
        print(s)
    return s


@numba.jit(nopython=True)
def get_board_score(grid):
    #return np.sum(grid)
    #mask = create_mask(grid)
    #return np.sum(grid + (mask * multipliers.hax[3]))

    score = 0
    for i, row in enumerate(grid):
        for j, elem in enumerate(row):
            #print("elem: ", elem)
            if abs(elem) == 100:
                hax = 0
                if elem > 0:
                    hax = elem + KING[i][j]
                else:
                    hax = elem - KING[i][j]
                score += hax
            elif abs(elem) == 1:
                hax = 0
                if elem > 0:
                    hax = elem + PAWN[i][j] *0.5
                else:
                    hax = elem - PAWN[i][j] *0.5
                score += hax
            elif elem != 0:
                hax = 0
                if elem > 0:
                    hax = elem + KNIGHT[i][j] *.5
                else:
                    hax = elem - KNIGHT[i][j] *.5
                score += hax
    return score
    


def get_legal_moves(game):
    return [{"from": m.uci()[:2], "to":m.uci()[2:]} for m in game.legal_moves]

PAWN = np.array([[0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
        [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.05, 0.05],
        [0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02],
        [0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02],
        [0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02],
        [0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02],
        [0.00, 0.00, 0.00, 0.00, 0.00, 0.02, 0.05, 0.05],
        [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00]])



KNIGHT = np.array([[0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00]])

BISH = np.array([[0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00, 0.00],
          [0.00, 0.02, 0.02, 0.00, 0.02, 0.02, 0.00, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00, 0.00],
          [0.00, 0.02, 0.02, 0.02, 0.02, 0.02, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00]])

ROOK = np.array([[0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00]])

QUEEN = np.array([[0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00]])

KING = np.array([[0.00, 0.00, .04, 0.00, 0.00, 0.00, .04, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
          [0.00, 0.00, .04, 0.00, 0.00, 0.00, .04, 0.00]])
