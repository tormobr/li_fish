import time
from matplotlib import pyplot as plt
import numpy as np
import cv2
import pyautogui 

pieces = ["pawnb.jpg",
        "pawnw.jpg",
        "bishb.jpg",
        "bishw.jpg",
        "rookw.jpg",
        "rookb.jpg",
        "horseb.jpg",
        "horsew.jpg",
        "kingw.jpg",
        "kingb.jpg",
        "queenb.jpg",
        "queenw.jpg"]

time.sleep(1)
image = pyautogui.screenshot()
image = cv2.cvtColor(np.array(image),cv2.COLOR_RGB2GRAY)

blur = cv2.GaussianBlur(image, (5,5), 0)
cv2.imshow("blur", blur)
#cv2.waitKey(0)
#cv2.destroyAllWindows()

_, thresh = cv2.threshold(blur,50,255,cv2.THRESH_BINARY)
cv2.imshow("thresh", thresh)
#cv2.waitKey(0)
#cv2.destroyAllWindows()

contours, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

# Find the largest contour
conts = {i:c for i,c in enumerate(contours)}
best = max(conts.items(), key=lambda x: cv2.contourArea(x[1]))
best_i, best_cnt = best
image = cv2.drawContours(image, contours, best_i, (0, 255, 0), 3)

mask = np.zeros((image.shape),np.uint8)
cv2.drawContours(mask,[best_cnt],0,255,-1)
cv2.drawContours(mask,[best_cnt],0,0,2)
cv2.imshow("mask", mask)
#cv2.waitKey(0)
#cv2.destroyAllWindows()

out = np.zeros_like(image)
out[mask == 255] = image[mask == 255]

(y, x) = np.where(mask == 255)
(topy, topx) = (np.min(y), np.min(x))
(bottomy, bottomx) = (np.max(y), np.max(x))
out = out[topy:bottomy+1, topx:bottomx+1]
cv2.imshow("out", out)
cv2.waitKey(0)
cv2.destroyAllWindows()

w = out.shape[0] // 8
h = out.shape[1] // 8
for i in range(8):
    for j in range(8):
        tmp = out[h*i:h*(i+1), w*j:w*(j+1)]
        tmp = tmp[5:-2, 5:-1]
        _, thresh = cv2.threshold(tmp,110,255,cv2.THRESH_BINARY)
        winner = 0
        for p in pieces:
            pimage = cv2.imread(p, 0)


        #cv2.imshow("piece", thresh)
        #cv2.imwrite("piece"+str(i)+str(j)+ ".jpg", thresh)
