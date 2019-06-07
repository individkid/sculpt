# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments are ignored if subsequent text does not satisfy. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands in the polytope's file to be modified or appended. Most commands are appended to the .-- file. Written to the middle of the .-- file are --plane commands with repeated identifier, --matrix commands, and --global commands.  

Per given file, there is a Read Thread, a Write Thread, and a File thread. The File thread opens, for read/write, the given file, the youngest or a new temporary seek file, a temporary named pipe, and an unnamed pipe. The File thread waits for read lock at end of the seek file, or gets a write lock at eof and waits for read from the named pipe. The write locker reads location/length/data from the named pipe, writes data of length at location to the given file, releases the write lock, sends the location/length/data on the unnamed pipe, and tries for write lock at eof of the seek file. The read lockers release their read locks, read the location/length/data, sends it on the unnamed pipe, and tries for write lock at eof of the seek file. If the seek file becomes too long, the File thread abandons it, and opens another. The Read Thread reads from the unnamed pipe, parses, mutex shares Data with the Write Thread, and sends the Data through Message. The Write Thread unparses Data from Message, mutex checks shared Data to find out location, and sends to the named pipe.  

Before sending data to the io thread, the window thread saves what it sends. When fields from the middle of the file are sent, the window thread subtracts or divides the saved data from the current data, and uses that as a delta to modify the received data. Thus, if the window thread gets back the same data it sent, as it in general does, the received data will cause no change. However, if the window thread gets data written by another process, current data will change, but the user changes since the data was last sent will be folded into the changes from the other process.  

The -- commands and messages between threads are as follows. Note that only messages from the thread that reads files from the command line has side effects.  
--include (read from given file)  
--exclude (read from including file)  
--sculpt click additive (add region over clicked faced) Read->SculptConf->Window  
--sculpt click subtractive (remove region under clicked faced) Read->SculptConf->Window  
--sculpt click refine (add random plane through clicked point on facet) Read->SculptConf->Window  
--sculpt click tweak (randomize clicked facet plane) Read->SculptConf->Window  
--sculpt click perform (trigger script of clicked facet) Read->SculptConf->Window  
--sculpt click transform (click starts transformation) Read->SculptConf->Window  
--sculpt click suspend (stop transformation by mouse motion) Read->SculptConf->Window  
--sculpt click pierce (click changes fixed pierce point) Read->SculptConf->Window  
--sculpt mouse rotate (rotate about fixed pierce point) Read->SculptConf->Window  
--sculpt mouse tangent (translate parallel to fixed facet) Read->SculptConf->Window  
--sculpt mouse translate (translate parallel to picture plane) Read->SculptConf->Window  
--sculpt roller cylinder (rotate with rotated fixed axis) Read->SculptConf->Window  
--sculpt roller clock (rotate with fixed normal to picture plane) Read->SculptConf->Window  
--sculpt roller normal (rotate with fixed normal to facet) Read->SculptConf->Window  
--sculpt roller parallel (translate with fixed normal to facet) Read->SculptConf->Window  
--sculpt roller scale (scale with fixed pierce point) Read->SculptConf->Window  
--sculpt target session (transform display) Read->SculptConf->Window  
--sculpt target polytope (transform clicked polytope) Read->SculptConf->Window  
--sculpt target facet (transform clicked facet) Read->SculptConf->Window  
--sculpt topology numeric (tweak holds nothing invariant) Read->SculptConf->Window  
--sculpt topology invariant (tweak holds polytope invariant) Read->SculptConf->Window  
--sculpt topology symbolic (tweak holds space invariant) Read->SculptConf->Window  
--sculpt fixed relative (tweak holds pierce point fixed) Read->SculptConf->Window  
--sculpt fixed absolute (tweak holds nothing fixed) Read->SculptConf->Window  
--configure (change constants like focal length) Read->ConfigureConf->Window  
--command (send microcode buffers and triggers) Read->Command->Window  
--matrix (change transformation of polytope) Read->MatrixConf->Window  
--global (change transformation of display) Read->GlobalConf->Window  
--plane (add and classify plane from vector) Read->PlaneConf->Polytope->Command->Window  
--picture (decorate plane with texture from file) Read->PictureConf->Polytope->Command->Window  
--region (change whether region in polytope) Read->RegionConf->Polytope->Command->Window  
--inflate (change polytope regions to all inside) Read->InflateConf->Polytope->Command->Window  
--space (add planes sampled from sidednesses) Read->SpaceConf->Polytope->Command->Window  
--polytope (add planes and regions sampled from polyants) Read->PolytopeConf->Polytope->Command->Window
--query (send request for topology feature to display) Read->Query->Polytope  
--timewheel (start and stop stock and flow system) Read->TimewheelConf->System  
--sound (add stock and flows to system) Read->Sound->System  
--script (send script to execute) Read->ScriptConf->Script  
--invoke (if at eof, send script to execute) Read->InvokeConf->Script  
--macro (send script to execute upon click) Read->MacroConf->Window->Script  
--hotkey (send script to execute upon keypress) Read->HotkeyConf->Window->Script  
--metric (send script and parameter indices for volatile stock) Read->MetricConf->System->Script  
(send window transformation change to other processes) Window->GlobalConf->Write  
(send polytope transformation change to other processes) Window->MatrixConf->Write  
(send facet transformation for plane to other processses) Window->Manip->Polytope->PlaneConf->Write  
(send region opacity change for other processes) Window->Manip->Polytope->RegionConf->Write
(request topology information) Script->Query->Polytope  
