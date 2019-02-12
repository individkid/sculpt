# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments ignore any text that does not satisfy the next expected argument. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands to be appended to the polytope's file.

Each file starts a Haskell thread whose IO is space, polytope, and decoration. An initial FFI C function maps enums by name to ints. One FFI C function returnscharacters read from the file; it blocks as readlocker until the file is appended to, or as writelocker until the file's pipe is readable. Other FFI C functions build a Command to send to the OpenGL thread. One sends the built Command, and waits for the response. And others consume the response Command. Another FFI C function sends a string to the file's output queue. The blocking functions, read from file and send Command, first deque one char at a time to write to the file's pipe while possible.

User manipulation of the mouse can cause transformation or additive or subtractive or refining or execution or revealing commands to append to the pierced polytope's file, or to modify memory mapped planes and matrices in the files. Sending a -- with a pid command causes the process to wait for the write lock, but not send a command, change the memory mapped plane or matrix, and release the write lock without appended text, to indicate the matrix has changed.

The -- commands are as follows.  
--additive change click mode to hollow out region under clicked facet  
--subractive change click mode to fillin region over clicked facet  
--refine chance click mode to add random plane through point on clicked facet  
--transform change click mode to transform clicked target  
--reveal change click mode to make clicked facet transparent  
--hide change click mode to make clicked through facets opaque  
--cylinder change roller mode to rotate around rotate line  
--clock change roller mode to rotate around normal to picture plane through pierce point  
--normal change roller mode to rotate around normal clicked facet at point through pierce point  
--scale change roller mode to scale with clicked point fixed  
--drive change roller mode to move picture plane forward or back  
--rotate change mouse mode to tip target from focal point to pierce point  
--look change mouse mode to rotate picture plane around focal point  
--tanget change mouse mode to translate parallel to clicked facet  
--translate change mouse mode to translate parallel to picture plane  
--session change transform target to all observed facets  
--polytope change transform target to all facets in file of clicked facet  
--facet change transform target to clicked facet  
--plane "bname" versor and table leg lengths  
--matrix floats for per-file transformation  
--global floats for per session transformation  
--shared plane index for communicating to other sessions  
--space list of optional bnames and lists of sidednesses  
--region two lists of bnames and whether the indicated region is in the polytope  
--picture "bname" filename for texture  
--sounde "sname" coefficients, variables as references to snames and mnames, equations as quotients of sums of terms of one coefficients and up to three variables, value as equation, value change delay as equation, reschedule delay as equation, sound contribution as equation  
--metric "mname" script, refers to features, returns value  
--script "bname" script, attached to plane, takes pierce point, has side effects, returns whether click does its thing  

For example, --plane uses a boundary count in Read to send a plane specification to a Haskell thread. The Haskell thread sends a Command to the OpenGL thread that appends the plane vector to the Versor and Plane buffers for the original file. Then the Command triggers microcode that classifies the plane, and intersects it with previously added planes. The response of the Command allows the Haskell thread to send another Command that updates Frame, Point, and related buffers, and triggers the display microcode.

For another example, --space sends sidedness info for boundaries to sample, one at a time to a Haskell thread. To sample, the Haskell thread reads the new boundary's threat points from the Small space, run microcode to reinterpret those as planes, run microcode to classify each reinterpreted plane into a Large subspace. Then the Haskell thread reads the region's corners from the Large subspace, finds the average or throw, back-interprets that, and does the --plane operations on that.

