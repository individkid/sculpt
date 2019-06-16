# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments are ignored if subsequent text does not satisfy. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands in the polytope's file to be modified or appended. Most commands are appended to the .-- file. Written to the middle of the .-- file are --plane commands with repeated identifier, --matrix commands, and --global commands.  

Per given file, there is a Read Thread, a Write Thread, a File thread, the youngest or a new temporary temp file, a temporary named pipe, and an unnamed pipe. The Read Thread reads from the unnamed pipe, parses, mutex shares location with the Write Thread, and sends the Data through Message. The Write Thread unparses Data from Message, mutex checks location, and sends to the named pipe.  

The File thread waits for read lock at current position in the temp file, or gets a write lock at eof and waits for read from the named pipe. The write locker reads location/length/data from the named pipe, write locks and writes data of length at location to the given file, releases the write locks, sends the location/length/data on the unnamed pipe, and tries for write lock at eof of the temp file. The read lockers release their read locks, read the location/length/data, send it on the unnamed pipe, and try for write locks at eof of the temp file. If the temp file becomes too long, the File thread abandons it, and opens another.  

Initially before blocking on read from the unnamed pipe, Read sends location/pid to the named pipe, which is ignored when read from unnamed pipe in other processes. The File in the process that that sent the location/pid read locks and reads from the given file, and sends it on the unnamed pipe as location/length/data. At eof, Read stops sending location/pid before blocking on the unnamed pipe. If before getting to eof, Read gets an update to a sent Message, Read postpones the update until eof.  

Before sending data to the io thread, the window thread saves what it sends. When fields from the middle of the file are sent, the window thread subtracts or divides the saved data from the current data, and uses that as a delta to modify the received data. Thus, if the window thread gets back the same data it sent, as it in general does, the received data will cause no change. However, if the window thread gets data written by another process, current data will change, but the user changes since the data was last sent will be folded into the changes from the other process.  

The -- commands and messages between threads are as follows. Note that only messages from the thread that reads files from the command line has side effects.  
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
--matrix (change transformation of polytope) Read->MatrixConf->Window  
--global (change transformation of display) Read->GlobalConf->Window  
--plane (add and classify plane from vector) Read->PlaneConf->Polytope->Command->Window  
--picture (decorate plane with texture from file) Read->PictureConf->Polytope->Command->Window  
--region (change whether region in polytope) Read->RegionConf->Polytope->Command->Window  
--inflate (change polytope regions to all inside) Read->InflateConf->Polytope->Command->Window  
--space (add planes sampled from sidednesses) Read->SpaceConf->Polytope->Command->Window  
--polytope (add planes and regions sampled from polyants) Read->PolytopeConf->Polytope->Command->Window
--include (open given file and use as shared subspace) Read->IncludeConf->Polytope  
--query (send request for topology feature to display) Read->Query->Polytope  
--command (send microcode buffers and triggers) Read->Command->Window  
--sound (add stock and flows to system) Read->Sound->System  
--script (send script to execute) Read->ScriptConf->Script  
--macro (send script to execute upon click) Read->MacroConf->Window->Script  
--hotkey (send script to execute upon keypress) Read->HotkeyConf->Window->Script  
--metric (send script and parameter indices for volatile stock) Read->MetricConf->System->Script  
--notify (send script to execute upon topology change) Read->NotifyConf->Polytope->Script
--configure (change constants like focal length) Read->ConfigureConf->Window  
--timewheel (start and stop stock and flow system) Read->TimewheelConf->System  
(send window transformation change to other processes) Window->GlobalConf->Write  
(send polytope transformation change to other processes) Window->MatrixConf->Write  
(send facet transformation for plane to other processses) Window->Manip->Polytope->PlaneConf->Write  
(send region opacity change for other processes) Window->Manip->Polytope->RegionConf->Write
(request topology information) Script->Query->Polytope  
