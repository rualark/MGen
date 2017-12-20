-- Stop playback (there is a bug that starts playback)
reaper.Main_OnCommand(1016, 0);
-- Select all items
reaper.Main_OnCommand(40182, 0);
-- Remove items
reaper.Main_OnCommand(40006, 0);
-- Go to start of project
reaper.Main_OnCommand(40042, 0);
-- Deselect all tracks
reaper.Main_OnCommand(40297, 0);
-- Select track 1
reaper.Main_OnCommand(40939, 0);
reaper.InsertMedia("C:\\ReaperBuf\\stage.mp3", 0);
-- Go to start of project
reaper.Main_OnCommand(40042, 0);
-- Select track 2
reaper.Main_OnCommand(40940, 0);
reaper.InsertMedia("C:\\ReaperBuf\\input.mid", 0);
-- Select all tracks
reaper.Main_OnCommand(40296, 0);
-- Render with last settings
reaper.Main_OnCommand(41824, 0)

-- Select all items
reaper.Main_OnCommand(40182, 0);
-- Remove items
reaper.Main_OnCommand(40006, 0);
-- Remove all tempo markers
count_tempo_markers = reaper.CountTempoTimeSigMarkers(0)
for i=count_tempo_markers,0,-1 do reaper.DeleteTempoTimeSigMarker(0, i) end
file = io.open("C:\\ReaperBuf\\finished.txt", "w")
file:write("Finished")
io.close(file)
-- Stop playback (there is a bug that starts playback)
reaper.Main_OnCommand(1016, 0);

