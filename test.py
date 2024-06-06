import numpy as np

layers = [

    [960,540],
    [960,540]

]

full_width = 0

full_height = 0

for layer in layers:

    full_width += layer[0]
    if(layer[1]>full_height): full_height = layer[1]

matrice = []

last_x = 0

for layer in layers:

    width = layer[0]/full_width
    matrice.append([width, layer[1]/full_height, round(last_x,4), 0])
    last_x+=matrice[-1][0]

size = .25
pos = .5

matrice_cp = np.array(matrice.copy())

matrice_cp[0][0] = matrice[0][0]*size
matrice_cp[0][2] = matrice[0][2]+(matrice[0][0]*pos)


vertices = []

for mat in matrice:

    vertices.append([mat[2], mat[3]])
    vertices.append([mat[2]+mat[0], mat[3]])
    vertices.append([mat[2], mat[3]+mat[1]])
    vertices.append([mat[2]+mat[0], mat[3]+mat[1]])



for vert in vertices:
    vert[0] = vert[0]*2-1
    vert[1] = vert[1]*2-1

for mat in matrice:
    print(mat)

print("----------------")

for vert in vertices:
    print(vert)


mat = [

    -.5 , -1,
     0  , -1,
    -.5 ,  0,
     0  ,  0


]
