import numpy as np
import cv2


a = np.zeros((1920,1080))


cv2.circle(a, (50,50),50, (255, 0, 0), -1)

list_of = np.transpose(np.nonzero(a))

for i in list_of:
    print(i)
    if(i[0] == 50 and i[1] == 50):
        cv2.circle(a, (i[0],i[1]), radius=0, color=100, thickness=-1)

cv2.imwrite("test.png",a)



