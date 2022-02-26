# import the necessary packages
import numpy as np
import argparse
import cv2

def detectAndDisplay(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # apply a Gaussian blur to the image then find the brightest region
    gray = cv2.medianBlur(gray, args["radius"])
    (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray)
    print(maxVal)
    image = frame.copy()
    if (maxVal == 255):
        cv2.circle(image, maxLoc, args["radius"], (255, 0, 0), 2)

    # display the results of our newly improved method
    cv2.imshow("Robust", image)

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
#ap.add_argument("-i", "--image", help = "path to the image file")
ap.add_argument("-r", "--radius", type = int, help = "radius of Gaussian blur; must be odd")
ap.add_argument('--camera', help='Camera divide number.', type=int, default=0)
args = vars(ap.parse_args())

camera_device = args["camera"]
#-- 2. Read the video stream
cap = cv2.VideoCapture(camera_device)
if not cap.isOpened:
    print('--(!)Error opening video capture')
    exit(0)

while True:
    ret, frame = cap.read()
    if frame is None:
        print('--(!) No captured frame -- Break!')
        break

    detectAndDisplay(frame)

    if cv2.waitKey(10) == 27:
        break