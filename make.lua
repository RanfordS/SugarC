#!/usr/bin/env lua
-- R's New Make --

require "luamake.tools"
local Project = require "luamake.project"

-- Coverage report:
-- gcovr -j 12 --gcov-executable "llvm-cov gcov" --object-directory objs/ --cobertura-pretty --cobertura result.xml



local project = {}
project.src = "src/"
project.obj = "objs/"
project.doc = "docs/"
project.extensions = {"c", "h"}
project.main = "main.c"
project.destination = "DataStructures"

-- -coverage
-- -fprofile-arcs -ftest-coverage
-- -fprofile-instr-generate -fcoverage-mapping
local flags = [[
-fsanitize=address -fsanitize=undefined
-fstack-protector-all
-fcf-protection=full
-Wall -Wextra
-g
-O0
-ggdb
]]..
"-I./"..project.src
project.flags = flags:gsub ("\n", " ")
-- -mavx2

-- linker flags
local links = [[
-lm
]]
project.links = links:gsub ("\n", " ")

local help = [[
OVERVIEW: Ranford's make script for C

USAGE: ./make.lua [action ...]

  Performs each action in the order they were provided.

OPTIONS:
  help                    Shows this help information
  clean                   Does the same as clean-build
  clean-all               Deletes all files created by this script
  clean-build             Deletes all files created by the `build` and `test` actions
  clean-coverage          Deletes all coverage related files
  clean-doc               Deletes all doxygen documentation
  clean-test              Deletes all programs built by the `test` action
  docs                    Builds the documentation
  build                   Builds the main program
  test                    Builds the unit test programs and runs them
  run                     Runs the project program
]]

Project.create (project)

local main_switch_table =
{   help = function ()
        print (help)
    end,
    clean = function ()
        Project.clean_objects (project)
    end,
    ["clean-all"] = function ()
        Project.clean_objects (project)
        Project.clean_documentation (project)
    end,
    ["clean-build"] = function ()
        Project.clean_objects (project)
    end,
    ["clean-coverage"] = function ()
        Project.clean_coverage (project)
    end,
    ["clean-doc"] = function ()
        Project.clean_documentation (project)
    end,
    ["clean-test"] = function ()
        print "clean-test action is not yet implemented"
    end,
    docs = function ()
        Project.documentation (project)
    end,
    build = function ()
        Project.check_all_dates (project)
        Project.build (project)
    end,
    run = function ()
        os.execute ("./".. project.destination)
    end,
    todo = function ()
        Project.todo (project)
    end,
    default = function ()
        print "Unrecognised action ignored"
    end,
}

if #arg == 0 then
    arg = {"help"}
end

for _, option in ipairs (arg) do
    Switch (option, main_switch_table)
end
--]]


-- EOF --
