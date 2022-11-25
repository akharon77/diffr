#ifndef DSL_H
#define DSL_H

#define NUM_CTOR(node, val)         NodeCtor   (node,     NODE_TYPE_NUM, {.dbl = val}, NULL, NULL)
#define CREATE_NUM(val)             CreateNode (          NODE_TYPE_NUM, {.dbl = val}, NULL, NULL)
#define OP_CTOR(node, op, lhs, rhs) NodeCtor (node, NODE_TYPE_OP, {.op = op}, lhs, rhs)

#define LEFT                        (CURR)->left
#define RIGHT                       (CURR)->right

#define D_L                         Differentiate (LEFT)
#define D_R                         Differentiate (RIGHT)

#define CP_L                        TreeCopy (LEFT)
#define CP_R                        TreeCopy (RIGHT)
#define CP_CR                       TreeCopy (CURR)

#define ADD(lhs, rhs)               CreateNode (NODE_TYPE_OP, {.op = OP_ADD}, lhs, rhs)
#define SUB(lhs, rhs)               CreateNode (NODE_TYPE_OP, {.op = OP_SUB}, lhs, rhs)

#define MUL(lhs, rhs)               CreateNode (NODE_TYPE_OP, {.op = OP_MUL}, lhs, rhs)
#define DIV(lhs, rhs)               CreateNode (NODE_TYPE_OP, {.op = OP_DIV}, lhs, rhs)

#define SIN(rhs)                    CreateNode (NODE_TYPE_OP, {.op = OP_SIN}, NULL, rhs)
#define COS(rhs)                    CreateNode (NODE_TYPE_OP, {.op = OP_COS}, NULL, rhs)

#define EXP(lhs, rhs)               CreateNode (NODE_TYPE_OP, {.op = OP_EXP}, lhs,  rhs)
#define LN(rhs)                     CreateNode (NODE_TYPE_OP, {.op = OP_LN},  NULL, rhs)

#define IS_OP(node)                 (GET_TYPE(node) == NODE_TYPE_OP)
#define IS_OP_CODE(node, op_code)   (IS_OP(node) && GET_OP(node) == op_code)

#define IS_NUM(node)                (node != NULL && GET_TYPE(node) == NODE_TYPE_NUM)

#define EPS                          1e-6

#define IS_EQ(node, val)            (IS_NUM(node)                && \
                                     (val) - EPS < GET_NUM(node) && \
                                     GET_NUM(node) < (val) + EPS)

#define IS_ZERO(node)               (IS_EQ(node, 0))
#define IS_ONE(node)                (IS_EQ(node, 1))

#define IS_VAR(node)                (node != NULL && GET_TYPE(node) == NODE_TYPE_VAR)

#define IS_FUNC(node)               (node != NULL && (IS_OP(node) || IS_VAR(node)))

#define GET_TYPE(node)              ((node)->type)
#define GET_NUM(node)               ((node)->value.dbl)
#define GET_OP(node)                ((node)->value.op)
#define GET_VAR(nodr)               ((node)->value.var)

#endif  // DSL_H
