# import the necessary packages
import numpy as np
import argparse
import cv2

NUM_ROWS = 5
NUM_COLS = 11

def getIntervals(frame_width, frame_height):
    rowIntervals = np.linspace(0, frame_height, NUM_ROWS)
    colIntervals = np.linspace(0, frame_width, NUM_COLS)

    return rowIntervals, colIntervals



def getRowAndCol(maxLoc, radius, rowIntervals, colIntervals):
    x = maxLoc[0]
    y = maxLoc[1]
    





def detectAndDisplay(frame):
    maxLocAboveThresh = -1
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # apply a Gaussian blur to the image then find the brightest region
    # gray = cv2.medianBlur(gray, args["radius"])
    gray = cv2.GaussianBlur(gray, (args["radius"], args["radius"]), 0)

    (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray)
    # (mean, stddev) = cv2.meanStdDev(gray)
    image = frame.copy()

    if(maxVal >= args["threshhold"]):
        cv2.circle(image, maxLoc, args["radius"], (255, 0, 0), 2)
        maxLocAboveThresh = maxLoc

    print("MAX VAL", maxVal)
    # display the results of our newly improved method
    cv2.imshow("Robust", image)
    return maxLocAboveThresh

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
#ap.add_argument("-i", "--image", help = "path to the image file")
ap.add_argument("-r", "--radius", type = int, help = "radius of Gaussian blur; must be odd")
ap.add_argument("-t", "--threshhold", type = int, help = "Minimum threshhold value to activate (0-255)", default=250)
ap.add_argument('--camera', help='Camera divide number.', type=int, default=0)
args = vars(ap.parse_args())

camera_device = args["camera"]
#-- 2. Read the video stream
cap = cv2.VideoCapture('/dev/video0', cv2.CAP_V4L2)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc("M", "J", "P", "G"))
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)


if not cap.isOpened:
    print('--(!)Error opening video capture')
    exit(0)

frame_width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
frame_height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)

rowIntervals, colIntervals = getIntervals(frame_width, frame_height)
print(rowIntervals, colIntervals)

print("Width", frame_width)
print("Height", frame_height)

while True:
    ret, frame = cap.read()
    if frame is None:
        print('--(!) No captured frame -- Break!')
        break

    maxLoc = detectAndDisplay(frame)

    if(maxLoc != -1):
        getRowAndCol(maxLoc, args["radius"], rowIntervals, colIntervals)


    if cv2.waitKey(10) == 27:
        break