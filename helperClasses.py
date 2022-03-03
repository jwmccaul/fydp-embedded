from threading import Thread
import cv2
import numpy as np

class RowAndColThread:        
    def start(self, maxLoc, radius, rowIntervals, colIntervals, frame_width,frame_height):
        Thread(target=self.getRowAndColHelper, args=(maxLoc, radius, rowIntervals, colIntervals, frame_width,
            frame_height)).start()
        return self


    def getRowAndColHelper(self, maxLoc, radius, rowIntervals, colIntervals, frame_width,
            frame_height):
        zero_mask = np.zeros((frame_height,frame_width))
        cv2.circle(zero_mask, maxLoc, radius, (255, 0, 0), -1)

        # cv2.imwrite("test.png",zero_mask)


        coords_in_circle = np.transpose(np.nonzero(zero_mask))
        rows_to_be_on = [0 for i in range(5)]
        cols_to_be_on = [0 for i in range(11)]

        for coord in coords_in_circle:
            for i in range(len(rowIntervals) - 1):
                if(coord[0] > rowIntervals[i] and coord[0] <= rowIntervals[i+1]):
                   rows_to_be_on[i] = 1
            for j in range(len(colIntervals) - 1):
                if(coord[1] > colIntervals[j] and coord[1] <= colIntervals[j+1]):
                    cols_to_be_on[j] = 1

        print(rows_to_be_on, cols_to_be_on)

class VideoGet:
    """
    Class that continuously gets frames from a VideoCapture object
    with a dedicated thread.
    """

    def __init__(self, src=0):
        self.stream = cv2.VideoCapture(src)
        self.stream.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc("M", "J", "P", "G"))
        self.stream.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
        self.stream.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
        self.stream.set(cv2.CAP_PROP_FPS, 30)

        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False
        
    def start(self):
        Thread(target=self.get, args=()).start()
        return self

    def get(self):
        while not self.stopped:
            if not self.grabbed:
                self.stop()
            else:
                (self.grabbed, self.frame) = self.stream.read()

    def stop(self):
        self.stopped = True

class VideoShow:
    """
    Class that continuously shows a frame using a dedicated thread.
    """

    def __init__(self, frame=None):
        self.frame = frame
        self.stopped = False
    def start(self):
        Thread(target=self.show, args=()).start()
        return self

    def show(self):
        while not self.stopped:
            cv2.imshow("Video", self.frame)
            if cv2.waitKey(1) == ord("q"):
                self.stopped = True

    def stop(self):
        self.stopped = True

