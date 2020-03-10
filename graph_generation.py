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
    for directory, file_name in image_names:
        os.mkdir(output_directory+file_name[:-4])
        print(output_directory+file_name[:-4]+'/')
        object_detection.get_objects(os.path.join(directory, file_name), output_directory+file_name[:-4]+'/')
        # embeddings_json = []
        temp_directory = output_directory+file_name[:-4]+'/'
        image_list = [temp_directory+file_name for file_name in os.listdir(temp_directory) if os.path.isfile(os.path.join(temp_directory, file_name))]
        # image_list = [os.path.join(directory, file_name) for file_name in os.listdir(output_directory+file_name[:-4]+'/') if os.path.isfile(os.path.join(output_directory+file_name[:-4]+'/', file_name))]
        print(image_list)
        for image_filename in image_list:
            print(image_filename, end=",")
            # sys.stdout.flush()

            image = cv2.imread(image_filename)
            if image is None:
                raise ValueError("Couldn't load image {}".format(image_filename))

            batch = triplet_reid.trinet_embed.get_augmentation_batch(image, triplet_reid.trinet_embed.im_mean)
            embedding = np.mean(triplet_reid.trinet_embed.predict_features(batch), axis=0)
            print(','.join(map(str, embedding)))
            # embeddings_json.append({'file_name': image_filename, 'embedding_vector': embeddings_json})