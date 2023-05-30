#!/usr/bin/env lua
-- R's New Make --

require "luamake.tools"


--[[
--  example structure:
--
--  project =
--  {   ["rfont/cmap.c"] =
--      {   path = "rfont/cmap.c"
--      ,   subfolder = "rfont/"
--      ,   filename = "cmap.c"
--      ,   extension = "c"
--      ,   time = 165451403
--      ,   dependencies =
--          {   ["rfont/cmap.h"] = true
--          ,   ["rfont.h"] = true
--          }
--      ,   compile = true
--      }
--  ,   ["rfont/cmap.h"] =
--      {   path = "rfont/cmap.h"
--      ,   subfolder = "rfont/"
--      ,   filename = "cmap.h"
--      ,   extension = "h"
--      ,   time = 165451335
--      ,   dependencies =
--          {   ["rfont.h"] = true
--          }
--      ,   compile = false
--      }
--  }
--]]



---@class FileEntry
---@field path string
---@field subfolder string
---@field filename string
---@field extension string
---@field time number
---@field dependencies {[string]: boolean}
---@field compile boolean Determined during build
local FileEntry = {}

---Prints information about a file entry.
---@param entry FileEntry
--
function FileEntry.print (entry)
    Printf ("path      = %s", entry.path)
    Printf ("subfolder = %s", entry.subfolder)
    Printf ("filename  = %s", entry.filename)
    Printf ("extension = %s", entry.extension)
    Printf ("time      = %i", entry.time)
    print  ("dependencies:")
    if entry.dependencies then
        for k, _ in pairs (entry.dependencies) do
            Printf ("- %s", k)
        end
    else
        print ("- NOT COLLECTED!")
    end
    Printf ("compile = %s", tostring (entry.compile))
end


---@class Project
---@field src string Base directory of all source code
---@field obj string Directory for object files
---@field doc string Directory for doxygen documentation
---@field extensions string[] List of source file extensions to build
---@field main string Path of the main file (redundant?)
---@field flags string Build flags
---@field links string Link flags
---@field destination string Resulting executable
---@field files {string: FileEntry} List of files included in the project
local Project = {}



---Takes a table and checks that it has all the required fields.
---If valid, it will search for the project files.
---@param proj Project
--
function Project.create (proj)
    assert (proj.src, "Source directory not specified.")
    assert (proj.obj, "Object directory not specified.")
    assert (proj.extensions, "Extensions list not specified.")
    assert (#proj.extensions, "Extensions list is empty.")
    assert (proj.main, "Main file not specified.")
    assert (proj.flags, "Compiler flags not specified.")
    assert (proj.links, "Linker flags not specified.")
    assert (proj.destination, "Project destination not specified.")
    proj.files = {}
    for _, ext in ipairs (proj.extensions) do
        Project.find_files (proj, ext)
    end
end


--[[
---Prints information about a file entry.
---@param data table
--
function Project.print_file_data (data)
    Printf ("path      = %s", data.path)
    Printf ("subfolder = %s", data.subfolder)
    Printf ("filename  = %s", data.filename)
    Printf ("extension = %s", data.extension)
    Printf ("time      = %i", data.time)
    print  ("Dependencies:")
    if data.dependencies then
        for k, _ in pairs (data.dependencies) do
            Printf ("- %s", k)
        end
    else
        print ("- NOT COLLECTED!")
    end
end
--]]


---Displays info about the project files.
---@param proj Project
--
function Project.debug (proj)
    for _, v in pairs (proj.files) do
        --Project.print_file_data (v)
        FileEntry.print (v)
        print ""
    end
end


---Adds files of the specified extension into the project.
---@param proj table
---@param ext string
--
function Project.find_files (proj, ext)
    -- run find command
    local command = "find %s -type f -name '*.%s'"
    command = command:format (proj.src, ext)
    local stream = assert (io.popen (command), "failed to run command")
    -- parse  results
    while true do
        local path = stream:read "*line"
        if path == nil then break end
        -- remove the leading source part of the path
        path = path:sub (1 + proj.src:len ())
        -- create entry
        local entry = {}
        entry.path = path
        entry.subfolder, entry.filename = Path.split (path)
        entry.extension = Path.extension (path)
        entry.time = Path.get_time (proj.src..path)
        entry.dependencies = false -- indicates not yet collected
        entry.compile = false
        proj.files[path] = entry
    end
    stream:close ()
end



---Collects the dependencies for the given entry, acts recursively.
---Updates the entry time to match dependencies.
---@param proj Project
---@param entry FileEntry
--
function Project.dependencies (proj, entry)
    assert (entry, "attempted to collect dependencies on nil")
    if entry.dependencies then return end

    entry.dependencies = {}
    local file = io.open (proj.src .. entry.path, "r")
    assert (file, "Could not open ".. entry.path .." for dependency collection")
    local text = file:read "*all"
    -- remove comments
    text = text:gsub ("(/%*.-%*/)", "")
    text = text:gsub ("(//.-\n)", "\n")
    file:close ()
    -- create an intermediate list..
    local deps = {}
    for dep in text:gmatch "#include%s-\"(.-)\"" do
        table.insert (deps, Path.make_absolute (entry.path, dep))
    end
    -- .. so we can clear the text from memory before recursion
    text = nil
    -- recurse
    for _, dep in ipairs (deps) do
        entry.dependencies[dep] = true
        local other = proj.files[dep]
        assert (other, entry.path.." attempted to include `"..dep.."`, file not in project")
        Project.dependencies (proj, other)
        for other_dep, _ in pairs (other.dependencies) do
            entry.dependencies[other_dep] = true
            entry.time = math.max (entry.time, other.time)
        end
    end
end



---Compares a C file entry with its object
---and determines whether it needs to be compiled
---@param proj Project
---@param entry FileEntry
--
function Project.check_dates (proj, entry)
    local object = proj.obj .. Path.extension (entry.path, "o")
    if Path.file_exists (object) then
        local time = Path.get_time (object)
        entry.compile = time < entry.time
    else
        entry.compile = true
    end
end

---Does check_dates for all buildable files in the project
---@param proj any
function Project.check_all_dates (proj)
    for _, v in pairs (proj.files) do
        Project.dependencies (proj, v)
        if v.extension == "c" then
            Project.check_dates (proj, v)
        end
    end
end



---Collects the files and dependencies for the project.
---@param proj Project
--
function Project.collect (proj)
    for _, ext in ipairs (proj.extensions) do
        Project.find_files (proj, ext)
    end
end



--[[ Job structure
--
--  job =
--  {   src = full path to source folder
--  ,   dst = full path to object folder
--  }
--
--]]


---@class Job
---@field src string
---@field dst string
---@field process file*
---@field altprocess file*



---Provides three lists:
---1. Full paths to all unit source files
---2. Full paths to all non-unit object files
---3. Source/destination mappings for all build jobs
---@param proj Project
---@return string[] Object file list
---@return string[] Compile job list
function Project.build_list (proj)
    local objects = {}
    local jobs = {}

    for path, entry in pairs (proj.files) do
        if entry.extension == "c" then
            local map = {}
            map.src = proj.src .. path
            map.dst = proj.obj .. Path.extension (path, "o")
            table.insert (objects, map.dst)
            if entry.compile then
                table.insert (jobs, map)
            end
        end
    end
    return objects, jobs
end



---Processes all object build jobs.
---@param proj Project
---@param jobs Job[]
---@return boolean
function Project.do_jobs (proj, jobs)
    -- no jobs? do nothing
    if #jobs == 0 then
        print ("\027[1;32m- No Files to Build -\027[0m")
        return true
    end
    -- start jobs
    for _, job in ipairs (jobs) do
        -- ensure directories exist
        local folder = Path.split (job.dst)
        Path.ensure (folder)
        -- compile
        local command = "clang %s -o %s -c %s 2>&1"
        command = command:format (proj.flags, job.dst, job.src)
        job.process = assert (io.popen (command), "Could not start build process")
        Printf ("\027[1;34m- Building %s -\027[0m", job.src)
        --local altcommand = "clang -S -fverbose-asm %s -o %s %s 2>&1"
        local altcommand = "clang -S %s -o %s %s 2>&1"
        altcommand = altcommand:format (proj.flags, Path.extension (job.dst, "s"), job.src)
        job.altprocess = assert (io.popen (altcommand), "Could not start assembly process")
        Printf ("\027[1;34m- ASM Spool %s -\027[0m", job.src)
    end
    -- tracks whether there was at least one success
    local build_success = false
    -- tracks whether there was at least one error
    local build_error = false
    local error_count = 0
    local warning_count = 0
    -- collect results
    for _, job in ipairs (jobs) do
        job.process:flush ()
        local message = job.process:read "*all"
        local results = {job.process:close ()}
        if results[3] ~= 0 then
            build_error = true
            Printf ("\027[1;31m- Errors from %s -\027[0m", job.src)
            print (message)
            error_count = error_count + 1
        elseif message ~= "" then
            Printf ("\027[1;33m- Warnings from %s -\027[0m", job.src)
            print (message)
            warning_count = warning_count + 1
        else
            build_success = true
        end
        job.altprocess:flush ()
        local altmessage = job.altprocess:read "*all"
        local altresults = {job.altprocess:close ()}
        if altresults[3] ~= 0 then
            Printf ("\027[1;32m- Errors from alt %s -\027[0m", job.src)
            print (altmessage)
            error_count = error_count + 1
        elseif altmessage ~= "" then
            Printf ("\027[1;32m- Warnings from alt %s -\027[0m", job.src)
            print (altmessage)
            warning_count = warning_count + 1
        end
    end
    -- residual messages
    if build_success then
        if warning_count + error_count == 0 then
            print ("\027[1;32m- Everything Built without Warnings -\027[0m")
        else
            print ("\027[1;32m- Everything Else Built without Warnings -\027[0m")
        end
    end
    if build_error then
        return false
    end
    return not build_error
end



---Calls clang to build and link the project files.
---@param proj Project
---@return boolean Success state of the build
--
function Project.build (proj)
    local objects, jobs = Project.build_list (proj)
    if not Project.do_jobs (proj, jobs) then
        print ("\027[1;31m- Terminated: Build Encountered an Error -\027[0m")
        return false
    end

    Printf ("\027[1;34m- Linking %s -\027[0m", proj.destination)

    local object_list = table.concat (objects, " ")
    local command = "clang %s -o %s %s %s 2>&1"
    command = command:format (proj.flags, proj.destination, object_list, proj.links)
    local stream = io.popen (command)
    assert (stream, "Failed to run linker?")
    local text = stream:read "*all"
    local results = {stream:close ()}
    if results[3] == 0 then
        print "\027[1;32m- Done: Success -\027[0m"
    else
        print "\027[1;31m- Terminated: Linking Encountered an Error -\027[0m"
        print (text)
    end
    return true
end



--  {   ["rfont/cmap.c"] =
--      {   path = "rfont/cmap.c"
--      ,   subfolder = "rfont/"
--      ,   filename = "cmap.c"
--      ,   extension = "c"
--      ,   time = 165451403
--      ,   dependencies =
--          {   ["rfont/cmap.h"] = true
--          ,   ["rfont.h"] = true
--          }
--      ,   compile = true
--      }
---Collects all todo notes.
---@param proj Project
function Project.todo (proj)
    Printf ("\027[1;34m- Collecting TODO Notes -\027[0m")
    local note_found = false

    for _, v in pairs (proj.files) do
        local path = proj.src .. v.path
        local file = io.open (path, "r")
        assert (file, "Could not open "..path.." for collecting TODO notes")
        local source = file:read "*all"
        -- remove all block comments
        source = source:gsub ("/%*!(.*)%*/", "")
        -- collect notes
        local notes = {}
        for note in source:gmatch "%s-//TODO:(.-)\n" do
            table.insert (notes, note)
        end
        -- display if applicable
        if #notes > 0 then
            note_found = true
            print ""
            Printf ("\027[1;33m%s\027[0m", v.path)
            for _, note in ipairs (notes) do
                print ("-".. note)
            end
        end
    end

    if note_found then
        print ""
    else
        print "\027[1;32m- No TODO Notes Found -\027[0m"
    end
end



---Builds the documentation
---@param proj Project
--
function Project.documentation (proj)
    local _ = proj
    print "\027[1;32m- Documenting -\027[0m"
    os.execute "doxygen doxyfile"
    print "\027[1;32m- Done -\027[0m"
end



---Deletes all object files
---@param proj Project
--
function Project.clean_objects (proj)
    print "\027[1;35m- Cleaning Object Files -\027[0m"
    local command = string.format ("find %s -type f -delete", proj.obj)
    os.execute (command)
    print "\027[1;32m- Done -\027[0m"
end



---Deletes all object files
---@param proj Project
--
function Project.clean_documentation (proj)
    Guard (proj.doc, "project.doc", "string")
    print "\027[1;35m- Cleaning Documentation Files -\027[0m"
    local command = string.format ("rm -f -r %s*", proj.doc)
    os.execute (command)
    print "\027[1;32m- Done -\027[0m"
end



---Deletes all object files
---@param proj Project
--
function Project.clean_coverage (proj)
    print "\027[1;35m- Cleaning Coverage Files -\027[0m"
    local command
    --command = string.format ("find %s -type f -name '*.gcno' -delete", proj.obj)
    --os.execute (command)
    command = string.format ("find %s -type f -name '*.gcda' -delete", proj.obj)
    os.execute (command)
    print "\027[1;32m- Done -\027[0m"
end



---Coverage: TODO
---@param proj Project
--
function Project.run_coverage (proj)
    local command = "gcovr -j 12 --gcov-executable \"llvm-cov gcov\" --object-directory %s --coveralls-pretty --output coverage.json"
    os.execute (string.format (command, proj.obj))
    print "\027[1;32m- Done -\027[0m"
end



return Project

-- EOF --
