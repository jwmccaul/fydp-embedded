import cv2
import numpy as np

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
#begin streaming

def detectLight(frame, ):
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
            return frame, thr, (int(x), int(final_y)), int(radius)
            # cv2.rectangle(frame, (int(x) - 5, int(final_y) - 5), (int(x) + 5, int(final_y) + 5), (0, 128, 255), -1)
    return frame, thr, -1, -1
    

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print('--(!)Error opening video capture')
    exit(0)

frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))



rowIntervals, colIntervals = getIntervals(frame_width, frame_height)


while True:
    _, frame = cap.read()
 
    for i in rowIntervals:
        cv2.line(frame, (0,i), (frame_width, i), (255,0,0), 2)

    for j in colIntervals:
        cv2.line(frame, (j,0), (j, frame_height), (255,0,0), 2)

    frame, thr, maxLoc, radius = detectLight(frame)

    if(maxLoc != -1):
        rows_to_be_on, cols_to_be_on = getRowAndCol(maxLoc, radius, rowIntervals, 
        colIntervals, frame_width, frame_height)

        print("ROWS TO BE ON", rows_to_be_on)
        print("COLS TO BE ON", cols_to_be_on)

    cv2.imshow('light', thr)
    cv2.imshow('frame', frame)
    cv2.waitKey(4)
    key = cv2.waitKey(5) & 0xFF
    if key == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()