from __future__ import print_function
import cv2 as cv
import argparse
import nanocamera as nano

font = cv.FONT_HERSHEY_SIMPLEX
prev_center = None
prev_w = 0
prev_h = 0

def detectAndDisplay(frame):
    frame_gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    frame_gray = cv.equalizeHist(frame_gray)

    center = None
    out_w = 600
    out_h = 600

    #-- Detect faces
    faces = face_cascade.detectMultiScale(frame_gray)
    for (x,y,w,h) in faces:
        center = (x + w//2, y + h//2)
        frame = cv.ellipse(frame, center, (w//2, h//2), 0, 0, 360, (255, 0, 255), 4)

        out_w = w
        out_h = h

        faceROI = frame_gray[y:y+h,x:x+w]
        #-- In each face, detect eyes
        eyes = eyes_cascade.detectMultiScale(faceROI)
        for (x2,y2,w2,h2) in eyes:
            eye_center = (x + x2 + w2//2, y + y2 + h2//2)
            radius = int(round((w2 + h2)*0.25))
            frame = cv.circle(frame, eye_center, radius, (255, 0, 0 ), 4)

    if center == None:
        center = prev_center
        frame = cv.ellipse(frame, center, (prev_w//2, prev_h//2), 0, 0, 360, (255, 0, 255), 4)
        
    locn = str(center)

    cv.putText(frame,locn,(10,500), font, 2, (255, 255, 255), 2, cv.LINE_AA)

    cv.imshow('Capture - Face detection', frame)

    return center, out_w, out_h

parser = argparse.ArgumentParser(description='Code for Cascade Classifier tutorial.')
parser.add_argument('--face_cascade', help='Path to face cascade.', default='haarcascade_frontalface_alt.xml')
parser.add_argument('--eyes_cascade', help='Path to eyes cascade.', default='haarcascade_eye_tree_eyeglasses.xml')
parser.add_argument('--camera', help='Camera divide number.', type=str, default="/dev/video2")
args = parser.parse_args()

face_cascade_name = args.face_cascade
eyes_cascade_name = args.eyes_cascade

face_cascade = cv.CascadeClassifier()
eyes_cascade = cv.CascadeClassifier()

#-- 1. Load the cascades
if not face_cascade.load(cv.samples.findFile(face_cascade_name)):
    print('--(!)Error loading face cascade')
    exit(0)
if not eyes_cascade.load(cv.samples.findFile(eyes_cascade_name)):
    print('--(!)Error loading eyes cascade')
    exit(0)

camera_device = args.camera
#-- 2. Read the video stream
# cap = nano.Camera(flip=0)
cap = cv.VideoCapture('/dev/video3')
# if not cap.isOpened:
#     print('--(!)Error opening video capture')
#     exit(0)

while True:
    _, frame = cap.read()
    if frame is None:
        print('--(!) No captured frame -- Break!')
        break

    prev_center, prev_w, prev_h = detectAndDisplay(frame)

    if cv.waitKey(10) == 27:
        break
