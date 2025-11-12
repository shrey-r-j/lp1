# importing the dictionaries of intructions
from mnenomics import AD, IS, DL, REG, COND

# declaring the Symbol table and Literal Table
SYMBOL = {}
LITERAL = {}
POOL = []

LC = []
# opening the source program
with open("sourceCode.txt") as f1:
    code = f1.readlines()
f1.close()

# formatting the code
for i in range (len(code)):
    code[i] = code[i].replace("\n",'').replace(',','').split()

# opening the intermediate file
f2 = open("intermediateCode.txt",'w')

# funcions to handle assembler directives and declarative statements
def assemblerDirective(line, word):
    global lc
    global cnt
    if word == "START":
        try :
            lc = int(line[1])
        except(IndexError, ValueError):
            lc = 0
    elif word == "END":
        flag = True
        lc +=1
        for i in (LITERAL):
            if LITERAL[i][1] == 'notset':
                LITERAL[i][1] = str(lc)
                flag = False
                lc += 1
        if flag == False:
            POOL.append(cnt)
    elif word == "LTORG":
        POOL.append(cnt)
        for i in (LITERAL):
            if LITERAL[i][1] == 'notset':
                LITERAL[i][1] = str(lc)
                lc += 1
                cnt+=1
    elif word == "ORIGIN":
        symbol = int(SYMBOL[line[1]][1])
        value = int(line[3])
        if line[2] == '+':
            lc = symbol + value
        else:
            lc = symbol - value
    elif word == "EQU":
        SYMBOL[line[0]][1] = SYMBOL[line[2]][1]  

def declareStatement(line,word):
    global lc
    if word == 'DC':
        SYMBOL [line[0]][1] = str(lc)
        lc += 1
    else:
        SYMBOL[line[0]][1] = str(lc)
        lc += int(line[2])

# pass 1 of Assembler
symindex = 1
litindex = 1
cnt = 1
for line in code:
    for word in line:
        if word in AD:
            f2.write(AD[word] + ' ')
            assemblerDirective(line,word)
        elif word in IS:
            f2.write(IS[word] + ' ')
            lc += 1
        elif word in DL:
            f2.write(DL[word] +' '+ line[2])
            declareStatement(line,word)
        elif word in REG:
            f2.write(REG[word] + ' ')   
        elif word in COND:
            f2.write(COND[word] + ' ')  
        elif "='" in word:
            if word not in LITERAL:
                LITERAL[word] = ['L' + str(litindex), 'notset']
            else: 
                word += "."
                LITERAL[word] = ['L' + str(litindex), 'notset']
            litindex += 1         
            f2.write(LITERAL[word][0] + ' ')  
        elif not word.isdigit() and word not in SYMBOL and '+' not in word and '-' not in word and "'" not in word:
            SYMBOL[word] = ['S' + str(symindex), str(lc)]
            symindex += 1
            if word != line[0]:
                f2.write(SYMBOL[word][0] + ' ')
        elif line[0] not in SYMBOL and word in SYMBOL:
            f2.write(SYMBOL[word][0] + ' ') 
    f2.write("\n")        

f2.close()

print("  ------------  ")
print(SYMBOL)
print("  ------------ ")
print(LITERAL)    

print(POOL)

