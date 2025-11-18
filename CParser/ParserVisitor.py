# -*- coding: utf-8 -*-
# @Author  : Jiang Yuan
# @Time    : 2021/5/16 11:40
# @Function:

from CParser.Prolog import Prolog
from CParser.cGrammer.CParser import CParser
from CParser.cGrammer.CVisitor import CVisitor


class ParserVisitor(CVisitor):
    def __init__(self):
        self.prolog_list = {}
        self.count = 1
        self.current_father_prolog = list()
        self.line_input_space = {}
        self.line_output_space = {}
        self.const_variable = {}
        self.const_int_dict = {}
        self.const_string_dict = {}
        self.func_definition_dict = {}

    def visitCompilationUnit(self, ctx:CParser.CompilationUnitContext):
        prolog = Prolog(id=self.count, name='CompilationUnit', value='', father_id=0, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp

    def visitDeclaration(self, ctx:CParser.DeclarationContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='VariableDeclaration', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp

    def visitDeclarationSpecifier(self, ctx:CParser.DeclarationSpecifierContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='DeclarationSpecifier', value=ctx.getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
        if prolog.value == 'const':
            self.const_variable.setdefault(prolog.line, [])
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp

    def visitInitDeclarator(self, ctx:CParser.InitDeclaratorContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        if ctx.getChildCount() == 1:
            prolog = Prolog(id=self.count, name='VariableName', value=ctx.getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.prolog_list[prolog.id] = prolog
            self.line_output_space.setdefault(prolog.line, []).append(prolog.value)
        else:
            prolog = Prolog(id=self.count, name='InitVariable', value='',  father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)

            # self.visitDirectDeclarator(ctx)
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            if ctx.declarator() == None:
                return
            directDeclarator_prolog = Prolog(id=self.count, name='VariableName', value=ctx.declarator().getText(),  father_id=cfp.id, children_id=set(), line=ctx.start.line)
            if self.const_variable.__contains__(directDeclarator_prolog.line):
                self.const_variable.setdefault(directDeclarator_prolog.line, []).append(directDeclarator_prolog.value)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.prolog_list[directDeclarator_prolog.id] = directDeclarator_prolog
            self.line_output_space.setdefault(directDeclarator_prolog.line,[]).append(directDeclarator_prolog.value)

            # self.visitInitializer(ctx)
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            if ctx.initializer() == None:
                return
            initializer_prolog = Prolog(id=self.count, name='InitVariableValue', value=ctx.initializer().getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.prolog_list[initializer_prolog.id] = initializer_prolog
            if initializer_prolog.value != '':
                self.line_input_space.setdefault(initializer_prolog.line, []).append(initializer_prolog.value)
                if ctx.start.line == 106:
                    self.const_int_dict.setdefault(initializer_prolog.line, []).append(initializer_prolog.value)
                elif ctx.start.line == 108:
                    self.const_string_dict.setdefault(initializer_prolog.line, []).append(initializer_prolog.value)

            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

    def visitFunctionDefinition(self, ctx:CParser.FunctionDefinitionContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='FunctionDefinition', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp


    def visitDirectDeclarator(self, ctx:CParser.DirectDeclaratorContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            self.current_father_prolog.append(cfp)
            if ctx.directDeclarator()!=None:
                prolog = Prolog(id=self.count, name='FunctionName', value=ctx.directDeclarator().getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[prolog.id] = prolog
                self.func_definition_dict.setdefault(prolog.line, []).append(prolog.value)
            if ctx.getChildCount() == 4:
                if ctx.parameterTypeList() == None:
                    return
                self.visitParameterTypeList(ctx.parameterTypeList())

    def visitParameterList(self, ctx:CParser.ParameterTypeListContext):
        cfp = self.current_father_prolog.pop()
        if cfp.name != 'ParameterList':
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='ParameterList', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            self.current_father_prolog.append(cfp)
            self.visitChildren(ctx)

    def visitParameterDeclaration(self, ctx:CParser.ParameterDeclarationContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='ParameterDeclaration', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)

        # visitDeclarationSpecifiers
        self.visitDeclarationSpecifiers(ctx.declarationSpecifiers())

        # visitDeclarator
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        if ctx.declarator() == None:
            return
        declarator_prolog = Prolog(id=self.count, name='ParameterName', value=ctx.declarator().getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.prolog_list[declarator_prolog.id] = declarator_prolog
        self.prolog_list[cfp.id] = cfp

    def visitArgumentExpressionList(self, ctx:CParser.ArgumentExpressionListContext):
        cfp = self.current_father_prolog.pop()
        if cfp.name != ctx.__class__.__name__[:-7]:
            cfp.children_id.add(self.count)
            argumentExpressionList_prolog = Prolog(id=self.count, name=ctx.__class__.__name__[:-7], value=ctx.__class__.__name__[:-7], father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(argumentExpressionList_prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            self.current_father_prolog.append(cfp)
            self.visitChildren(ctx)

    # statement
    def visitCompoundStatement(self, ctx:CParser.CompoundStatementContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        compoundStatement_prolog = Prolog(id=self.count, name='CompoundStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(compoundStatement_prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp

    def visitLabeledStatement(self, ctx:CParser.LabeledStatementContext):
        if ctx.getChild(0).getText() == 'case':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='CaseLabeledStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)

            #self.visitConstantExpression(ctx.constantExpression())
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            if ctx.constantExpression() == None:
                return
            constantExpression_prolog= Prolog(id=self.count, name='CaseValue', value=ctx.constantExpression().getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.prolog_list[constantExpression_prolog.id] = constantExpression_prolog

            self.visitStatement(ctx.statement())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        elif ctx.getChild(0).getText() == 'default':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='DefaultLabeledStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitStatement(ctx.statement())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

    def visitExpressionStatement(self, ctx:CParser.ExpressionStatementContext):
        cfp = self.current_father_prolog.pop()
        if cfp.name != 'ExpressionStatement':
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='ExpressionStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            self.current_father_prolog.append(cfp)
            self.visitChildren(ctx)

    def visitSelectionStatement(self, ctx:CParser.SelectionStatementContext):
        if ctx.getChild(0).getText() == 'if':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='SwitchSelectionStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        elif ctx.getChild(0).getText() == 'switch':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='SwitchSelectionStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

    def visitIterationStatement(self, ctx:CParser.IterationStatementContext):
        if ctx.getChild(0).getText() == 'for':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='ForIterationStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)

            # visitExpression
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            forCondition_prolog = Prolog(id=self.count, name='ForConditionalStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(forCondition_prolog)
            self.visitForCondition(ctx.forCondition())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

            self.visitStatement(ctx.statement())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        elif ctx.getChild(0).getText() == 'while':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='WhileIterationStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitStatement(ctx.statement())
            # visitExpression
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            expression_prolog = Prolog(id=self.count, name='WhileIterationStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(expression_prolog)
            if ctx.expression() == None:
                return
            self.visitExpression(ctx.expression())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        elif ctx.getChild(0).getText() == 'do':
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='DowhileIterationStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)

            # visitForCondition
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            expression_prolog = Prolog(id=self.count, name='DowhileConditionalStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(expression_prolog)
            if ctx.expression() == None:
                return
            self.visitExpression(ctx.expression())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

            self.visitStatement(ctx.statement())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp

    def visitJumpStatement(self, ctx:CParser.JumpStatementContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='JumpStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp

    # expression
    def visitAssignmentExpression(self, ctx:CParser.AssignmentExpressionContext):
        if ctx.getChildCount() == 3:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='AssignmentExpression', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)

            # visitUnaryExpression
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            if ctx.unaryExpression() == None:
                return
            unaryExpression_prolog = Prolog(id=self.count, name='VariableName', value=ctx.unaryExpression().getText(), father_id=cfp.id,children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.prolog_list[unaryExpression_prolog.id] = unaryExpression_prolog
            self.visitAssignmentOperator(ctx.assignmentOperator())
            self.visitAssignmentExpression(ctx.assignmentExpression())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
            self.line_output_space.setdefault(unaryExpression_prolog.line, []).append(unaryExpression_prolog.value)
        else:
            return self.visitChildren(ctx)

    def visitConditionalExpression(self, ctx:CParser.ConditionalExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='ConditionalExpression', value=ctx.getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitLogicalOrExpression(self, ctx:CParser.LogicalOrExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='LogicalOrExpression', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitLogicalAndExpression(self, ctx:CParser.LogicalAndExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='LogicalAndExpression', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitInclusiveOrExpression(self, ctx:CParser.InclusiveOrExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='InclusiveOrExpression', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitExclusiveOrExpression(self, ctx:CParser.ExclusiveOrExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='ExclusiveOrExpression', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitAndExpression(self, ctx:CParser.AndExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='AndExpression', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitEqualityExpression(self, ctx:CParser.EqualityExpressionContext):
        if ctx.getChildCount() > 1:
            if ctx.getChild(1).getText() == '==':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='EqualityExpression', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitEqualityExpression(ctx.equalityExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='EqualitySymbol', value='==', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitRelationalExpression(ctx.relationalExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
            elif ctx.getChild(1).getText() == '!=':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='InequalityExpression', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitEqualityExpression(ctx.equalityExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='InequalitySymbol', value='!=', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitRelationalExpression(ctx.relationalExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitRelationalExpression(self, ctx:CParser.RelationalExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='RelationalExpression', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitRelationalExpression(ctx.relationalExpression())
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            operate_prolog = Prolog(id=self.count, name='RelationalSymbol', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.prolog_list[operate_prolog.id] = operate_prolog
            self.visitShiftExpression(ctx.shiftExpression())
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitShiftExpression(self, ctx:CParser.ShiftExpressionContext):
        if ctx.getChildCount() > 1:
            cfp = self.current_father_prolog.pop()
            cfp.children_id.add(self.count)
            prolog = Prolog(id=self.count, name='ShiftExpression', value=ctx.getChild(1).getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
            self.count += 1
            self.current_father_prolog.append(cfp)
            self.current_father_prolog.append(prolog)
            self.visitChildren(ctx)
            cfp = self.current_father_prolog.pop()
            self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitAdditiveExpression(self, ctx:CParser.AdditiveExpressionContext):
        if ctx.getChildCount() > 1:
            if ctx.getChild(1).getText() == '+':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='AdditiveExpression', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitAdditiveExpression(ctx.additiveExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='PlusSymbol', value='+', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitMultiplicativeExpression(ctx.multiplicativeExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
            elif ctx.getChild(1).getText() == '-':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='SubtractStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitAdditiveExpression(ctx.additiveExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='MinusSymbol', value='-', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitMultiplicativeExpression(ctx.multiplicativeExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitMultiplicativeExpression(self, ctx:CParser.MultiplicativeExpressionContext):
        if ctx.getChildCount() > 1:
            if ctx.getChild(1).getText() == '*':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='MultiplicativeExpression', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitMultiplicativeExpression(ctx.multiplicativeExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='MultipSymbol', value='*', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitCastExpression(ctx.castExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
            elif ctx.getChild(1).getText() == '/':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='DivisionStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitMultiplicativeExpression(ctx.multiplicativeExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='DivisionSymbol', value='/', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitCastExpression(ctx.castExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
            elif ctx.getChild(1).getText() == '%':
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                prolog = Prolog(id=self.count, name='ModStatement', value='', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.current_father_prolog.append(cfp)
                self.current_father_prolog.append(prolog)
                self.visitMultiplicativeExpression(ctx.multiplicativeExpression())
                cfp = self.current_father_prolog.pop()
                cfp.children_id.add(self.count)
                operate_prolog = Prolog(id=self.count, name='ModSymbol', value='%', father_id=cfp.id, children_id=set(), line=ctx.start.line)
                self.count += 1
                self.prolog_list[operate_prolog.id] = operate_prolog
                self.current_father_prolog.append(cfp)
                self.visitCastExpression(ctx.castExpression())
                cfp = self.current_father_prolog.pop()
                self.prolog_list[cfp.id] = cfp
        else:
            return self.visitChildren(ctx)

    def visitPrimaryExpression(self, ctx:CParser.PrimaryExpressionContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='Value',value=ctx.getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp
        self.line_input_space.setdefault(prolog.line, []).append(prolog.value)
        if ctx.start.type == 106:
            # 106是常数 108是字符串
            self.const_int_dict.setdefault(prolog.line, []).append(prolog.value)
        elif ctx.start.type == 108:
            self.const_string_dict.setdefault(prolog.line, []).append(prolog.value)

    # operator
    def visitAssignmentOperator(self, ctx:CParser.AssignmentOperatorContext):
        cfp = self.current_father_prolog.pop()
        cfp.children_id.add(self.count)
        prolog = Prolog(id=self.count, name='Assignment',value=ctx.getText(), father_id=cfp.id, children_id=set(), line=ctx.start.line)
        self.count += 1
        self.current_father_prolog.append(cfp)
        self.current_father_prolog.append(prolog)
        self.visitChildren(ctx)
        cfp = self.current_father_prolog.pop()
        self.prolog_list[cfp.id] = cfp

