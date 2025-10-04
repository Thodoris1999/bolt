import numpy as np
import matplotlib.pyplot as plt

# Parameters
width = 1000
height = 1000
channels = 3  # RGB
dtype = np.float32  # 32-bit floating point

# Read the raw float RGB data from file
filename = "frame.bin"
with open(filename, "rb") as file:
    data = np.fromfile(file, dtype=dtype)

# Reshape the data into an image
# The shape should be (height, width, channels)
image = data.reshape((height, width, channels))

# glReadPixels origin is bottom left while maplotlib is top left
image = np.flipud(image)

# Display the image using matplotlib
plt.imshow(image)
plt.axis("off")  # Hide axes
plt.show()
