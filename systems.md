# Hardware Systems

System Name | Details | Hostname | Hardware Specs | Software Specs |
| :-------- | :------ | :------- | :------------- | :------------- |
| NCI System _Gadi_ | Intel Xeon<br/>Cascade Lake server | gadi-login | 2 x Intel Platinum 8274<br/>24-core, 2.6GHz; turboboost up to 3.2 GHz with HyperThreading<br/>  32 KB 8-way L1 Cache, 1MB 16-way L2 cache, 36 MB 11-way L3 Cache (shared)<br/> 196 GB DDR4 RAM | GCC 8.5.0<br/>OpenMP<br/>PAPI 5.7.0 |
<!--
| ARM Neoverse | AWS Graviton | (see below) | 16 vCPUs Neoverse N1 cores, 2.6GHz<br/>64 KB 4-way L1 Cache, 512 KB 8-way L2 Cache, 4 MB 16-way L3 Cache (shared)<br/>4 32 GB RAM | GCC 8.2.1<br/>OpenMP<br/>   PAPI 5.7.0 |
-->

## Logging In

Briefly browse the [NCI Gadi user guide](https://opus.nci.org.au/display/Help/Gadi+User+Guide) to note the main topics.
    
Once you've logged in to your laptop, open a Terminal window and SSH into Gadi (_gadi.nci.org.au_) using your Gadi ID and password supplied during the workshop. The ID will be of the form `j[k-l][a-z]777`; so, for ID jka777, log in via `ssh -X jka777@gadi.nci.org.au`. (The `-X` option requests X11 Forwarding, which is necessary to run graphical programs over SSH, e.g. emacs.)
    
You can use [this link](https://my.nci.org.au/mancini/change-password) to change your NCI password.

Gadi uses [environment modules](https://opus.nci.org.au/display/Help/Environment+Modules) to customize user environments. Run the command `module avail` to see what modules are available, and `module list` to see what modules you are using. What version of gcc are you using by default?

You will need gcc and papi for most exercises. If not already loaded, execute the command `module load gcc papi` to add them, and also add the above command into your `~/.bashrc` file for later. Standard UNIX editors are installed including nano, vim and emacs (if you are not familiar with any of these, nano is recommended).

For the exercises, you may wish to open two sessions on remote system at once, one for editing files and one for commands. On Gadi, the command `xterm &` can be used to create a remote terminal window (saving you from typing your password in again).

## Creating an Interactive Session

For exercises which require timing measurements, you should run them interactively on a single compute node of Gadi. To start an interactive job in your current working directory, enter the following command:

    qsub -l wd -l ncpus=48 -l mem=192GB -l walltime=30:00 -I -X

This will allocate an entire node of Gadi (48 cores, 192GB of memory) for your exclusive interactive use for a period of 30 minutes, forwarding X windows through your login session. _As soon as you've finished using the node, remember to `exit` to free up the resource for other people to use!_

<!--
*   Execute the command `echo $graviton`. You should see the DNS address of the AWS ARM server; it will look something like: `ec2-18-217-136-35.us-east-2.compute.amazonaws.com` (the exact address changes every time the server is brought up). From your Gadi account, you should be able to login to the AWS ARM server using the same ID using the set-up ssh key, i.e. `ssh $graviton`. The editors nano, vim and emacs are available there as well.
-->