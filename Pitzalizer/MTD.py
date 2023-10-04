import json

mapName = input()
mp = open(mapName+'.json')
dt = json.load(mp)

n = dt['number_of_nodes']
m = dt['number_of_edges']
print(n, m)

for edge in dt['list_of_edges']:  
    print(edge[0], edge[1])

strategic = dt["strategic_nodes"]
score = dt["scores_of_strategic_nodes"]
for i in range(6):
	print(strategic[i], score[i])

mp.close()
print("\n")

