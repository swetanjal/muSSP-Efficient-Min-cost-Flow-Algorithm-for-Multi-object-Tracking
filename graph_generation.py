import os
import sys
import cv2
import numpy as np
from yolo_object_detection.object_counting import ObjectDet
import triplet_reid.trinet_embed

if __name__ == "__main__":
    object_detection = ObjectDet('./yolo_object_detection/yolo-coco', 0.5, 0.3)
    # objdet.get_objects('000001.jpg')
    directory = "/home/nishanth/Desktop/Academics/Semster VI/Computer Vision/Project/MOT20/train/MOT20-01/img1/"
    output_directory = "/home/nishanth/Desktop/Academics/Semster VI/Computer Vision/Project/output/"
    os.mkdir(output_directory)    
    print(output_directory)
    image_names = [(directory, file_name) for file_name in os.listdir(directory) if os.path.isfile(os.path.join(directory, file_name))]
    image_names.sort()
    nodes, edges = 0, 0
    G = []
    previous_set = []
    current_set = []
    for directory, file_name in image_names:
        os.mkdir(output_directory+file_name[:-4])
        print(output_directory+file_name[:-4]+'/')
        unary_score = object_detection.get_objects(os.path.join(directory, file_name), output_directory+file_name[:-4]+'/')
        temp_directory = output_directory+file_name[:-4]+'/'
        image_list = [temp_directory+file_name for file_name in os.listdir(temp_directory) if os.path.isfile(os.path.join(temp_directory, file_name))]
        image_list.sort()
        # image_list = [os.path.join(directory, file_name) for file_name in os.listdir(output_directory+file_name[:-4]+'/') if os.path.isfile(os.path.join(output_directory+file_name[:-4]+'/', file_name))]
        print(image_list)
        print(unary_score)
        previous_set = current_set
        current_set = []
        iterator = 0
        for image_filename in image_list:
            print(image_filename, end=",")
            nodes += 2

            image = cv2.imread(image_filename)
            if image is None:
                raise ValueError("Couldn't load image {}".format(image_filename))

            batch = triplet_reid.trinet_embed.get_augmentation_batch(image, triplet_reid.trinet_embed.im_mean)
            embedding = np.mean(triplet_reid.trinet_embed.predict_features(batch), axis=0)
            embedding = embedding/np.linalg.norm(embedding)
            current_set.append((nodes-1, nodes, embedding))
            G.append((nodes-1, nodes, unary_score[iterator]))
            iterator += 1
            print(','.join(map(str, embedding)))
        print("Edges: ")
        for _, h_i, f_i in previous_set:
            for o_j, _, f_j in current_set:
                G.append((h_i, o_j, np.log(1-f_i.dot(f_j))))
                print((h_i, o_j, np.log(1-f_i.dot(f_j))))
        print(G)
    edges = len(G)
    print("p min %d %d" % (nodes, edges))
    for head, tail, cost in G:
        print("a %d %d %d" % (head, tail, cost))