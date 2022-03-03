# import the necessary packages
import numpy as np
import argparse
import cv2

from helperClasses import VideoGet, RowAndColThread, VideoShow

NUM_ROWS = 5
NUM_COLS = 11

def getIntervals(frame_width, frame_height):
    rowIntervals = np.round(np.linspace(0, frame_height, NUM_ROWS+1)).astype(int)
    colIntervals = np.round(np.linspace(0, frame_width, NUM_COLS+1)).astype(int)

    return rowIntervals, colIntervals

            

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
    # cv2.imshow("Robust", image)
    return maxLocAboveThresh, image

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
#ap.add_argument("-i", "--image", help = "path to the image file")
ap.add_argument("-r", "--radius", type = int, help = "radius of Gaussian blur; must be odd", default=51)
ap.add_argument("-t", "--threshhold", type = int, help = "Minimum threshhold value to activate (0-255)", default=250)
ap.add_argument('--camera', help='Camera divide number.', type=int, default=0)
args = vars(ap.parse_args())

camera_device = args["camera"]

video_getter = VideoGet("/dev/video0").start()
video_shower = VideoShow(video_getter.frame).start()
# video_shower = VideoShow(video_getter.frame).start()
rowAndColCalculator = RowAndColThread()







frame_width = 1280
frame_height = 720

rowIntervals, colIntervals = getIntervals(frame_width, frame_height)
print(rowIntervals, colIntervals)

print("Width", frame_width)
print("Height", frame_height)

while True:
    if video_getter.stopped or video_shower.stopped:
        video_getter.stop()
        video_shower.stop()
        break
    
    frame = video_getter.frame
    video_shower.frame = frame
       
    maxLoc, image = detectAndDisplay(frame, frame_width, frame_height, rowIntervals, colIntervals)


    if(maxLoc != -1):
        rowAndColCalculator.start(maxLoc, args["radius"], rowIntervals, 
        colIntervals, frame_width, frame_height)


cv2.destroyAllWindows()
