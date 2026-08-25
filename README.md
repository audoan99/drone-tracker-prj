# drone-tracker-prj: DeepStream Detection & Tracking Pipeline

A high-performance, containerized video analytics pipeline built with **NVIDIA DeepStream SDK**, **TensorRT**, and **GStreamer**. Designed for real-time object detection, hardware tracking, and live RTSP video streaming, with seamless cross-platform deployment from x86_64 Development Host GPUs to ARM64 Edge AI hardware (**NVIDIA Jetson Orin NX**).

---

## Key Features

- **Hardware-Accelerated Inference**: Powered by TensorRT FP16/INT8 optimization for ultra-low latency processing.
- **Object Detection & Tracking**: Integrates Primary GIE (Custom YOLO/ResNet) with hardware-accelerated tracking (`nvtracker` / NvDCF).
- **Live RTSP Output**: Broadcasts processed video with drawn bounding boxes and labels over RTSP.
- **Modular C++ Architecture**: Clean, single-responsibility C++ codebase with dedicated metadata probes for downstream integration (ROS 2 / MAVLink).
- **Dual-Architecture Docker Setup**: Ready for both x86_64 (Development) and ARM64 (Jetson Production).

---

## Project Structure

```text
drone-tracker-prj/
├── CMakeLists.txt                 # C++ build configuration
├── config/
│   ├── deepstream_app_config.txt  # Main pipeline orchestrator (for deepstream-app)
│   ├── config_infer_primary.txt   # TensorRT model inference parameters (PGIE)
│   └── tracker_config.yml         # NvDCF hardware tracker configuration
├── deploy/
│   ├── Dockerfile                 # Container config for x86_64 Host (DeepStream 6.1.1)
│   └── Dockerfile.jetson          # Container config for Jetson Orin NX (L4T)
├── resource/                      # (Optional) Place custom .engine models and videos here
├── src/                           # Modular C++ Application Source Code
│   ├── main.cpp                   # Entry point & GStreamer main loop
│   ├── pipeline.h / pipeline.cpp  # Pipeline orchestrator & element linking
│   ├── source.h / source.cpp      # uridecodebin & streammux logic
│   ├── inference.h / inference.cpp# nvinfer (PGIE) configuration
│   ├── tracker.h / tracker.cpp    # nvtracker configuration
│   ├── osd.h / osd.cpp            # nvdsosd & metadata probe attachment
│   ├── sink.h / sink.cpp          # Encoding chain & RTSP sink (nvrtspout)
│   └── probe.h / probe.cpp        # Metadata extraction callback (ROS/MAVLink ready)
├── LICENSE
└── README.md
```

## Development guideline

### Step 1: Build Docker Image
docker build -f deploy/Dockerfile -t drone-tracker:dev .

### Step 2: Run Command for Test
docker run --runtime=nvidia -it --rm --network host \
    -v $(pwd):/app \
    --name drone_dev \
    drone-tracker:latest /bin/bash

### Step 3: Build C++ Application
mkdir -p build && cd build
cmake ..
make -j$(nproc)

### Step 4: Run Application
./drone_tracker ../config/deepstream_app_config.txt

## Deployment guideline

### Step 1: Build Docker Image
docker build -f deploy/Dockerfile -t drone-tracker:prod .

### Step 2: Run Command for Deploy
docker run --gpus all -d --network host --name drone_prod \
    -v $(pwd)/config:/app/config \
    drone-tracker:prod

### Step 3: Build & Run App
docker logs -f drone_prod

## Visual Test: Open VLC Media Player → Open Network Stream → Enter: rtsp://<YOUR_SERVER_IP>:8554/ds-test.

## Jetson Orin NX (ARM64) Deployment

### Step 1: Build Image on the Jetson device
docker build -f deploy/Dockerfile.jetson -t drone-tracker:jetson .

### Step 2: Run Deploy (Note the --runtime nvidia flag for Jetson)
docker run --runtime nvidia -d --network host --name drone_jetson \
    -v $(pwd)/config:/app/config \
    drone-tracker:jetson

### Step 3: View Logs
docker logs -f drone_jetson

## Testing & Verification Guide

### Test 1: Verify Terminal Logs (Metadata Extraction)
Expected Output: Look for the startup message *** Pipeline Running ***, followed by continuous lines like:
```
  [Drone Tracker] Frame: 142 | ID: 5 | Class: 0 | Conf: 0.85
  [Drone Tracker] Frame: 143 | ID: 5 | Class: 0 | Conf: 0.88
```
If it fails: Check that config_infer_primary.txt points to a valid .engine file and that the threshold isn't set too high.

### Test 2: Verify RTSP Video Stream (Visual Bounding Boxes)
- Open VLC Media Player on your local machine.
- Go to Media → Open Network Stream (Ctrl + N).
- Enter the URL: rtsp://<YOUR_SERVER_IP>:8554/ds-test (Replace <YOUR_SERVER_IP> with the IP of your host machine, or use localhost if running locally).
- Expected Output: You should see the video playing with bounding boxes and tracking IDs drawn over the detected objects.
- Pro-Tip for Low Latency: In VLC, go to Tools > Preferences > Input / Codecs and set Network caching to 100 or 200 ms. Alternatively, use ffplay for the lowest possible latency:
```
   ffplay -fflags nobuffer -flags low_delay -framedrop rtsp://<YOUR_SERVER_IP>:8554/ds-test
```
