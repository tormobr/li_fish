from ctypes import *

so_file = "./board.so"

c_funcs = CDLL(so_file)
c_funcs.main.restype = c_funcs.main.restype = c_char_p

ret = c_funcs.main(b"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
ret = str(ret)[2:-1]
strudel = [{"from":x[:2], "to":x[2:]} for x in ret.split("|")[:-1]]
print(f"ret: {strudel}")
