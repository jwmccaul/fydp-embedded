import cv2
import numpy as np
import time
 
#begin streaming
cap = cv2.VideoCapture(0)
while True:
    _, frame = cap.read()
 
    #convert frame to monochrome and blur
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    blur = cv2.GaussianBlur(gray, (9,9), 0)
 
    #use function to identify threshold intensities and locations
    (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(blur)
 
    #threshold the blurred frame accordingly
    hi, threshold = cv2.threshold(blur, maxVal-1, 255, cv2.THRESH_BINARY)
    thr = threshold.copy()
 
    #resize frame for ease
    cv2.resize(thr, (300,300))

    #find contours in thresholded frame
    edged = cv2.Canny(threshold, 50, 150)
    lightcontours, hierarchy = cv2.findContours(edged, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    #attempts finding the circle created by the torch illumination on the wall
    circles = cv2.HoughCircles(threshold, cv2.HOUGH_GRADIENT, 1.0, 20,
                            param1=10,
                            param2= 15,
                            minRadius=10,
                            maxRadius=100,)
     #check if the list of contours is greater than 0 and if any circles are detected
    # if(len(lightcontours) > 0):
    #     print("LIGHT CONTOURS", len(lightcontours))
    # if(circles is not None):
    #     print("CIRCLES", circles)
    if len(lightcontours)>0: ## and circles is not None:
        #Find the Maxmimum Contour, this is assumed to be the light beam
        maxcontour = max(lightcontours, key=cv2.contourArea)

        #avoids random spots of brightness by making sure the contour is reasonably sized
        if cv2.contourArea(maxcontour) > 1000:
            (x, final_y), radius = cv2.minEnclosingCircle(maxcontour)
            cv2.circle(frame, (int(x), int(final_y)), int(radius), (0, 255, 0), 4)
            # cv2.rectangle(frame, (int(x) - 5, int(final_y) - 5), (int(x) + 5, int(final_y) + 5), (0, 128, 255), -1)
    
    #display frames and exit
    cv2.imshow('light', thr)
    cv2.imshow('frame', frame)
    cv2.waitKey(4)
    key = cv2.waitKey(5) & 0xFF
    if key == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()