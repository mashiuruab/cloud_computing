import os
import numpy as np

def randomize_centroids(data, centroids, k):
    for cluster in range(0, k):
        centroids.append(data[np.random.randint(0, len(data), size=1)].flatten().tolist())
    return centroids

# data is like [0,1,2,3,4,5,6,7,8,9]
print(randomize_centroids(np.arange(10), [], 5))



'''
- distributed learning with bagging 
'''
