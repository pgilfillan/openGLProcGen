import cv2 as cv
import numpy as np

width = 512
height = 512
image = np.zeros((height, width, 1), np.uint8)
cv.imshow('Noise', image)
cv.waitKey(0)
cv.destroyAllWindows()