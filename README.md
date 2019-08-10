# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments are ignored if subsequent text does not satisfy. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands in the polytope's file to be modified or appended. Most commands are appended to the .-- file. Written to the middle of the .-- file are --plane commands with repeated identifier, --matrix commands, and --global commands.  

Per given file, there is a Read Thread, a Write Thread, a File thread, the youngest or a new temporary temp file, a temporary named pipe, and an unnamed pipe. The Read Thread reads from the unnamed pipe, parses, mutex shares location with the Write Thread, and sends the Data through Message. The Write Thread unparses Data from Message, mutex checks location, and sends to the named pipe.  

The File thread waits for read lock at current position in the temp file, or gets a write lock at eof and waits for read from the named pipe. The write locker reads location/length/data from the named pipe, write locks and writes data of length at location to the given file, releases the write locks, sends the location/length/data on the unnamed pipe, and tries for write lock at eof of the temp file. The read lockers release their read locks, read the location/length/data, send it on the unnamed pipe, and try for write locks at eof of the temp file. If the temp file becomes too long, the File thread abandons it, and opens another.  

Initially before blocking on read from the unnamed pipe, Read sends location/pid to the named pipe, which is ignored when read from unnamed pipe in other processes. The File in the process that that sent the location/pid read locks and reads from the given file, and sends it on the unnamed pipe as location/length/data. At eof, Read stops sending location/pid before blocking on the unnamed pipe. If before getting to eof, Read gets an update to a sent Message, Read postpones the update until eof.  

Before sending data to the io thread, the window thread saves what it sends. When fields from the middle of the file are sent, the window thread subtracts or divides the saved data from the current data, and uses that as a delta to modify the received data. Thus, if the window thread gets back the same data it sent, as it in general does, the received data will cause no change. However, if the window thread gets data written by another process, current data will change, but the user changes since the data was last sent will be folded into the changes from the other process.  

The -- commands and messages between threads are as follows.  
--sculpt click additive (add region over clicked faced) ModeSource->Window  
--sculpt click subtractive (remove region under clicked faced) ModeSource->Window  
--sculpt click refine (add random plane through clicked point on facet) ModeSource->Window  
--sculpt click tweak (randomize clicked facet plane) ModeSource->Window  
--sculpt click perform (trigger script of clicked facet) ModeSource->Window  
--sculpt click transform (click starts transformation) ModeSource->Window  
--sculpt click suspend (stop transformation by mouse motion) ModeSource->Window  
--sculpt click pierce (click changes fixed pierce point) ModeSource->Window  
--sculpt mouse rotate (rotate about fixed pierce point) ModeSource->Window  
--sculpt mouse tangent (translate parallel to fixed facet) ModeSource->Window  
--sculpt mouse translate (translate parallel to picture plane) ModeSource->Window  
--sculpt roller cylinder (rotate with rotated fixed axis) ModeSource->Window  
--sculpt roller clock (rotate with fixed normal to picture plane) ModeSource->Window  
--sculpt roller normal (rotate with fixed normal to facet) ModeSource->Window  
--sculpt roller parallel (translate with fixed normal to facet) ModeSource->Window  
--sculpt roller scale (scale with fixed pierce point) ModeSource->Window  
--sculpt target session (transform display) ModeSource->Window  
--sculpt target polytope (transform clicked polytope) ModeSource->Window  
--sculpt target facet (transform clicked facet) ModeSource->Window  
--sculpt topology numeric (tweak holds nothing invariant) ModeSource->Window  
--sculpt topology invariant (tweak holds polytope invariant) ModeSource->Window  
--sculpt topology symbolic (tweak holds space invariant) ModeSource->Window  
--sculpt fixed relative (tweak holds pierce point fixed) ModeSource->Window  
--sculpt fixed absolute (tweak holds nothing fixed) ModeSource->Window  
--configure (change constants like focal length) ConfigureSource->Window  
--command (send microcode buffers and triggers) PolytopeSource->Window  
--global (change transformation of display) GlobalSource->Window  
--matrix (change transformation of polytope) MatrixSource->Window  
--plane (add and classify plane from vector) PlaneConf->Polytope  
--space (add planes sampled from sidednesses) SpaceConf->Polytope  
--region (change whether region in polytope) RegionConf->Polytope  
--inflate (change polytope regions to all inside) InflateConf->Polytope  
--polytope (add planes and regions sampled from polyants) PolytopeConf->Polytope  
--picture (decorate plane with texture from file) PictureConf->Polytope  
--once (send script to with data from polytope) OnceConf->Polytope  
--notify (send script for change of polytope) NotifyConf->Polytope  
--macro (send script to execute upon click) MacroConf->Window  
--hotkey (send script to execute upon keypress) HotkeyConf->Window  
--start (start stock and flow system) StartEvent->System  
--stop (stop stock and flow system) StopEvent->System  
--sound (add stock and flows to system) SoundEvent->System  
--metric (send script and parameter indices for volatile stock) NotifyEvent->System  
--script (send script to execute) Query->Script  
(send window transformation change to other processes) GlobalChange->Write  
(send polytope transformation change to other processes) MatrixChange->Write  
(send facet transformation for plane to other processses) PlaneChange->Write  
(send region opacity change for other processes) RegionChange->Write
