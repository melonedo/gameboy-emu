"""Automatically generate most of the inctruction set.
Generate all the switch branches, and the opcode length table.
These will be replace the anchor in exec-instr-draft"""

def find_anchor(draft, anchor):
    "Locate the inchor string, return \
        (text until last '\n', test after next '\n', indent string)"
    begin = draft.find(anchor)
    next_endl = draft[begin:].index('\n')
    last_endl = draft[:begin].rindex('\n')
    indent = draft[last_endl+1:begin]
    return (draft[:last_endl], draft[begin+next_endl+1:], indent)

def load_instruction_set():
    import parse_instruction as pi
    set = pi.load_instruction_set()
    pi.parse_instruction_set(set)
    pi.expand_operand(set)
    return set

def gen_length_table(set):
    "Generate the length table"
    with open("instruction-set.cpp", 'rt') as f:
        draft = f.read()
    (foreword, postscript, indent) = \
    find_anchor(draft, "/*--- The data will go here ---*/")
    with open("instruction-set.cpp", 'wt') as f:
        f.write(foreword)
        for i in range(0,256):
            if i % 16 == 0:
                f.write('\n' + indent)
            if (set[i]['opname'] == 'UNDEF'):
                f.write('0')
            else:
                f.write(set[i]['len'])
            if i != 255:
                f.write(', ')

        f.write('\n')
        f.write(postscript)

def gen_disas(set):
    "Generate disassembly"
    with open("instruction-set.cpp", 'rt') as f:
        draft = f.read()
    (foreword, postscript, indent) = \
    find_anchor(draft, "/*--- The disas will go here ---*/")
    with open("instruction-set.cpp", 'wt') as f:
        f.write(foreword)
        for i in range(0, 512):
            f.write('\n' + indent + '"')
            opname = set[i]['opname']
            if (opname == 'UNDEF'):
                f.write('UNDEF')
            else:
                f.write(set[i]['opname'] + ' ' + set[i]['operand'])
            if i != 511:
                f.write('",')
            else:
                f.write('"')
        f.write('\n')
        f.write(postscript)

def gen_instruction_case(set):
    "Generate individual cases for each instruction"
    with open("instruction-set.cpp", 'rt') as f:
        draft = f.read()
    (foreword, postscript, indent) = \
    find_anchor(draft, "/*--- More cases will go here ---*/")

    with open("instruction-set.cpp", 'wt') as f:
        f.write(foreword)

        for i in set:
            # Edge cases, specified in draft
            if i['opcode'] in [0xe8, 0xf8, 0xcb, 0xd9, 0x08]:
                continue

            f.write('\n' + indent)

            if i['opname'] == 'UNDEF':
                f.write(f"// case {hex(i['opcode'])}: // UNDEF\n")
                continue

            line = "case {}: // {} {}".format(
                hex(i['opcode']), i['opname'], i['operand'])
            f.write(line)

            for line in format_instruction(i):
                f.write('\n' + indent)
                f.write(line)
            # One blank line after break
            f.write('\n' + indent + 'break;\n')

        f.write(postscript)

# Replacement for each operand
repl = {
    'A': 'reg.a()', 'F': 'reg.f()', 'B': 'reg.b()', 'C': 'reg.c()',
    'D': 'reg.d()', 'E': 'reg.e()', 'H': 'reg.h()', 'L': 'reg.l()',
    'AF': 'reg.af()', 'BC': 'reg.bc()', 'DE': 'reg.de()',
    'HL': 'reg.hl()', 'SP': 'reg.sp()', 'PC': 'reg.pc()',

    'd8': 'opr8', 'r8': 'opr8',
    'd16': 'opr16', 'a16': 'opr16',

    '(HL)': 'mem_ref(reg.hl())',
    '(HL+)': 'mem_ref(reg.hl()++)',
    '(HL-)': 'mem_ref(reg.hl()--)',
    '(BC)': 'mem_ref(reg.bc())',
    '(DE)': 'mem_ref(reg.de())',
    '(a16)': 'mem_ref(opr16)',
    '(a8)': 'mem_ref(0xff00 + opr8)',
    '(C)': 'mem_ref(0xff00 + reg.c())',
}

def format_instruction(instr):
    "Format a single instruction. Return a list of lines."
    op = instr['opname']
    branch = None
    def xlate(key):
        return repl[instr[key]]

    if op in ['LD', 'LDH']:
        line = f"{xlate('opr1')} = {xlate('opr2')};"
    elif op in \
    ['ADD', 'ADC', 'SUB', 'SBC', 'AND', 'OR', 'XOR']:
        line = f"{xlate('opr1')} = {op}({xlate('opr1')}, {xlate('opr2')});"
    elif op in \
    ['INC','DEC','RL','RR','RRC','RLC','SWAP','SLA','SRL','SRA','CPL','DAA']:
        line = f"{xlate('operand')} = {op}({xlate('operand')});"
    elif op in ['RES', 'SET']:
        line = f"{xlate('opr2')} = {op}({instr['opr1']}, {xlate('opr2')});"
    elif op == 'BIT':
        line = f"{op}({instr['opr1']}, {xlate('opr2')});"
    elif op in \
    ['NOP', 'STOP', 'HALT', 'EI', 'DI', 'SCF', 'CCF']:
        line = f"{op}();"
    elif op in ['JP', 'CALL', 'JR']:
        if instr['oprnum'] == 1:
            line = f"{op}({xlate('operand')});"
        else:
            branch = instr['opr1']
            line = f"{op}({xlate('opr2')});"
    elif op == 'RET':
        if instr['operand'] != '':
            branch = instr['operand']
        line = f"{op}();"
    elif op in ['PUSH', 'POP']:
        line = f"{op}({xlate('operand')});"
    elif op == 'RST':
        dst = instr['operand']
        # __h -> 0x__
        dst = '0x' + dst[:-1]
        line = f"{op}({dst});"
    elif op == 'CP':
        line = f"{op}({xlate('opr1')}, {xlate('opr2')});"
    else:
        raise KeyError(instr)

    if branch is None:
        lines = [
        line,
        f"clocks = {instr['time']};"
        ]
    else:
        true_clocks, _, false_clocks = instr['time'].partition('/')
        lines = [
        "if (" + branch + "())",
        "{",
        '  ' + line,
        f"  clocks = {true_clocks};",
        "}",
        "else",
        f"  clocks = {false_clocks};",
        ]
    return lines


# Copy the draft
with open("instruction-set-draft.cpp", 'rt') as src:
    draft = src.read()
with open("instruction-set.cpp", 'wt') as dst:
    dst.write(draft)

ins_set = load_instruction_set()
gen_length_table(ins_set)
gen_disas(ins_set)
gen_instruction_case(ins_set)
