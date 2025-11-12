# declaring the data sturctures
MDT = []
MNT = []
PNT = []
KPDT = []

# reading the source program
with open("sourceCode2.txt") as f1:
    code = f1.readlines()
f1.close()

# functions
def handleMacro(s_index):
    # getiing the name of the macro
    infoLine = code[s_index].split()
    
    macroinfo = []
    macroinfo.append(infoLine[0])

    # getting the parameters in the macro
    infoLine = code[s_index].replace('=',' ').replace(',','').split()
    pntab = {}
    for i in range(len(infoLine)):
        if '&' in infoLine[i]:
            pntab[infoLine[i]] = ('P' + str(i))
    PNT.append(pntab)
    macroinfo.append(len(pntab))

    # getting the keyword parameters
    infoLine = code[s_index].split()
    # print(infoLine)
    keydict = {}
    for i in range (len(infoLine)):
        if '=' in infoLine[i]:
            tmp = infoLine[i].split('=')
            keydict[tmp[0]] = tmp[1]
    KPDT.append(keydict)
    macroinfo.append(len(keydict))
    # definition of the macro
    while("MEND" not in code[s_index]):
        templine = code[s_index + 1].replace(',','').split()
        mdtline = " "
        for word in templine:
            if word in pntab:
                
                mdtline += pntab[word] + ' '
            else:
                
                mdtline += word + ' '
        MDT.append(mdtline)            
        s_index += 1
      
    # adding the data to MNT
    MNT.append(macroinfo)

    # returning the next pointer
    return s_index
# Macroprocessor pass one
index = 0
while ( index < len(code)):
    if "MACRO" in code[index]:
        handleMacro(index + 1)
    index+=1

print(MNT)
print(PNT)
print(KPDT)

f1 = open("MDT.txt","w")
for i in MDT:
    f1.write(i)
    f1.write('\n')