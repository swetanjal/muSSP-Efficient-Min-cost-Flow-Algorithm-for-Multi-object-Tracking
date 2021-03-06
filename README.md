# muSSP-Efficient-Min-cost-Flow-Algorithm-for-Multi-object-Tracking

# Team Members:
- Teja Dhondu
- Swetanjal Dutta
- Nishanth Sharma

## Run demo as follows:
```bash
>> python3 create_graph.py input_detections_folder graph_file
>> ./a.out -i graph_file
>> python3 create_demo.py input_detections_folder output_detections_folder Shortest_Paths.txt input_frames_folder output_frames_folder output_video_path path_to_output_file .png(or .jpg depending on what format input frames are in)
```
## Evaluating using MOT Challenge Dataset Detections:
```
>> python3 format_detections path_to_det.txt input_detections_folder
```
- After which, follow steps as mentioned in previous section.

## Preparing demo data:
- Extract all the frames (using software like ffmpeg) and put them in input_frames_folder
- Run a detector like Yolo and store the detections in input_detections_folder. Each image in input_frames_folder has a corresponding detection file with the same name as the image but ending with .txt extension. Every line inside the detection file has a detection in the following format [x_min, y_min, x_max, y_max, class]. Class is 0 for person. 
- Create empty output_detections_folder and output_frames_folder. 
- Run commands as mentioned in previous section.

## Demo:
- https://www.youtube.com/watch?v=53r4WUOPvPA
- https://www.youtube.com/watch?v=wwhGrDFN1W8
- https://www.youtube.com/watch?v=eQBFRRduv1s
- https://www.youtube.com/watch?v=pisYGk1Sumo
- https://www.youtube.com/watch?v=4LlgBuAjaFg
