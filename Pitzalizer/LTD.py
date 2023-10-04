import json

logName = input()
lg = open(logName+'.json')
dt = json.load(lg)

turnsdt = dt['turns']

count = 105
for i in range(106, 300):
	if turnsdt.get('turn'+str(i)) != None:
		count = i
	else:
		break
print(count)

init_puts = dt["initialize"]
print(len(init_puts), end="\t\t\t")
for user, node in init_puts:
    print(user, node, end = "\t\t")
print()

for turn in range(106 , count+1):
    turndt = turnsdt['turn'+str(turn)]
    print(*turndt['nodes_owner'])
    print(*turndt['troop_count'])
    print(*turndt['fort'])
    
    puted = turndt['add_troop']
    print(len(puted), end="\t\t\t\t")
    for node, count in puted:
    	print(node, count, end = "\t\t")
    print()
    attacks = turndt['attack']
    print(len(attacks), end = "\t\t\t\t")
    for attack in attacks:
    	print(attack["attacker"], attack["target"], attack["new_troop_count_attacker"],
    		attack["new_troop_count_target"], attack["new_target_owner"], attack["new_fort_troop"], end="\t\t\t")
    print()
    move = turndt['fortify']
    if move.get("path") == None:
    	print("0 0 0")
    else:
    	print(move.get("path")[0], move.get("path")[-1], move.get("number_of_troops"))
    
print(*dt['score'])
if dt.get("team_names") != None:
	print(*dt['team_names'])
lg.close()
print("\n")

