#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;


//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        class_map[classes->nth(i)->get_filename()] = classes->nth(i);
    }
}


void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);



    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);

    

    class_map[Object_class->get_filename()] = Object_class;
    class_map[IO_class->get_filename()] = IO_class;
    class_map[Int_class->get_filename()] = Int_class;
    class_map[Bool_class->get_filename()] = Bool_class;
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 


Symbol assign_class::checkType(ClassTable* classtable) {
    Symbol exprType = expr->checkType(classtable);
    if (!classtable->objectName_className.lookup(exprType)) {
        classtable->semant_error() << "assign exprType is not exit !" << endl;
    }
    Symbol assignedType = *classtable->objectName_className.lookup(name);
    if (!classtable->objectName_className.lookup(assignedType)) {
        classtable->semant_error() << "assign assignedType is not exit !" << endl;
    }
    if (!suitFor(assignedType, exprType, classtable)) {
        classtable->semant_error() << "assign not suitFor !" << endl;
    }

    return exprType;
}

Symbol static_dispatch_class::checkType(ClassTable* classtable) {
  Symbol e0 = expr->checkType(classtable);
    if (e0 != SELF_TYPE && classtable->class_map.find(e0) == classtable->class_map.end()) {
        classtable->semant_error() << "e0 is not exit " << endl;
    }

    Class_ cla = classtable->class_map[e0];
    std::vector<Class_> ancestorClassesIncludeSelf;
    std::vector<Class_> ancestorClasses = getAncestorClasses(cla, classtable);
    ancestorClassesIncludeSelf.push_back(cla);
    for (int i = 0; i < ancestorClasses.size(); i++) {
        ancestorClassesIncludeSelf.push_back(ancestorClasses[i]);
    }

    method_class* f;
    for (int i = 0; i < ancestorClassesIncludeSelf.size(); i++) {
        Class_ claTmp = ancestorClassesIncludeSelf[i];
        Features features = claTmp->get_features();
        for (int j = features->first(); features->more(j); j = features->next(j)) {
            Feature feature = features->nth(j);
            if (feature->get_ft() == feature_type::method) {
                method_class* m = (method_class*)feature;
                if (m->get_name() == name) {
                    f = m;
                    break;
                }
            }
        }
    }


    Formals formals = f->get_formals();
    Formal return_formal;
    for (int i = formals->first(), j = actual->first(); formals->more(i) && actual->more(j); i = formals->next(i), j = actual->next(j)) {
        Formal formalNeeded = formals->nth(i);
        Symbol formalNeededType = formalNeeded->get_type_decl();
        Symbol formalActual = actual->nth(j)->checkType(classtable);
        if (formalActual != SELF_TYPE && classtable->class_map.find(formalActual) == classtable->class_map.end()) {
            classtable->semant_error() << "formalActual is not exit in the class table " << endl;
        }
        if (!suitFor(formalNeededType, formalActual, classtable)) {
            classtable->semant_error() << "actual not sult fot f formal " << endl;
            break;
        }

        if ((!actual->more(j)) && formals->more(i)) {
            int return_formal_idx = formals->more(i);
            return_formal = formals->nth(return_formal_idx);
        }
    }

    if (return_formal->get_type_decl() == SELF_TYPE) {
        return e0;
    } else {
        return return_formal->get_type_decl();
    }
}

Symbol dispatch_class::checkType(ClassTable* classtable) {
    Symbol e0 = expr->checkType(classtable);
    if (e0 != SELF_TYPE && classtable->class_map.find(e0) == classtable->class_map.end()) {
        classtable->semant_error() << "e0 is not exit " << endl;
    }

    std::vector<Class_> ancestorClassesIncludeSelf;
    if (e0 == SELF_TYPE) {
        // self class : this.f(a1, a2, ...., an);
    } else {
        Class_ cla = classtable->class_map[e0];
        ancestorClassesIncludeSelf.push_back(cla);
    }

    // std::vector<Class_> ancestorClasses = getAncestorClasses(cla, classtable);
    
    // for (int i = 0; i < ancestorClasses.size(); i++) {
    //     ancestorClassesIncludeSelf.push_back(ancestorClasses[i]);
    // }

    method_class* f;
    for (int i = 0; i < ancestorClassesIncludeSelf.size(); i++) {
        Class_ claTmp = ancestorClassesIncludeSelf[i];
        Features features = claTmp->get_features();
        for (int j = features->first(); features->more(j); j = features->next(j)) {
            Feature feature = features->nth(j);
            if (feature->get_ft() == feature_type::method) {
                method_class* m = (method_class*)feature;
                if (m->get_name() == name) {
                    f = m;
                    break;
                }
            }
        }
    }


    Formals formals = f->get_formals();
    Formal return_formal;
    for (int i = formals->first(), j = actual->first(); formals->more(i) && actual->more(j); i = formals->next(i), j = actual->next(j)) {
        Formal formalNeeded = formals->nth(i);
        Symbol formalNeededType = formalNeeded->get_type_decl();
        Symbol formalActual = actual->nth(j)->checkType(classtable);
        if (formalActual != SELF_TYPE && classtable->class_map.find(formalActual) == classtable->class_map.end()) {
            classtable->semant_error() << "formalActual is not exit in the class table " << endl;
        }
        if (!suitFor(formalNeededType, formalActual, classtable)) {
            classtable->semant_error() << "actual not sult fot f formal " << endl;
            break;
        }

        if ((!actual->more(j)) && formals->more(i)) {
            int return_formal_idx = formals->more(i);
            return_formal = formals->nth(return_formal_idx);
        }
    }

    if (return_formal->get_type_decl() == SELF_TYPE) {
        return e0;
    } else {
        return return_formal->get_type_decl();
    }
}

Symbol cond_class::checkType(ClassTable* classtable) {
    Symbol type1 = pred->checkType(classtable);
    Symbol type2 = then_exp->checkType(classtable);
    Symbol type3 = else_exp->checkType(classtable);
    if (type1 != Bool) {
        classtable->semant_error() << "pred is not a boolean" << endl;
    }

    if (type2 != SELF_TYPE && classtable->class_map.find(type2) != classtable->class_map.end()) {
        classtable->semant_error() << "then_exp type is not exit in the class table" << endl;
    }

    if (type3 != SELF_TYPE && classtable->class_map.find(type3) != classtable->class_map.end()) {
        classtable->semant_error() << "else_exp type is not exit in the class table" << endl;
    }

    return leastType(type2, type3, classtable);

}

Symbol loop_class::checkType(ClassTable* classtable) {
    Symbol judegement = pred->checkType(classtable);
    if (judegement != Bool) {
        classtable->semant_error() << "judgement checkType is not a boolean" << endl;
    }
    Symbol bodyType = body->checkType(classtable);
    if (classtable->class_map.find(bodyType) != classtable->class_map.end()) {
        classtable->semant_error() << "bodyType is not exit in the class table " << endl;
    }

    return  Object;
}

Symbol typcase_class::checkType(ClassTable* classtable) {
    Symbol type = expr->checkType(classtable);
    for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
        Case ca = cases->nth(i);
        
    }
}

Symbol block_class::checkType(ClassTable* classtable) {
    for (int i = body->first(); body->more(i); i = body->next(i)) {
        Symbol type = body->nth(i)->checkType(classtable);
        if (type != SELF_TYPE && classtable->class_map.find(type) == classtable->class_map.end()) {
            classtable->semant_error() << "block type is not exit in the class table " << endl;
        }

        if (!body->more(i)) {
            return type;
        }
    }
}


Symbol let_class::checkType(ClassTable* classtable) {
    Symbol t0;
    if (get_type_decl() == SELF_TYPE) {
        t0 = SELF_TYPE;
    } else if (classtable->class_map.find(get_type_decl()) != classtable->class_map.end()) {
        t0 = get_type_decl();
    } else {
        classtable->semant_error() << "type is not exit in the class table" << endl;
    }

    Symbol type1 = init->checkType(classtable);
    if (classtable->class_map.find(type1) == classtable->class_map.end()) {
        classtable->semant_error() << "type1 is not exit in the class table" << endl;
    }

    Class_ cla = classtable->class_map[type1];
    std::vector<Class_> claes =  getAncestorClasses(cla, classtable);
    claes.push_back(cla);

    bool flag = false;
    for (int i = 0; i < claes.size(); i++) {
        if (t0 == claes[i]->get_name()) {
            flag = true;
        }
    }

    if (!flag) {
        classtable->semant_error() << "the condition of T1 <= T0 is not satisfied" << endl;
    }

    Symbol type2 = body->checkType(classtable);
    if (classtable->class_map.find(type2) == classtable->class_map.end()) {
        classtable->semant_error() << "type2 is not exit in the class table" << endl;
    }

    return type2;
}

Symbol plus_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Int;
}

Symbol sub_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Int;
}


Symbol mul_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Int;
}

Symbol divide_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Int;
}

Symbol neg_class::checkType(ClassTable* classtable) {
    Symbol type = e1->checkType(classtable);
    if (classtable->class_map.find(type) == classtable->class_map.end()) {
        classtable->semant_error() << "neg type is not exit in the class table" << endl;
    }
    return type;
}

Symbol lt_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Int;
}

Symbol eq_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Bool;
}

Symbol leq_class::checkType(ClassTable* classtable) {
    Symbol leftType = e1->checkType(classtable);
    Symbol rightType = e2->checkType(classtable);
    if (leftType != Int || rightType != Int) {
        classtable->semant_error() << "leftType or rightType is not Int " << endl;
    }

    return Bool;
}

Symbol comp_class::checkType(ClassTable* classtable) {
    Symbol type = e1->checkType(classtable);
    if (type == Bool) {
        return type;
    }
}

Symbol int_const_class::checkType(ClassTable* classtable) {
    return Int;
}

Symbol bool_const_class::checkType(ClassTable* classtable) {
    return Bool;
}

Symbol string_const_class::checkType(ClassTable* classtable) {
    return Str;
}

Symbol new__class::checkType(ClassTable* classtable) {
    if (type_name != SELF_TYPE && classtable->class_map.find(type_name) == classtable->class_map.end()) {
        classtable->semant_error() << "new type is not exit !" << endl;
    }
    return type_name;
}

Symbol isvoid_class::checkType(ClassTable* classtable) {
    Symbol claType = e1->checkType(classtable);
    if (classtable->class_map.find(claType) != classtable->class_map.end()) {
        classtable->semant_error() << "isvoid type is not exit " << endl;
    }

    return Bool;
}

Symbol no_expr_class::checkType(ClassTable* classtable) {
    return No_type;
}

Symbol object_class::checkType(ClassTable* classtable) {
    if (name == self) {
        return SELF_TYPE;
    } else if (classtable->objectName_className.lookup(name)) {
        return *classtable->objectName_className.lookup(name);
    } else {
        classtable->semant_error() << "" << endl;
    }
}





/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    checkMainClassAndMainMethodExit(classtable);

    /* some semantic analysis code may go here */

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}



// right must be suit for left
bool suitFor(Symbol nameLeft, Symbol nameRight, ClassTable* classtable) {
    // Class_ leftCla = classtable->class_map[nameLeft];
    Class_ rightCla = classtable->class_map[nameRight];
    std::vector<Class_> classes = getAncestorClasses(rightCla, classtable);
    for (int i = 0; i < classes.size(); i++) {
        Symbol name = classes[i]->get_name();
        if (nameLeft == name) {
            return true;
        }
    }

    return false;
}

Symbol leastType(Symbol a, Symbol b, ClassTable* classtable) {
    Class_ a_cla = classtable->class_map[a];
    Class_ b_cla = classtable->class_map[b];

    while (a_cla != NULL) {
        Class_ b_cla_tmp = b_cla;
        while (b_cla_tmp->get_parent() != NULL) {
            if (a_cla->get_name() == b_cla_tmp->get_name()) {
                return a_cla->get_name();
            }
            Symbol b_cla_name = b_cla->get_parent();
            if (classtable->class_map.find(b_cla_name) != classtable->class_map.end()) {
                classtable->semant_error() << "a_cla_name is not exit in the class table" << endl;
            }
            b_cla_tmp = classtable->class_map[b_cla_name];
        }
        Symbol a_cla_name = a_cla->get_parent();
        if (classtable->class_map.find(a_cla_name) != classtable->class_map.end()) {
            classtable->semant_error() << "a_cla_name is not exit in the class table" << endl;
        }
        Class_ a_cla = classtable->class_map[a_cla_name];
    }
    return NULL;
}


// get ancestor class, return vector include the child_class of the parmeter.
std::vector<Class_> getAncestorClasses(Class_ child_class, ClassTable* classtable) {
    std::vector<Class_> ancestorClasses;

    while (child_class->get_parent() != NULL) {
        ancestorClasses.push_back(child_class);
        Symbol parentClassName = child_class->get_parent();
        if (classtable->class_map.find(parentClassName) == classtable->class_map.end()) {
            classtable->semant_error() << "the parentClass is not defined in the class table ! " << endl;
            return;
        }

        child_class = classtable->class_map[parentClassName];
    }
    
    return ancestorClasses;
}


void program_class::checkClassInherbitCycle(ClassTable* classtable) {
    for (auto it = classtable->class_map.begin(); it != classtable->class_map.end(); it++) {
        Class_ cla = it -> second;
        std::vector<Class_> ancestorClasses = getAncestorClasses(cla, classtable);
        for (int i = 0; i < ancestorClasses.size(); i++) {
            Class_ c = ancestorClasses[i];
            if (c->get_parent() == cla->get_name()) {
                classtable->semant_error() << "there is a circle in the inherbit class !" << endl;
            }
        }
    }
}


void program_class::checkMethodRedefined(ClassTable* classtable) {
    for (auto it = classtable->class_map.begin(); it != classtable->class_map.end(); it++) {
       
    }
}



// Every program must have a class Main. Furthermore, the Main class must have a method main that
// takes no formal parameters.
void program_class::checkMainClassAndMainMethodExit(ClassTable* classtable) {
    // if not found Main class

    if (classtable->class_map.find(Main) == classtable->class_map.end()) {
        classtable->semant_error() << "There is no Main class !" << endl;
    }

    Class_ mainClass = classtable->class_map[Main];
    Features mainClassFeatures = mainClass->get_features();
    bool found_main = false;
    for(int i = mainClassFeatures->first(); mainClassFeatures->more(i); i = mainClassFeatures->next(i)) {
        Feature oneFeature = mainClassFeatures->nth(i);
        if (oneFeature->get_ft() == feature_type::method && oneFeature->get_name() == main_meth) {
            found_main = true;
        }
    }

    if (!found_main) {
        classtable->semant_error() << "The Main class has not found main method !" << endl;
    }

    return;
}



method_class* getMethodFromClass(Class_ cla, Symbol name) {
    Features features = cla->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->get_name() == name) {
            return (method_class*)feature;
        }
    }
}

void program_class::checkSomeWhatInMethod(ClassTable* classtable) {
    for (auto it = classtable->class_map.begin(); it != classtable->class_map.end(); it++) {
        Symbol className = it->first;
        Class_ cla = it->second;
        std::vector<Class_> ancesterClasses =  getAncestorClasses(cla, classtable);
        Features features =  cla->get_features();
        for (int i = features->first(); features->more(i); i = features->next(i)) {
            Feature feature = features->nth(i);
            if (feature->get_ft() == feature_type::method) {
                method_class* m = (method_class*)feature;
                Symbol name = m->get_name();
                
                for (int j = 0; j < ancesterClasses.size(); j++) {
                    method_class* ancestorClassMethod = getMethodFromClass(ancesterClasses[j], name);
                    if (!ancestorClassMethod) {
                        continue;
                    }

                    // check
                    if (ancestorClassMethod->get_return_type() == m->get_return_type()) {
                        classtable->semant_error() << "method redefined !" << endl;
                    }

                    Formals mFormals = m->get_formals();
                    Formals ancestorClassMethodFormals = ancestorClassMethod->get_formals();

                    for (int k = mFormals->first(), l = ancestorClassMethodFormals->first(); mFormals->more(k) && ancestorClassMethodFormals->more(l); k = mFormals->next(k), l = ancestorClassMethodFormals->next(l)) {
                        Formal mFormal = mFormals->nth(i);
                        Formal ancestorClassMethodFormal = ancestorClassMethodFormals->nth(i);
                        if (mFormal->get_name() != ancestorClassMethodFormal->get_name() || mFormal->get_type_decl() != ancestorClassMethodFormal->get_type_decl()) {
                            classtable->semant_error() << "method formal is different !" << endl;
                        }
                    }

                }

            } else if (feature->get_ft() == feature_type::attibute) {
                attr_class* attr = (attr_class*)feature;
                Symbol type = attr->get_type_decl();
                if (classtable->class_map.find(type) == classtable->class_map.end()) {
                    classtable->semant_error() << "attribute type is not exit !" << endl;
                }
            }
        }
    }
}