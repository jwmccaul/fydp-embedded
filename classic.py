# import the necessary packages
import numpy as np
import argparse
import cv2


NUM_ROWS = 5
NUM_COLS = 11

def getIntervals(frame_width, frame_height):
    rowIntervals = np.round(np.linspace(0, frame_height, NUM_ROWS+1)).astype(int)
    colIntervals = np.round(np.linspace(0, frame_width, NUM_COLS+1)).astype(int)

    return rowIntervals, colIntervals



def getRowAndCol(maxLoc, radius, rowIntervals, colIntervals, frame_width, frame_height):
    zero_mask = np.zeros((frame_height,frame_width))
    cv2.circle(zero_mask, maxLoc, radius, (255, 0, 0), -1)

    # cv2.imwrite("test.png",zero_mask)


    coords_in_circle = np.transpose(np.nonzero(zero_mask))
    rows_to_be_on = [0 for i in range(NUM_ROWS)]
    cols_to_be_on = [0 for i in range(NUM_COLS)]

    for coord in coords_in_circle:
        for i in range(len(rowIntervals) - 1):
            if(coord[0] > rowIntervals[i] and coord[0] <= rowIntervals[i+1]):
               rows_to_be_on[i] = 1
        for j in range(len(colIntervals) - 1):
            if(coord[1] > colIntervals[j] and coord[1] <= colIntervals[j+1]):
                cols_to_be_on[j] = 1

    return rows_to_be_on, cols_to_be_on

            

def detectAndDisplay(frame, frame_width, frame_height, rowIntervals, colIntervals):
    maxLocAboveThresh = -1
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # apply a Gaussian blur to the image then find the brightest region
    # gray = cv2.medianBlur(gray, args["radius"])
    gray = cv2.GaussianBlur(gray, (args["radius"], args["radius"]), 0)

    (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray)
    # (mean, stddev) = cv2.meanStdDev(gray)
    image = frame.copy()

    for i in rowIntervals:
        cv2.line(image, (0,i), (frame_width, i), (255,0,0), 2)
    
    for j in colIntervals:
        cv2.line(image, (j,0), (j, frame_height), (255,0,0), 2)



    if(maxVal >= args["threshhold"]):
        maxLocAboveThresh = maxLoc
        cv2.circle(image, maxLoc, args["radius"], (0, 0, 255), 2)


    # print("MAX VAL", maxVal)
    # display the results of our newly improved method
    cv2.imshow("Robust", image)
    return maxLocAboveThresh, image

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
#ap.add_argument("-i", "--image", help = "path to the image file")
ap.add_argument("-r", "--radius", type = int, help = "radius of Gaussian blur; must be odd", default=51)
ap.add_argument("-t", "--threshhold", type = int, help = "Minimum threshhold value to activate (0-255)", default=250)
ap.add_argument('--camera', help='Camera divide number.', type=int, default=0)

args = vars(ap.parse_args())

camera_device = args["camera"]

cap = cv2.VideoCapture('/dev/video2')
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc("M", "J", "P", "G"))
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)



if not cap.isOpened():
    print('--(!)Error opening video capture')
    exit(0)

frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))


rowIntervals, colIntervals = getIntervals(frame_width, frame_height)
print(rowIntervals, colIntervals)

print("Width", frame_width)
print("Height", frame_height)

while True:
    ret, frame = cap.read()
    
    if frame is None:
        print('--(!) No captured frame -- Break!')
        break

    maxLoc, image = detectAndDisplay(frame, frame_width, frame_height, rowIntervals, colIntervals)

    if(maxLoc != -1):
        rows_to_be_on, cols_to_be_on = getRowAndCol(maxLoc, args["radius"], rowIntervals, 
        colIntervals, frame_width, frame_height)

        print("ROWS TO BE ON", rows_to_be_on)
        print("COLS TO BE ON", cols_to_be_on)
    if cv2.waitKey(5) == 27:
        break


cv2.destroyAllWindows()
