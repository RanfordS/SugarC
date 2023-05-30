--[[ maketools.lua ]]--
--
-- Contains supporting functions for the lua make tool.



---Asserts that obj has the type tie and produces a standard error message
---formatted with the given name.
---@param obj any Object to test
---@param name string Name of variable (used in error message)
---@param tie type Type expected for obj.
--
function Guard (obj, name, tie)
    assert (type (obj) == tie,
    ("Expected %s to be %s, got %s"):format (name, tie, type (obj)))
end



---Emulates a switch case using a table of functions.
---If case is a key of the table then that function is called.
---A default function can be specified as cases.default.
---If case is not a key and default is not specified then an error is raised.
---@param case any
---@param cases table<any>
--
function Switch (case, cases)
    Guard (cases, "cases", "table")
    for k, v in pairs (cases) do
        Guard (v, "cases["..k.."]", "function")
    end
    local entry = cases[case]
    if entry then return entry () end
    entry = cases.default
    if entry then return entry () end
    error (("Unhandled case '%s' in switch function"):format (tostring (case)))
end



---Prints a formatted string to the output.
--- 
--- ANSI codes for reference: \027[ ... m
--- 0: reset,     1: bold,      3: italic,    4: underline, 5: blink;
--- 30-37: foreground, 40-47: background;
--- X0: black,   X1: red,     X2: green,   X3: yellow, 
--- X4: blue,    X5: magenta, X6: cyan,    X7: white;
---@param format string
---@param ... any
--
function Printf (format, ...)
    Guard (format, "format", "string")
    print (format:format (...))
end



---Takes the keys of a table and concatenates them to a single string,
---optionally applies a prefix to each element.
---@param tab table
---@param separator string
---@param prefix string
---@return string
--
function table.key_to_cat (tab, separator, prefix)
    prefix = prefix or ""
    Guard (tab, "tab", "table")
    Guard (separator, "separator", "string")
    Guard (prefix, "prefix", "string")
    local new = {}
    for key, _ in pairs (tab) do
        table.insert (new, prefix..key)
    end
    return table.concat (new, separator)
end



-- Table for containing various path tools.
--
Path = {}



---Checks if the given file exists.
---@param path string
---@return boolean
--
function Path.file_exists (path)
    local file = io.open (path, "r")
    if file then
        file:close ()
        return true
    end
    return false
end



---Creates the given folder path if it's not present
---@param path string
--
function Path.ensure (path)
    os.execute ("mkdir -p ".. path)
end



---Returns the epoch time of either the system or a given file's last modify.
---@param path string
---@return number
--
function Path.get_time (path)
    local command = "date +%s"
    if path then
        Guard (path, "path", "string")
        command = command .. " -r " .. path
    end

    local stream = io.popen (command)
    assert (stream, "Failed to get time stamp of ".. path)
    local time = tonumber (stream:read "*all")
    assert (time, "Result had unexpected formatting")
    stream:close ()
    return time
end



---Splits the given file path into the folder path and filename.
---@param path string
--
function Path.split (path)
    local folder = path:match "^.*/" or ""
    local file = path:match "[^/]+$" or false
    return folder, file
end



---Takes a given path and performs one of the following:
---1. if new_ext is false/nil, returns the extension of the file;
---2. if new_ext is a string, returns the path with the new extension.
---Errors when no file extensions are found.
---@param path string
---@param new_ext? string
---@return string
--
function Path.extension (path, new_ext)
    Guard (path, "path", "string")
    local old_ext = path:match "%.([^./]-)$"
    assert (old_ext, "Cannot find or replace extension of "..path)
    if not new_ext then return old_ext end
    return path:sub (1, -1-#old_ext) .. new_ext
end



---Takes the path a turns it into a list of folder names.
---If the path points to a file, the filename is stored as a member.
---@param path any
---@return table
--
function Path.decimate (path)
    path = path or ""
    Guard (path, "path", "string")
    local list = {}
    for folder in path:gmatch "(.-/)" do
        table.insert (list, folder)
    end
    list.file = path:match "[^/]+$"
    return list
end



---Makes the relative path rel absolute with respect to basis.
---@param basis string
---@param rel string
---@return string
--
function Path.make_absolute (basis, rel)
    Guard (basis, "basis", "string")
    Guard (rel,   "rel",   "string")
    local a = Path.decimate (basis)
    local r = Path.decimate (rel)

    for _, f in ipairs (r) do
        if f == "../" then
            table.remove (a, #a)
        else
            table.insert (a, f)
        end
    end
    table.insert (a, r.file or "")
    return table.concat (a, "")
end



-- EOF
