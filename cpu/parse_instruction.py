"Parse the instruction set and generate instructions"
def load_instruction_set():
    "Load the instruction set"
    import json
    with open("instruction-data.json", 'rt') as f:
        return json.load(f)

def parse_instruction_set(set):
    "Extract information from the instruction set"
    for i in range(0,512):
        instr = set[i]

        # Add opcode
        instr['opcode'] = i

        if ('opname' not in instr):
            instr['opname'] = 'UNDEF'
            continue

        # Count and separate the operands (if any)
        operand = instr['operand']
        if (operand == ""):
            instr['oprnum'] = 0
        elif (',' in operand):
            instr['oprnum'] = 2
            sep = operand.find(',')
            instr['opr1'] = operand[:sep]
            instr['opr2'] = operand[sep+1:]
        else:
            instr['oprnum'] = 1

def expand_operand(set):
    "Some operands are not coded directly, expand them"
    for i in set:
        opname = i['opname']
        if opname in ['CPL','DAA']:
            i['oprnum'] = 1
            i['operand'] = 'A'
        elif opname[-1] == 'A' and i['opcode'] < 256:
            assert(i['opname'] in ['RLCA','RRCA','RLA','RRA'])
            i['opname'] = opname[:-1]
            i['oprnum'] = 1
            i['operand'] = 'A'
        elif opname in ['SUB','CP','OR','AND','XOR']:
            assert(i['oprnum'] == 1)
            i['oprnum'] = 2
            i['opr1'] = 'A'
            i['opr2'] = i['operand']

def remove_flags(set):
    "Strip off the flags which is irrelevant to parsing"
    for i in set:
        if i['opname'] is 'UNDEF':
            continue
        del i['Z']
        del i['N']
        del i['H']
        del i['C']

def test():
    import json
    set = load_instruction_set()
    parse_instruction_set(set)
    expand_operand(set)
    remove_flags(set)
    for i in set:
        if i['opname'] is not 'UNDEF' and '16' in i['operand']:
            print(i)
    print(json.dumps(set, indent=2))
