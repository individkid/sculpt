# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments ignore any text that does not satisfy the next expected argument. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands in the polytope's file to be modified or appended.

Each file starts a polytope thread containing the topology, positions, and decorations of the planes in the file. An io thread per file reads to eof, sending text to the polytope thread, which sends buffer data to the window thread. The io thread waits on readlock, or if it gets a writelock, waits on read from the . prefixed named pipe. Mouse action can send messages from the window thread to the selected file's polytope thread. Thus, the polytope threads wait for either string or action messages. Action messages can cause the polytope thread to change or append to the named pipe.

User manipulation of the mouse can cause transformation or additive or subtractive or refining or execution or revealing actions to be sent to the file's polytope thread. Sending a -- with a pid command causes the process to wait for the write lock, but not send a command, change the memory mapped plane or matrix, and release the write lock without appended text, to indicate the matrix has changed.

The -- commands are as follows.  
--additive change click mode to fill in region over clicked facet  
--subractive change click mode to hollow out region under clicked facet  
--refine chance click mode to add random plane through point on clicked facet  
--transform change click mode to transform clicked target  
--reveal change click mode to make clicked facet transparent  
--hide change click mode to make clicked through facets opaque  
--tweak change click mode to randomize target with fixed pierce point  
--randomize change click mode to randomize target without fixed point  
--cylinder change roller mode to rotate around rotate line  
--clock change roller mode to rotate around normal to picture plane through pierce point  
--normal change roller mode to rotate around normal to clicked facet through pierce point  
--scale change roller mode to scale with clicked point fixed  
--drive change roller mode to move picture plane forward or back  
--rotate change mouse mode to tip target from focal point to pierce point  
--look change mouse mode to rotate picture plane around focal point  
--tanget change mouse mode to translate parallel to clicked facet  
--translate change mouse mode to translate parallel to picture plane  
--session change transform target to all observed facets  
--polytope change transform target to all facets in file of clicked facet  
--facet change transform target to clicked facet  
--numeric hold nothing invariant upon randomize or tweak  
--invariant hold polytope invariant upon randomize or tweak  
--symbolic hold space invariant upon randomize or tweak  
--include share boundaries with given file  
--exclude stop sharing boundaries with last included file  
--plane "bname" versor and table leg lengths  
--matrix floats for per-file transformation  
--global floats for per-session transformation  
--shared plane index for communicating to other sessions  
--space list of optional bnames and lists of sidednesses  
--region two lists of bnames and whether the indicated region is in the polytope  
--picture "bname" filename for texture  
--sounde "sname" coefficients, variables as references to snames and mnames, equations as quotients of sums of terms of one coefficients and up to three variables, value as equation, value change delay as equation, reschedule delay as equation, sound contribution as equation  
--metric "mname" script, refers to features, returns value  
--script "bname" script, attached to plane, takes pierce point, has side effects, returns whether click does its thing  

For example, --plane sends a Command to the OpenGL thread that appends the plane to the Versor and Plane buffers for the file. Then the Command triggers microcode that classifies the plane, and intersects it with previously added planes. The response of the Command allows the Haskell thread to send another Command that updates Frame, Point, and related buffers, and triggers the display microcode.

For another example, --space sends a Command, one boundary at a time, threat points from the Small space, to OpenGL microcode to reinterpret those as planes, run microcode to classify each reinterpreted plane into a Large subspace. Then the Haskell thread reads the region's corners from the Large subspace, finds the average or throw, back-interprets that, and does the --plane operations on that.

