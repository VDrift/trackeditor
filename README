Converting a track from RACER:

* Unzip and put racer files in temporary folder RACER_TP (temporary track path).  Make sure the path has no spaces (dof2joe doesn't like spaces).
* Create new folder for track in track editor folder TRACKEDITOR_TP.
* Convert all texture names to lowercase in TRACKEDITOR_TP via "find . -type f -name \*.tga|sort -r|awk '{f=tolower($1);if(f!=$1 && p[f]!=1){print "mv "$1" "f}p[$1]=1}' | /bin/sh"
* Make folder TRACKEDITOR_TP/objects/
* Run "dof2joe/dof2joe -p TRACKEDITOR_TP/objects/ RACER_TP/*.dof".  If there are thousands of .dof files, you may have to do this in steps to avoid a "too many arguments" error from your shell.  This will take a while.  Textures are automatically converted using nconvert.  No filenames should have spaces.
* Open track editor /data/tracks/editor.config and set active track to TRACKEDITOR_TP.
* Run the track editor.  Trace the roadways and mark the starting position (press H for help).
* CD to the TRACKEDITOR_TP/objects folder (this is important, the packfile stores relative paths) and run "VDrift-tracked/joepack/joepack -c objects.jpk *.joe"
* Copy TRACKEDITOR_TP into the main VDrift tracks folder VDRIFT_TP.  Erase VDRIFT_TP/objects/*.joe (since they are in the pack file).
* Add your VDRIFT_TP to VDrift/data/tracks/track_list.txt
* Add VDRIFT_TP/about.txt and ensure that the first line is the name of the track.
* Open up all of the texture files in TRACKEDITOR_TP/objects and review which textures belong to objects that should be collide-able (roads and walls), have full brightness (trees), be mipmapped (fences and fine transparent objects should not be mipmapped), or be skyboxes.
* Any textures that have transparent areas are usually colored #ff00ff in the Racer textures. You'll need to make these truly transparent in the PNG files. An easy way to do this is to use ImageMagick. Use the command "mogrify -transparent rgb\(255,0,255\) file.png". This can be scripted to speed things up of course.
* Set the correct object properties using the VDrift-tracked/listedit tool (more documentation to come).
* Run VDrift and check out what the track looks like in-game.  Note that you will only be able to drive on the roadways you defined in the track editor since no other surfaces have been flagged as collideable.  Also take a screenshot for the track selection screen.
* Add the track selection screenshot to VDrift/gui/tracks/VDRIFT_TP.png (hopefully these png files will be moved into the folders of the individual tracks soon).
* Done!
