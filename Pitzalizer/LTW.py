import json

logName = input()
lg = open(logName+'.json')
dt = json.load(lg)

res = open('result.txt' , 'a')
res.write(str(dt['score']) + '\n')
res.write('winner : ' + str(dt['score'].index(max(dt['score']))) + '\n')
res.close()
