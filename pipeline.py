def gstreamer_pipeline(
    capture_width=1640,
    capture_height=1232,
    sensor_mode=3,
    flip_method=0,
):
    framerate = 20
    display_width = 640
    display_height = 480

    return (
        "nvarguscamerasrc sensor-id=0 sensor-mode=%d ! "
        "video/x-raw(memory:NVMM), "
        "width=(int)%d, height=(int)%d, "
        "format=(string)NV12, framerate=(fraction)%d/1 ! "
        "nvvidconv flip-method=%d ! "
        "video/x-raw, width=(int)%d, height=(int)%d, format=(string)BGRx ! "
        "videoconvert ! "
        "video/x-raw, format=(string)BGR ! appsink"
        % (
            sensor_mode,
            capture_width,
            capture_height,
            framerate,
            flip_method,
            display_width,
            display_height,
        )
    )

print(gstreamer_pipeline())