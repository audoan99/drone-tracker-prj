#!/usr/bin/env python3
import os
import subprocess
from ultralytics import YOLO

# 1. Tải model YOLOv8n (Nano - cực nhẹ, phù hợp GTX 1080 Ti)
print("🚀 Downloading YOLOv8n from Ultralytics...")
model = YOLO('yolov8n.pt')

# 2. Export sang ONNX (FP32)
print("📦 Exporting to ONNX (FP32)...")
# Lưu ý: tham số 'half=False' đảm bảo export ở định dạng FP32
onnx_path = model.export(format='onnx', imgsz=640, half=False, simplify=True)
onnx_file = os.path.abspath(onnx_path)

# 3. Dùng trtexec để convert ONNX -> TensorRT Engine
print("⚙️ Building TensorRT Engine for GTX 1080 Ti (FP32 - Default)...")
engine_file = onnx_file.replace('.onnx', '_fp32.engine')

# Sửa lỗi: Bỏ '--fp32' (vì là mặc định) và dùng '--memPoolSize' thay cho '--workspace'
trtexec_cmd = [
    "/usr/src/tensorrt/bin/trtexec",
    f"--onnx={onnx_file}",
    f"--saveEngine={engine_file}",
    "--memPoolSize=workspace:4096",  # Cấp 4GB bộ nhớ cho quá trình build
    "--avgRuns=10"
]

print(f"Running command: {' '.join(trtexec_cmd)}")
subprocess.run(trtexec_cmd, check=True)

# 4. Di chuyển file vào thư mục resource (đã được mount từ host)
target_dir = "/app/resource/models"
os.makedirs(target_dir, exist_ok=True)

import shutil
shutil.move(onnx_file, os.path.join(target_dir, "yolov8n.onnx"))
shutil.move(engine_file, os.path.join(target_dir, "yolov8n_fp32.engine"))

# 5. Tạo file labels.txt (80 classes của COCO)
labels = ["person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", 
          "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", 
          "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", 
          "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", 
          "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", 
          "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", 
          "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", 
          "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", 
          "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", 
          "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"]

with open(os.path.join(target_dir, "labels.txt"), "w") as f:
    f.write("\n".join(labels))

print(f"✅ Done! Model and labels saved to {target_dir}")