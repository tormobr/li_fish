from collections import deque
import numpy as np

piece_map = {
        "K": 100,
        "Q": 9,
        "R": 5,
        "B": 4,
        "N": 3,
        "P": 1
}
class Board:
    def __init__(self, fen):
        self.grid = np.array([[0 for i in range(8)] for j in range(8)])
        self.history = deque()
        for y,row in enumerate(fen.split("/")):
            x = 0
            for c in row:
                if c.isdigit():
                    x += int(c)
                    continue
                self.grid[y][x] = piece_map[c.upper()]
                if c.islower():
                    self.grid[y][x] *= -1
                x += 1

    def __repr__(self):
        ret = ""
        for row in self.grid:
            for elem in row:
                ret += str(elem) + "  "
            ret += "\n"
        return ret

    def letter_to_index(self, loc):
        x = int(ord(loc[0]) - 97)
        y = int(loc[1]) * -1
        return x, y

    def index_to_letter(self, x, y):
        a = chr(x + 97)
        b = abs(y-8)
        return a+str(b)

    def move(self, fromm, to):
        x0, y0 = self.letter_to_index(fromm)
        x1, y1= self.letter_to_index(to)
        f = self.grid[y0][x0]
        t = self.grid[y1][x1]
        hist_item = ((y0, x0, f), (y1, x1, t))
        self.history.append(hist_item)
        self.grid[y1][x1] = self.grid[y0][x0]
        self.grid[y0][x0] = 0

    def pop(self):
        item = self.history.pop()
        f, t = item
        y0, x0, val_from = f
        y1, x1, val_to = t
        self.grid[y0][x0] = val_from
        self.grid[y1][x1] = val_to
     
if __name__ == "__main__":
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
    b = Board(fen)
    print(b)
    print(b.letter_to_index("e4"))
