

local workspace = {}

workspace.name = 'Flock'
workspace.system = "premake5"

workspace.projects = {
	common = {
	    name = 'common',
	    type = "StaticLib",
	    path = 'C:/Develop/Flock/Source/Common',      
	    includePath = 'C:/Develop/Flock/Source',
	    system = 'premake5',
	    naming = "standard",
	},
	server = {
	    name = 'server',
	    type = "ConsoleApp",
	    path = 'C:/Develop/Flock/Source/Server',     
	    includePath = 'C:/Develop/Flock/Source', 
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	client_console = {
	    name = 'client_console',
	    type = "ConsoleApp",
	    path = 'C:/Develop/Flock/Source/ClientConsole',      
	    includePath = 'C:/Develop/Flock/Source',
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	client = {
	    name = 'client',
	    type = "WindowedApp",
	    path = 'C:/Develop/Flock/Source/Client',      
	    includePath = 'C:/Develop/Flock/Source',
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq', 'NebulaeEngine:nebulous_beta', 'NebulaeEngine:nebulous_gl3' },
	    naming = "standard",
	}	
}

return workspace