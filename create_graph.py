# python3 create_graph.py detections_folder output_file
import os
import numpy as np
import sys
import copy

def search(A, val):
    l = 0; h = len(A) - 1
    while l <= h:
        m = (l + h) // 2
        if A[m] == val:
            return m
        elif A[m] < val:
            l = m + 1
        else:
            h = m - 1
    assert(False)

def IoU(box1, box2):
    """
    calculate intersection over union cover percent
    :param box1: box1 with shape (N,4) or (N,2,2) or (2,2) or (4,). first shape is preferred
    :param box2: box2 with shape (N,4) or (N,2,2) or (2,2) or (4,). first shape is preferred
    :return: IoU ratio if intersect, else 0
    """
    # first unify all boxes to shape (N,4)
    if box1.shape[-1] == 2 or len(box1.shape) == 1:
        box1 = box1.reshape(1, 4) if len(box1.shape) <= 2 else box1.reshape(box1.shape[0], 4)
    if box2.shape[-1] == 2 or len(box2.shape) == 1:
        box2 = box2.reshape(1, 4) if len(box2.shape) <= 2 else box2.reshape(box2.shape[0], 4)
    point_num = max(box1.shape[0], box2.shape[0])
    b1p1, b1p2, b2p1, b2p2 = box1[:, :2], box1[:, 2:], box2[:, :2], box2[:, 2:]

    # mask that eliminates non-intersecting matrices
    base_mat = np.ones(shape=(point_num,))
    base_mat *= np.all(np.greater(b1p2 - b2p1, 0), axis=1)
    base_mat *= np.all(np.greater(b2p2 - b1p1, 0), axis=1)

    # I area
    intersect_area = np.prod(np.minimum(b2p2, b1p2) - np.maximum(b1p1, b2p1), axis=1)
    # U area
    union_area = np.prod(b1p2 - b1p1, axis=1) + np.prod(b2p2 - b2p1, axis=1) - intersect_area
    # IoU
    intersect_ratio = intersect_area / union_area

    return (base_mat * intersect_ratio)[0]

edges = []
cost = []
for i in range(100005):
    edges.append([])
    cost.append([])
files = os.listdir(sys.argv[1])
files.sort()
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
        x_min = float(line.split()[0])
        y_min = float(line.split()[1])
        x_max = float(line.split()[2])
        y_max = float(line.split()[3])
        # Appending bbox, pre-node number and post-node number
        bbox.append([x_min, y_min, x_max, y_max])
        pre_node.append(cnt)
        post_node.append(cnt + 1)
        # Add edge from post node i in previous frame to current pre-node
        for idx in prev_idx:
            edge_cnt = edge_cnt + 1
            edges[post_node[idx]].append(cnt)
            cost[post_node[idx]].append(-IoU(np.array(bbox[idx]), np.array([x_min, y_min, x_max, y_max])))
        # Add edge between pre node and post node
        edge_cnt += 1
        edges[cnt].append(cnt + 1)
        cost[cnt].append(0)
        
        # Add idx to current frame indices
        curr.append(len(pre_node) - 1) 
        
        # Update node number
        cnt = cnt + 2 
    prev_idx = copy.deepcopy(curr)

for i in pre_node:
    edge_cnt += 1
    edges[1].append(i)
    cost[1].append(0)
for i in post_node:
    edge_cnt += 1
    edges[i].append(cnt)
    cost[i].append(0)

OUT = open(sys.argv[2], "w")
OUT.write("p min " + str(cnt) + " " + str(edge_cnt) + "\n")

for i in range(2, cnt, 2):
    # Source node to pre-node
    j = search(edges[1], i)
    OUT.write("a 1" + " " + str(edges[1][j]) + " " + str(cost[1][j]) + "\n")
    # Pre node to post node
    j = search(edges[i], i + 1)
    OUT.write("a " + str(i) + " " + str(edges[i][j]) + " " + str(cost[i][j]) + "\n")
    # Post node to sink
    j = search(edges[i + 1], cnt)
    OUT.write("a " + str(i + 1) + " " + str(edges[i + 1][j]) + " " + str(cost[i + 1][j]) + "\n")

for i in range(3, cnt, 2):
    for j in range(len(edges[i])):
        if edges[i][j] != cnt:
            OUT.write("a " + str(i) + " " + str(edges[i][j]) + " " + str(cost[i][j]) + "\n")