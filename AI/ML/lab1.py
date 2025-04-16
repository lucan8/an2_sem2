import numpy as np
from PIL import Image

images = [np.load("images/car_" + str(i) + ".npy") for i in range(9)]
# print(images)

tot_img_sum = np.sum(images)
# print(tot_img_sum)

img_sum_l = [np.sum(img) for img in images]
# print(img_sum_l)

max_img_sum_i = img_sum_l.index(np.max(img_sum_l))
# print(max_img_sum_i)

# I want the average image
avg_img = np.mean(images, axis=0)
# print(avg_img)
Image.fromarray(avg_img.astype(np.uint8)).save("avg_image.png")

std_dev = np.std(images, axis=0)
# print(std_dev)

norm_img = [(img - avg_img) / std_dev for img in images]
# print(norm_img)

crop_img = [img[200:300][280:400] for img in images]
# print(crop_img)

img = Image.open('images/test_img.jpg')
img = np.array(img)
# print(img)

img_col_sum = np.sum(img, axis=(0, 2))
# print(img_col_sum)
max_col_sum = max(img_col_sum)
new_images = []
tmp = []

for i in range(len(img_col_sum)):
    if img_col_sum[i] == max_col_sum:
        if tmp != []:
            new_images.append(tmp)
            Image.fromarray(np.swapaxes(np.array(tmp), axis1=0, axis2=1).astype(np.uint8)).save("avg_image_" + str(len(new_images)) + ".jpg")
            tmp = []
    else:
        tmp.append(img[:, i])
        

