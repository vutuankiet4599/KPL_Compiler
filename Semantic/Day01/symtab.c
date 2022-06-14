/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT; 
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR; 
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY; 
  type->arraySize = arraySize;  
  type->elementType = elementType; 
  return type;
}

Type* duplicateType(Type* type) { 
	Type* newType = (Type*)malloc(sizeof(Type)); 
	newType->typeClass = type->typeClass;
	newType->arraySize = type->arraySize;
	newType->elementType = type->elementType;
	return newType;
}

int compareType(Type* type1, Type* type2) {	  
	if(type1->typeClass == type2->typeClass) {
		if(type1->typeClass == TP_ARRAY){ // type1->typeClass = type2->typeClass = TP_ARRAY
	    	if(type1->arraySize != type2->arraySize || compareType(type1->elementType, type2->elementType) == 0) {
	        	return 0;
	    	} else {
	        	return 1; 
	    	}
		}
	}
	
	return 0;
}

void freeType(Type* type) {	  
	if(type->elementType != NULL) {
		freeType(type->elementType);
	}
	free(type);
	type = NULL;
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
	ConstantValue *intConst = (ConstantValue*)malloc(sizeof(ConstantValue));	
	intConst->type = TP_INT;
	intConst->intValue = i;	
	return intConst;
}

ConstantValue* makeCharConstant(char ch) {	  
	ConstantValue *charConst = (ConstantValue*)malloc(sizeof(ConstantValue));	
	charConst->type = TP_CHAR;
	charConst->charValue = ch;	
	return charConst;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {	  
	ConstantValue *newConst = (ConstantValue*)malloc(sizeof(ConstantValue));	
	newConst->type = v->type;
	if(v->type == TP_CHAR) {
		newConst->charValue = v->charValue;
	} else if(v->type == TP_INT) {
	    newConst->intValue = v->intValue;
	}	
	return newConst;
}

/******************* Object utilities ******************************/

Scope* CreateScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* CreateProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = CreateScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* CreateConstantObject(char *name) { 
	Object *constobj = (Object*)malloc(sizeof(Object));	
	strcpy(constobj->name, name);
	constobj->kind = OBJ_CONSTANT;
	constobj->constAttrs = (ConstantAttributes*)malloc(sizeof(ConstantAttributes));
	constobj->constAttrs->value = NULL;	
	return constobj;
}

Object* CreateTypeObject(char *name) {	  
	Object *typeObj = (Object*)malloc(sizeof(Object));	
	strcpy(typeObj->name, name);
	typeObj->kind = OBJ_TYPE;
	typeObj->typeAttrs = (TypeAttributes*)malloc(sizeof(TypeAttributes));
	typeObj->typeAttrs->actualType = NULL;
  	return typeObj;
}

Object* CreateVariableObject(char *name) {  
  	Object *valObj = (Object*)malloc(sizeof(Object));
  	strcpy(valObj->name, name);
  	valObj->kind = OBJ_VARIABLE;
  	valObj->varAttrs = (VariableAttributes*)malloc(sizeof(VariableAttributes));
  	valObj->varAttrs->scope = symtab->currentScope;
  	valObj->varAttrs->type = NULL;
  	return valObj;
}

Object* CreateFunctionObject(char *name) {  
  	Object *func = (Object*)malloc(sizeof(Object));
  	strcpy(func->name, name);
  	func->kind = OBJ_FUNCTION;
  	func->funcAttrs = (FunctionAttributes*)malloc(sizeof(FunctionAttributes));
  	func->funcAttrs->scope = CreateScope(func, symtab->currentScope);
  	func->funcAttrs->paramList = NULL;
  	func->funcAttrs->returnType = NULL;
  	return func;
}

Object* CreateProcedureObject(char *name) {
  	Object *proc = (Object*)malloc(sizeof(Object));
  	strcpy(proc->name, name);
  	proc->kind = OBJ_PROCEDURE;
  	proc->procAttrs = (ProcedureAttributes*)malloc(sizeof(ProcedureAttributes));
  	proc->procAttrs->scope = CreateScope(proc, symtab->currentScope);
  	proc->procAttrs->paramList = NULL;
  	return proc;
}

Object* CreateParameterObject(char *name, enum ParamKind kind, Object* owner) {  
  	Object *param = (Object*)malloc(sizeof(Object));
  	strcpy(param->name, name);
  	param->kind = OBJ_PARAMETER;
  	param->paramAttrs = (ParameterAttributes*)malloc(sizeof(ParameterAttributes));
  	param->paramAttrs->kind = kind;
  	param->paramAttrs->type = NULL;
  	param->paramAttrs->function = owner;
  	return param;
}

void freeObject(Object* obj) {
    if (obj != NULL) {
        if (obj->constAttrs != NULL) {
            switch (obj->kind) {
            case OBJ_CONSTANT:
                if (obj->constAttrs->value != NULL) {
                    free(obj->constAttrs->value);
                    obj->constAttrs->value = NULL;
                }
                break;
            case OBJ_VARIABLE:
                if (obj->varAttrs->type != NULL) {
                    free(obj->varAttrs->type);
                    obj->varAttrs->type = NULL;
                }
                break;
            case OBJ_TYPE:
                if (obj->typeAttrs->actualType != NULL) {
                    free(obj->typeAttrs->actualType);
                    obj->typeAttrs->actualType = NULL;
                }
                break;
            case OBJ_PROGRAM:
                if (obj->progAttrs->scope != NULL) {
                    freeScope(obj->progAttrs->scope);
                    obj->progAttrs->scope = NULL;
                }
                break;
            case OBJ_FUNCTION:
                freeScope(obj->funcAttrs->scope); 
                break;
            case OBJ_PROCEDURE:
                freeScope(obj->procAttrs->scope);
                break;
            case OBJ_PARAMETER:
                if (obj->paramAttrs->type != NULL) {
                    free(obj->paramAttrs->type);
                    obj->paramAttrs->type = NULL;
                }
                break;
            default:
                break;
            }

            free(obj->constAttrs);
            obj->constAttrs = NULL;
        }
        free(obj);
        obj = NULL;
    }
}

void freeScope(Scope* scope) {
    if (scope != NULL) {
        freeObjectList(scope->objList);
        free(scope);
        scope = NULL;
    }
}

void freeObjectList(ObjectNode *objList) {
    if (objList != NULL) {
        freeObject(objList->object);
        freeObjectList(objList->next);
        objList = NULL;
    }
}

void AddObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {  
  	ObjectNode *curr = objList;
  	while(curr != NULL) {
    	if(strcmp(curr->object->name, name) == 0) {
      		return curr->object;
    	}
    	curr = curr->next;
 	}
  	return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = CreateFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEI");
  param = CreateParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEC");
  param = CreateParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITELN");
  AddObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      AddObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      AddObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  AddObject(&(symtab->currentScope->objList), obj);
}

