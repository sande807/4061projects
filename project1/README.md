Nick Orf 4669898
Patrick Sheedy 4176118
Eric Sande 4828951

	Make4061 will parse the "Makefile" and create a list of targets with their commands that are to be executed. After the file is parsed the program will search through each target and check to see if it has dependencies. If it does it will look if these dependencies have their own targets to be run. If a dependency has it's own target then it will fork to that target and complete it's command or other dependencies. After a target has completed all of it's dependencies it will use exec to run it's own command.

	Running 'make' in the shell will compile the program. After this, you have several options when you want to run the compiled 'make4061' file. -f followed by a file will run make4061 on that specific file. If -n is included then the commands will be output as text rather than being executed. If -m followed by a log file is set then the output will be placed in that specific log file. If -B is set then the program will ignore timestamps and recompile each program. If a specific target in the Makefile is listed when running ./make4061 then the program will recognize this target and set that as the only target (including it's dependencies) to be run.
