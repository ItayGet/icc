#!/usr/bin/python

from pygdbmi.gdbcontroller import GdbController
from pprint import pprint
import sys

class gdb_controller:
    '''
    A dictionary that defines each expr_ast's struct
    the values' first indices are fields that aren't recusive
    while their second indices are fields that are recusive
    '''
    ast_struct = {
            'binary': [['op'], ['lhs', 'rhs']],
            'conditional': [[], ['cond', 'lhs', 'rhs']],
            'identifier': [['name'], []],
            'unary': [['op'], ['operand']],
            'index': [[], ['array', 'index']],
            'memberAccess': [['isArrow'], ['record']],
            'sizeofUnary': [[], ['expr']],
            'sizeofTypename': [['typename'], []],
            'cast': [['typename'], ['expr']],
    }

    '''Start gdb process'''
    def __init__(self, args=''):
        self.gdb = GdbController()
        self.gdb.write('-file-exec-and-symbols ./icc')
        self.gdb.write('set args ' + args);
        self.gdb.write('b icc.c:43')
        self.gdb.write('r')

    '''Issue a print statement in gdb and return the result'''
    def print(self, arg):
        r = self.gdb.write('p ' + arg)
        try:
            r = r[1]['payload']
            r = r[r.index('=')+2:-2]
        except ValueError:
            pprint(r)
            raise
        return r

    '''Build ast from gdb prints'''
    def build_ast(self, expr='expr'):
        struct = self.print(expr + '->type')[7:]
        struct = struct[0].lower() + struct[1:]
        props = self.ast_struct[struct]
        struct_expr = expr + '->' + struct

        # Identifier is represented by its string with no dictionary
        if struct == 'identifier':
            n = self.print(struct_expr + '.name')
            n = n[n.index('"')+1:]
            n = n[:n.index('"')-1]
            return n

        ret = {'type': struct}
        for i in props[0]:
            p = self.print(struct_expr + '.' + i)
            if i == 'typename':
                p = p[p.index('"')+1:]
                p = p[:p.index('"')-1]
            ret[i] = p

        for i in props[1]:
            ret[i] = self.build_ast(struct_expr + '.' + i)

        return ret

progs = [
        [
            "(int) a ++ + (a?b:c)",
            {'type': 'binary',
             'op': 'exprAstAdd',
             'lhs': {'type': 'cast',
                     'typename': 'int',
                     'expr': {'type': 'unary', 'op': 'exprAstPostfixPlus', 'operand': 'a'}},
             'rhs': {'type': 'conditional', 'cond': 'a', 'lhs': 'b', 'rhs': 'c'}}
        ],
        [
            "a ? b += c - d[i] ? a, e : f : g",
            {'type': 'conditional',
             'cond': 'a',
             'lhs': {'type': 'binary',
                     'op': 'exprAstAddAssignment',
                     'lhs': 'b',
                     'rhs': {'type': 'conditional',
                             'cond': {'type': 'binary',
                                      'op': 'exprAstSubract',
                                      'lhs': 'c',
                                      'rhs': {'type': 'index', 'array': 'd', 'index': 'i'}},
                             'lhs': {'type': 'binary',
                                     'op': 'exprAstComma',
                                     'lhs': 'a',
                                     'rhs': 'e'},
                             'rhs': 'f'}},
             'rhs': 'g'}
        ],
        [
            "a ? b || c ? b + c : c + d : *a",
            {'type': 'conditional',
             'cond': 'a',
             'lhs': {'type': 'conditional',
                     'cond': {'type': 'binary',
                              'op': 'exprAstLogicalOr',
                              'lhs': 'b',
                              'rhs': 'c'},
                     'lhs': {'type': 'binary', 'op': 'exprAstAdd', 'lhs': 'b', 'rhs': 'c'},
                     'rhs': {'type': 'binary', 'op': 'exprAstAdd', 'lhs': 'c', 'rhs': 'd'}},
             'rhs': {'type': 'unary', 'op': 'exprAstDeref', 'operand': 'a'}}
        ],
]


for i in progs:
    a = gdb_controller('-a "' + i[0] + '"')
    ast = a.build_ast()

    # Test failed
    if ast != i[1]:
        print("ERROR: Test failed! for", file=sys.stderr)
        print(i[0], file=sys.stderr)
        pprint(ast, sort_dicts=False, stream=sys.stderr)
    else:
        print("Test passed successfully!")
