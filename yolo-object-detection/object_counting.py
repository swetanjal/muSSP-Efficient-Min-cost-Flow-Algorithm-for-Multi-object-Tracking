#!/usr/bin/env python
# coding: utf-8

# In[1]:


import os
import cv2
import copy
import time
import imutils
import argparse
import numpy as np


class ObjectDet:
    '''class for doing object detection'''
    
    def __init__(self, yolo_path,confidence, threshold):
        self.labelsPath = os.path.sep.join([yolo_path, "coco.names"])
        # derive the paths to the YOLO weights and model configuration
        self.weightsPath = os.path.sep.join([yolo_path, "yolov3.weights"])
        self.configPath = os.path.sep.join([yolo_path, "yolov3.cfg"])
        self.LABELS = open(self.labelsPath).read().strip().split("\n")
        self.REQUIRED_LABELS = ['person']
        self.OBJECTS_COUNT_VIDEO = {}
        self.OBJECT_COUNT = {}
        self.confidence = confidence
        self.threshold = threshold
        self.COLORS = None
        self.net = None
        self.ln = None

    def initialize(self):
        '''initialize the network,video capture object and other object_count dictionaries'''
        for lab in self.REQUIRED_LABELS:
            self.OBJECT_COUNT[lab] = 0
        np.random.seed(42)
        self.COLORS = np.random.randint(0, 255, size=(len(self.LABELS), 3),
            dtype="uint8")
        print("[INFO] loading YOLO from disk...")
        self.net = cv2.dnn.readNetFromDarknet(self.configPath, self.weightsPath)
        self.ln = self.net.getLayerNames()
        self.ln = [self.ln[i[0] - 1] for i in self.net.getUnconnectedOutLayers()]
    
    
    def get_objects(self, image_name):
        self.initialize()
        cnt=0
        (W, H) = (None, None)
        for lab in self.REQUIRED_LABELS:
            self.OBJECT_COUNT[lab] = 0
        frame = cv2.imread(image_name)

        if W is None or H is None:
            (H, W) = frame.shape[:2]

        blob = cv2.dnn.blobFromImage(frame, 1 / 255.0, (416, 416),
            swapRB=True, crop=False)
        self.net.setInput(blob)
        start = time.time()
        layerOutputs = self.net.forward(self.ln)
        end = time.time()
        boxes = []
        confidences = []
        classIDs = []
        cnt = 0

        for output in layerOutputs:
            # loop over each of the detections
            for detection in output:
                scores = detection[5:]
                classID = np.argmax(scores)
                confidence = scores[classID]

                if confidence > self.confidence and self.LABELS[classID] in self.REQUIRED_LABELS:
                    box = detection[0:4] * np.array([W, H, W, H])
                    (centerX, centerY, width, height) = box.astype("int")

                    x = int(centerX - (width / 2))
                    y = int(centerY - (height / 2))
                    boxes.append([x, y, int(width), int(height)])
                    # print(width, height, frame[int(y-height/2):int(y+height/2), int(x-width/2):int(x+width/2)].shape)
                    tmp = frame[int(y-height/2):int(y+height/2), int(x-width/2):int(x+width/2)].shape
                    if tmp[0] != 0 and tmp[1] != 0:
                        cnt += 1
                        cv2.imwrite('img'+str(cnt)+'.jpg', frame[int(y-height/2):int(y+height/2), int(x-width/2):int(x+width/2)])
                    # cv2.imshow('ImageWindow', frame[int(x-width/2):int(x+width/2), int(y-height/2):int(y+height/2)])
                    # cv2.waitKey(500)
                    # print(frame[int(x-width/2):int(x+width/2), int(y-height/2):int(y+height/2)])
                    confidences.append(float(confidence))
                    classIDs.append(classID)

            idxs = cv2.dnn.NMSBoxes(boxes, confidences, self.confidence,
                self.threshold)

        #     if len(idxs) > 0:
        #         for i in idxs.flatten():
        #             self.OBJECT_COUNT[self.LABELS[classIDs[i]]] += 1

        #     self.OBJECTS_COUNT_VIDEO['frame'+str(cnt)] = copy.copy(self.OBJECT_COUNT)
        # return self.OBJECTS_COUNT_VIDEO
        return 


# arg1 - video path
# arg2 - yolo directory path
# arg3 - confidence(minimum probability to filter weak detections)
# arg4 - threshold(threshold when applyong non-maxima suppression)
# arg5 - number of frames to check per second
# yolo files link - https://drive.google.com/drive/folders/1id-cCLutIXJ128QBZtvrvVtCksMl7zdi?usp=sharing
objdet = ObjectDet('yolo-coco', 0.5, 0.3)
objdet.get_objects('000001.jpg')


