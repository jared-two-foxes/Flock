

local workspace = {}

workspace.name = 'Flock'
workspace.system = "premake5"
workspace.includePath = 'C:/Develop/Flock/Source'

workspace.projects = {
	common = {
	    name = 'common',
	    type = "StaticLib",
	    path = 'C:/Develop/Flock/Source/Common', 
	    system = 'premake5',
	    naming = "standard",
	},
	server_lib = {
	    name = 'server_lib',
	    type = "StaticLib",
	    path = 'C:/Develop/Flock/Source/Server',   
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	server = {
	    name = 'server',
	    type = "ConsoleApp",
	    path = 'C:/Develop/Flock/Source/ServerApp',
	    system = 'premake5',
	    dependencies = { 'server_lib' },
	    naming = "standard",
	},
	server_test = {
	    name = 'server_test',
	    type = "ConsoleApp",
	    path = 'C:/Develop/Flock/Source/ServerTest',  
	    system = 'premake5',
	    dependencies = { 'gmock', 'server_lib' },
	    naming = "standard",
	},	
	client_console = {
	    name = 'client_console',
	    type = "ConsoleApp",
	    path = 'C:/Develop/Flock/Source/ClientConsole',  
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	client = {
	    name = 'client',
	    type = "WindowedApp",
	    path = 'C:/Develop/Flock/Source/Client',  
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq', 'NebulaeEngine:nebulous_beta', 'NebulaeEngine:nebulous_gl3' },
	    naming = "standard",
	}	
}

return workspace