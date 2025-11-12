# Data in data structures:
MNT = [['ONE', 3, 1], ['TWO', 3, 1]]
PNTAB = [
    {'P1' : '&O', 'P2' : '&N', 'P3' : '&E'},
    {'P1' : '&T', 'P2' : '&W', 'P3' : '&O'}
]
KPDTAB = [{'&E' : 'AREG'}, {'&O' : 'BREG'}]
MDT = [[' MOVER P3 P1 ', ' ADD P3 P2 ', ' MOVEM P3 P1 ', ' MEND '], [' MOVER P3 P1 ', ' ADD P3 P2 ', ' MOVEM P3 P1 ', ' MEND ']]

# reading the source code file
with open("sourceCode2.txt") as f1:
    codeline = f1.readlines()
f1.close()

# creating an index for macro call
macrocall = []
for i in range(len(MNT)):
    macrocall.append(MNT[i][0])


# opening the expanded code file
f2 = open("expandedCode.txt",'w')
# functions
def macroExpansion(line):
    # identifying the macro
    line = line.replace(',','').split()
    getindex = macrocall.index(line[0])
    # building the aptab
    aptab = {}
    k = 0
    for k in range(1,len(line)):
        aptab['P' + str(k)] = line[k]
    
    if len(aptab) != len(PNTAB[getindex]):
        tmp2 = KPDTAB[getindex]
        key = PNTAB[getindex]['P3']
        aptab['P3'] = tmp2[key]

    print(aptab)    

    # writing the expanded code into file
    i = 0
    while("MEND" not in MDT[getindex][i]):
        tmp = MDT[getindex][i].split()
        for j in tmp:
            if j in aptab:
                f2.write(aptab[j] + " ")
            else:
                f2.write(j + ' ')    
        f2.write('\n')
        i += 1

#Macroprocessor pass 2
index = 0
while (index < len(codeline)):
    if 'MACRO' in codeline[index]:
        while('MEND' not in codeline[index]):
            index +=1
        index+=1    
    else:    
        if codeline[index].split()[0] in macrocall:
            macroExpansion(codeline[index])
        else:    
            f2.write(codeline[index])
        index +=1  