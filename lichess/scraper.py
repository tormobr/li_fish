import random
import sys
from pynput.keyboard import Key, Controller
import chess
import time
import re
from urllib.request import urlopen
#from moves import get_best_move
from moves import get_best_move
from ctypes import *


## C STUFF 
so_file = "./board.so"

c_funcs = CDLL(so_file)
c_funcs.hax.restype = c_funcs.hax.restype = c_char_p


## C STUFF 

keyboard = Controller()
move_sleep = .5
url = sys.argv[1]
prev = []
res = []
board = chess.Board()
moves = 1
turn = True

time.sleep(1)
# Handle white first
html = urlopen(url).read().decode("utf-8")
white_player = re.findall(r"\[White &quot;(.*?)&quot;\]", html)
white = True if "Konradode" in white_player else False
depth = int(sys.argv[2])
if white:
    print("WE ARE WHITE HERE")
    #best = get_best_move(board, 4) #board.push(best)

    # CSTUFF #####
    c_ret = c_funcs.hax(bytes(board.fen(), encoding="utf-8"), 1 if white else -1, depth)
    best = str(c_ret)[2:-1]
    print("best shit: ", best)

    # ########

    print(f"best move: ::: :  {best}")
    for c in str(best):
        keyboard.press(c)
        keyboard.release(c)
        time.sleep(move_sleep)
    #keyboard.type(str(best))
    print(board.board_fen())
    print(board)
    moves += 1


while True:
    time.sleep(.1)
    html = urlopen(url).read().decode("utf-8")

    res = re.search(r"\[Termination &quot;Unterminated&quot;\](.|\n|\r)*?</div>", html).group(0).split()

    if len(res) == 3:
        print("waiting for move..")
        continue

    res = res[2:-1]
    if res != prev:
        move = res[-1]
        #print(prev)
        #print(res)
        for r in res[len(prev):]:
            if (not r[0].isdigit()):
                print("moving: ", r)
                board.push_san(r)
                moves += 1

        if moves % 2 == 0:
            #best = get_best_move(board)


            print(board.fen())
            # CSTUFF #####
            #best = get_best_move(board, 4)

            c_ret = c_funcs.hax(bytes(board.fen(), encoding="utf-8"), 1 if white else -1, depth)
            best = str(c_ret)[2:-1]

            # ########


            #print("black moving")
            #board.push(best)
            print(f"best move: ::: :  {best} struuudel")
            for c in str(best):
                #print("typyng shit here... ", c)
                keyboard.press(c)
                keyboard.release(c)
                time.sleep(move_sleep)
            #keyboard.type(str(best))
            #print(board.board_fen())
            #print(board)


    prev = res

