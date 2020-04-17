# python3 create_demo.py detections_folder output_detections_folder extracted_paths_file input_frame_folder output_frame_folder
import os
import numpy as np
import sys
import copy
import cv2

edges = []
flow = []
for i in range(100005):
    edges.append([])
    flow.append([])

files = os.listdir(sys.argv[1])
files.sort()
frame = []
bbox = []
pre_node = []
post_node = []
prev_idx = []
# Source is node 1
cnt = 2
edge_cnt = 0

for f in files:
    FIL = open(sys.argv[1] + '/' + f, "r")
    lines = FIL.readlines()
    curr = []
    for line in lines:
        # Get coordinates of current bbox
        if int(line.split()[4]) != 0:
            continue
        frame.append(f.replace('.txt', ''))
        x_min = int(line.split()[0])
        y_min = int(line.split()[1])
        x_max = int(line.split()[2])
        y_max = int(line.split()[3])
        # Appending bbox, pre-node number and post-node number
        
        bbox.append([x_min, y_min, x_max, y_max])
        pre_node.append(cnt)
        post_node.append(cnt + 1)
        # Add edge from post node i in previous frame to current pre-node
        for idx in prev_idx:
            edge_cnt = edge_cnt + 1
            edges[post_node[idx]].append(cnt)
            flow[post_node[idx]].append(0)
        # Add edge between pre node and post node
        edge_cnt += 1
        edges[cnt].append(cnt + 1)
        flow[cnt].append(0)
        
        # Add idx to current frame indices
        curr.append(len(pre_node) - 1) 
        
        # Update node number
        cnt = cnt + 2 
    prev_idx = copy.deepcopy(curr)

for i in pre_node:
    edge_cnt += 1
    edges[1].append(i)
    flow[1].append(0)
for i in post_node:
    edge_cnt += 1
    edges[i].append(cnt)
    flow[i].append(0)

FILE = open(sys.argv[3])
FILE = FILE.readlines()
for line in FILE:
    toks = line.split()
    for i in range(1, len(toks)):
        u = int(toks[i - 1])
        v = int(toks[i])
        # Find an edge from u to v
        for j in range(len(edges[u])):
            if edges[u][j] == v:
                flow[u][j] += 1
        for j in range(len(edges[v])):
            if edges[v][j] == u:
                flow[v][j] -= 1

color = 0
codes = [[0, 0, 0], [255, 0, 0], [0, 255, 0], [0, 0, 255]]

output_folder = sys.argv[2]

for k in frame:
    t = open(output_folder + '/' + k + '.txt', 'w')
    t.write("")

def dfs(node):
    global color
    l = len(edges[node])
    for i in range(l):
        if flow[node][i] > 0:
            if (node % 2) == 0:
                for j in range(len(pre_node)):
                    if pre_node[j] == node:
                        t = open(output_folder + '/' + frame[j] + '.txt', 'a')
                        t.write(str(bbox[j][0]) + " " + str(bbox[j][1]) + " " + str(bbox[j][2]) + " " + str(bbox[j][3]) + " " + str(codes[color][0]) + " " + str(codes[color][1]) + " " + str(codes[color][2]) + "\n")
            dfs(edges[node][i])
        if node == 1:
            # print("**************************")
            color += 1
            color = color % len(codes)

dfs(1)

files = os.listdir(sys.argv[4])
for f in files:
    img = cv2.imread(sys.argv[4] + '/' + f)
    cv2.imwrite(sys.argv[5] + '/' + f, img)

for name in frame:
    f = open(output_folder + '/' + name + '.txt')
    f = f.readlines()
    img = cv2.imread(sys.argv[4] + '/' + name)
    for line in f:
        tok = line.split()
        color = (int(tok[4]), int(tok[5]), int(tok[6]))
        img = cv2.rectangle(img, (int(tok[0]), int(tok[1])), (int(tok[2]), int(tok[3])), color, 5)
    cv2.imwrite(sys.argv[5] + '/' + name, img)