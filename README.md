# muSSP-Efficient-Min-cost-Flow-Algorithm-for-Multi-object-Tracking

## Run demo as follows:
```bash
>> python3 create_demo.py input_detections_folder graph_file
>> ./a.out -i graph_file
>> python3 create_demo.py input_detections_folder output_detections_folder Shortest_Paths.txt input_frames_folder output_frames_folder output_video_path
```

## Preparing demo data:
- Extract all the frames (using software like ffmpeg) and put them in input_frames_folder
- Run a detector like Yolo and store the detections in input_detections_folder. Each image in input_frames_folder has a corresponding detection file with the same name as the image but ending with .txt extension. Every line inside the detection file has a detection in the following format [x_min, y_min, x_max, y_max, class]. Class is 0 for person. 
- Create empty output_detections_folder and output_frames_folder. 
- Run commands as mentioned in previous section.