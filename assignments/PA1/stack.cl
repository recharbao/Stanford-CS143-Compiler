(*
 *  CS164 Fall 94
 *
 *  Programming Assignment 1
 *    Implementation of a simple stack machine.
 *
 *  Skeleton file
 *)


(*This is the meaning of paper, not mine*)

class StackCommand inherits IO {
   value : String <- "";
   nextCommand : StackCommand;

   value() : String {
      value
   };

   next() : StackCommand{
      nextCommand
   };

   save_value(s : String, next : StackCommand) : StackCommand {
      {
         value <- s;
         nextCommand <- next;
         self;
      }
   };


   push(s : String) : StackCommand {
      {
         (new StackCommand).save_value(s, self);
      }
   };

   pop() : StackCommand {
      {
         nextCommand;
      }
   };

   top() : StackCommand {
      {
         self;
      }
   };

   empty() : Bool {
      {
         if value = "" then true
         else false
         fi;
      }
   };
};


class Main inherits IO {

   display(s : StackCommand) : Object {
      {
         (let i : StackCommand <- s.top() in
            while not i.empty() loop {
               out_string(i.value());
               out_string("\n");
               i <- i.next();
            }
            pool
         );
      }
   };

   exec(s : StackCommand) : StackCommand {
      (let tmp : StackCommand <- s in
         if s.top().value() = "+" then tmp <- add(s)
         else tmp <- swap(s)
         fi
      )
   };

   swap(s : StackCommand) : StackCommand {
      
      {
         s <- s.pop();
         (let x : String <- s.top().value() in {
            s <- s.pop();
            (let y : String <- s.top().value() in {
               s <- s.pop();
               s <- s.push(x);
               s <- s.push(y);
            });
         });
      } 
   };

   add(s : StackCommand) : StackCommand {
      {
            s <- s.pop();
            (let x : String <- s.top().value() in {
               s <- s.pop();
               (let y : String <- s.top().value() in {
                  s <- s.pop();
               
                  (let x1 : Int <- (new A2I).a2i(x) in {
                     (let y1 : Int <- (new A2I).a2i(y) in {
                        (let z1 : Int <- x1 + y1 in {
                           (let z : String <- (new A2I).i2a(z1) in {
                              s <- s.push(z);
                           });
                        });
                     });
                  });
               });
            });
      }
   };

   main() : Object {
      (let s : StackCommand <- new StackCommand in {
         out_string(">");
         (let a : String <- in_string() in 
            while not a = "x" loop {
               --out_string(a);
               if a = "d" then {
                  display(s);
               }else {
                  if a = "e" then {
                     s <- exec(s);
                  }else {
                     s <- s.push(a);
                  }
                  fi;
               }
               fi;
               out_string(">");
               a <- in_string();
            }
            pool
         );
      })
   };

};
