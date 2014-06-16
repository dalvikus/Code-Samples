AgentWithMS
===========

Short Description
-----------------
It is a platform-independent client/server model; a "main" server in a designated machine
collects from and controls over hundreds of machines by communicating with "clients" called workers in each machine through distributing python script.

Long Description
----------------
The client/server themselves are written by C++.
In order to gather and distribute Python script, they use Python/C API.
The project includes python modules for python script,
and sample python scripts in a WorkerScript directory.
It is independent of platforms (OSs) of each servers;
the platforms include UNIX (Solaris, HP), Linux, and Windows Server.

Roles and Responsibilities
--------------------------
We worked together in a team with 5 members.
I did research, design, and implement all in the sources in the architecture:
  . client/server
  . platform-specific modules: windows, unix
  . specificaion of python scripts for workers and sample scripts

The others' roles are to supplement serveral pythons script depending on platforms,
and to display all the gathered information on the web as a UI.

Note
----
1. It took three months to deploy a alpha version,
and two more months to distribute beta version with supporting modules in each platform.

2. Some comments in sources and document (Protocol-0.9.TXT) were written in Korean.
