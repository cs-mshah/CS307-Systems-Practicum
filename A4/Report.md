
## Problem 1

### 1A
Using ifconfig, the laptop gave the output of `172.16.21.161`
On using iplocation.com, the IP address shown was `14.139.34.5`.
These values are different. `172.16.21.161` is our Private IP Address. which is used to communicate within the network, and is use dto load the network. It has a local scope.

On the other hand, `14.139.34.5` is our Public IP Address which is used to communicate outside the network. It is used for accessing the internet.

Similarly, results for each of the team members:
Pratiksha:
![](ifconfig_prati.png) 
![](ip2l_prati.png)

Solai Adithya:
![](ifconfig_adhi.png) 
![](ip2l_adhi.png)

Ayuj:
![](ifconfig_ayuj.png) 
![](ip2l_ayu.pngj)

Manan:
![](ifconfig_prati.png) 
![](ip2l_prati,png)

Niveditha:
![](ifconfig_niv.png) 
![](ip2l_niv.png)

Harnanman:
![](ifconfig_harnanman.png) 
![](ip2l_harnanman.png)

### 1B

We traced `www.amazon.com`
Observations:
![](traceroute.png) 
Location of ip Addresses
![](iplookup_2.png) 
![](iplookup_3.png) 
![](iplookup_4.png) 
![](iplookup_5.png) 
![](iplookup_6.png) 
![](iplookup_7.png) 
![](iplookup_8.png) 
![](iplookup_9.png) 
![](iplookup_15.png) 

### 1C
We set up remote server and satrted jupyter notebook:
![](jupyter-notebook-start.png) 
After that, we did port forwarding from the remote using the `ssh` command:
![](port-forward.png) 
After that, on the "local" or different laptop, we looked at port:4321 and started the notebook - whose result is shown.
![](jupyter-notebook-result.png) 


## Problem 2

Compile in `/question1` using: `make`

Run using `./server` and `./client`

`details` : User id, total size transmitted, and list of files transferred are all included.

`<filename>`: Checks if the file is on the system and then transmits it to the client.

Objective : Multi-user file transfer programme that can handle very huge files.

Result : With the exception of slight loss in pdf and binary files, we developed a multi-user programme that permitted near-lossless transfer of text and picture data. We were unable to support files larger than 65 KB in size. Active users' information was also kept track of by the programme.

## Problem 3 
