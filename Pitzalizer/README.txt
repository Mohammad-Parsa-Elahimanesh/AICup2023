It must be Updated!

usage:
./Pitzalizer <map relative address> <log relative address>

keys:
i : show ID and Score of Nodes
-> : next turn
<- : last turn
space : play game
^ (up key): *2 faster
v (down key): *2 solwer

# input-data format

# MTD out = DGP in
n m
v1 u1
v2 u2
. . .
vm um
str1 score_str1
str2 score_str2
...
str6 score_str6

#DGP out
n m
str1 score_str1
str2 score_str2
...
str6 score_str6
x1 y1
x2 y2
...
xn yn
v1 u1
v2 u2
. . .
vm um

#LTD out

count turns
troop1 troop2 troop3 ... troop105
# now main phase data comes
owner1 owner2 ... owner_n
troop1 troop2 ... troop_n
fort1 fort2 ... fort_n
countPut    node cnt     node cnt     node cnt  ...
countAttack     nodeH nodeT countH countT ownerT countFortT ....
...


score1 score2 score3

#visualizer input
DGP out
LTD out

# TODO: 
# check troops are all or just attack every where
# add Taqviat defayy to LTD.py & visualizer
# fix readme

