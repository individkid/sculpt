# sculpt

The only arguments are filenames, usually with a .-- extension. Each given file contains commands, each started with a --. Double dash commands that expect arguments are ignored if subsequent text does not satisfy. If a -- occurs before all expected arguments are satisfied, the incomplete command is ignored. Each instance of the program opens one OpenGL window. The program reads commands appended to the given files. Commands can manipulate the orientation and topology of planes. Planes from a file are considered a polytope. User manipulation of the mouse can cause commands in the polytope's file to be modified or appended.  

Each command line file, and files opened by a Lua thread, have a thread that synchronizes reads and writes of the file with other processes. Initially, -- commands are read until eof. Then a writelock at eof to infinity is attempted, or a readlock on one byte at eof is waited for. The writelocker waits on read from a named pipe. Processes append to the file by writing to the named pipe. After appending to file from named pipe, the writelock is released, the readlocks are released, and all processes read to eof and negotiate for locks again.  

Some commands, in particular --matrix, --global, and --plane, have dynamic fields. A write to the named pipe can cause a change to a field in the middle of the file instead of appending to the file. The writelocker must obtain a writelock to change the middle of the file, and readlocks protect reads from the middle of the file. To notify processes waiting for readlock at eof that a field in the middle of the file has changed, the writelocker appends --seek with a file location, and releases the writelock. After reading --seek, and reading the indicated field in the middle of the file, processes wait for writelock on the --seek command. Once writelock on the --seek is acquired, the --seek is truncated from the file, writelock is released, and locking at the new eof are negotiated.  

Before sending data to the io thread, the window thread saves what it sends. When fields from the middle of the file are sent, the window thread subtracts or divides the saved data from the current data, and uses that as a delta to modify the received data. Thus, if the window thread gets back the same data it sent, as it in general does, the received data will cause no change. However, if the window thread gets data written by another process, current data will change, but the user changes since the data was last sent will be folded into the changes from the other process.  

The -- commands and messages between threads are as follows. Note that only messages from the thread that reads files from the command line has side effects.  
-—seek --(resend command from given file location)  
-—include --(read from given file)  
-—exclude --(read from including file)  
--sculpt click additive --(add region over clicked faced) Read->SculptConf->Window  
--sculpt click subtractive --(remove region under clicked faced) Read->SculptConf->Window  
--sculpt click refine --(add random plane through clicked point on facet) Read->SculptConf->Window  
--sculpt click tweak --(randomize clicked facet plane) Read->SculptConf->Window  
--sculpt click perform --(trigger script of clicked facet) Read->SculptConf->Window  
--sculpt click transform --(click starts transformation) Read->SculptConf->Window  
--sculpt click suspend --(stop transformation by mouse motion) Read->SculptConf->Window  
--sculpt click pierce --(click changes fixed pierce point) Read->SculptConf->Window  
--sculpt mouse rotate --(rotate about fixed pierce point) Read->SculptConf->Window  
--sculpt mouse tangent --(translate parallel to fixed facet) Read->SculptConf->Window  
--sculpt mouse translate --(translate parallel to picture plane) Read->SculptConf->Window  
--sculpt roller cylinder --(rotate with rotated fixed axis) Read->SculptConf->Window  
--sculpt roller clock --(rotate with fixed normal to picture plane) Read->SculptConf->Window  
--sculpt roller normal --(rotate with fixed normal to facet) Read->SculptConf->Window  
--sculpt roller parallel --(translate with fixed normal to facet) Read->SculptConf->Window  
--sculpt roller scale --(scale with fixed pierce point) Read->SculptConf->Window  
--sculpt target session --(transform display) Read->SculptConf->Window  
--sculpt target polytope --(transform clicked polytope) Read->SculptConf->Window  
--sculpt target facet --(transform clicked facet) Read->SculptConf->Window  
--sculpt topology numeric --(tweak holds nothing invariant) Read->SculptConf->Window  
--sculpt topology invariant --(tweak holds polytope invariant) Read->SculptConf->Window  
--sculpt topology symbolic --(tweak holds space invariant) Read->SculptConf->Window  
--sculpt fixed relative --(tweak holds pierce point fixed) Read->SculptConf->Window  
--sculpt fixed absolute --(tweak holds nothing fixed) Read->SculptConf->Window  
-—configure --(change constants like focal length) Read->ConfigureConf->Window  
-—command --(send microcode buffers and triggers) Read->Command->Window  
--query --(send request for topology feature to display) Read->Query->Polytope  
-—matrix --(change transformation of polytope) Read->MatrixConf->Window  
-—global --(change transformation of display) Read->GlobalConf->Window  
-—region --(change whether region in polytope) Read->RegionConf->Polytope->Command->Window  
-—inflate --(change polytope regions to all inside) Read->InflateConf->Polytope->Command->Window  
-—space --(add planes sampled from sidednesses) Read->SpaceConf->Polytope->Command->Window  
-—plane --(add and classify plane from vector) Read->PlaneConf->Polytope->Command->Window  
-—picture --(decorate plane with texture from file) Read->PictureConf->Polytope->Command->Window  
-—test --(read and augment test results in file) Read->TestConf->Polytope  
-—timewheel --(start and stop stock and flow system) Read->TimewheelConf->System  
-—sound --(add stock and flows to system) Read->Sound->System  
-—script --(send script to execute) Read->ScriptConf->Script  
-—invoke --(if at eof, send script to execute) Read->InvokeConf->Script  
-—macro --(send script to execute upon click) Read->MacroConf->Window->Script  
-—hotkey --(send script to execute upon keypress) Read->HotkeyConf->Window->Script  
-—metric --(send script and parameter indices for volatile stock) Read->MetricConf->System->Script  
--(send window transformation change to other processes) Window->GlobalConf->Write  
--(send polytope transformation change to other processes) Window->MatrixConf->Write  
--(send facet transformation for plane to other processses) Window->ManipConf->Polytope->PlaneConf->Write  
--(request topology information) Script->Query->Polytope  
