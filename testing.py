import cv2

cap1 = cv2.VideoCapture("/dev/video0")
cap2 = cv2.VideoCapture("/dev/video1")

while True:
    _, frame = cap1.read()
    cv2.imshow("1", frame)
    _, frame2 = cap2.read()
    cv2.imshow("2", frame2)

