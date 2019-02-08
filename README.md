# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments ignore any text that does not satisfy the next expected argument. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands to be appended to the polytope's file.

Each file is opened by a separate thread. Another thread reads from named pipes with . prefix. Commands from the files are sent to the OpenGl thread, or to a Haskell thread, or to a timewheel thread. The OpenGl thread takes buffer data, and runs programs to find point or plane sidedness, or to find pierce points, or to render the display. The Haskell thread takes sidedness information or region specification, changes the topology of the polytope, and sends information to the OpenGL thread. The timewheel thread produces sound or framebuffer.

User manipulation of the mouse can cause transformation or additive or subtractive or refining or execution or revealing commands to append to the pierced polytope's file, or to modify memory mapped planes and matrices in the files. Sending a -- with a pid command causes the process to wait for the write lock, but not send a command, change the memory mapped plane or matrix, and release the write lock without appended text to indicate the matrix has changed.

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


